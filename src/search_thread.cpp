#include "../include/search_thread.h"

#include "../include/memory.h"
#include "../include/test_pattern_generator.h"
#include "../include/segment_utilities.h"
#include "../include/nls_optimizer.h"
#include "../include/comparator.h"

toccata::SearchThread::SearchThread() {
    m_searchStart = -1;
    m_searchEnd = -1;

    m_testPatternBuffer = nullptr;
    m_notesByPitchBuffer = nullptr;
}

toccata::SearchThread::~SearchThread() {
    /* void */
}

void toccata::SearchThread::Initialize(int searchStart, int searchEnd) {
    m_searchStart = searchStart;
    m_searchEnd = searchEnd;

    m_testPatternBuffer = Memory::Allocate<int>(NoteBufferSize);
    m_notesByPitchBuffer = Memory::Allocate2d<int>(MaxPitches, NoteBufferSize);

    TestPatternEvaluator::AllocateMemorySpace(&m_memorySpace, NoteBufferSize, NoteBufferSize, NoteBufferSize);

    m_testPatternGenerator.Seed(0);
}

void toccata::SearchThread::Release() {
	toccata::TestPatternEvaluator::FreeMemorySpace(&m_memorySpace);

	Memory::Free(m_testPatternBuffer);
	Memory::Free2d(m_notesByPitchBuffer);
}

void toccata::SearchThread::Search(const MusicSegment *segment, const Library *library, Result *result) {
	double minError = DBL_MAX;
	double minErrorRate = INT_MAX;
	int best = -1;
	Transform best_T;
	
	for (int i = m_searchStart; i <= m_searchEnd; ++i) {
		Transform coarse;
		coarse.s = 1.0;
		coarse.t = 0.0;
		coarse.t_coarse = segment->NoteContainer.GetPoints()[i].Timestamp;

        const MusicSegment *reference = library->GetSegment(i);
        const int n = reference->NoteContainer.GetCount();

        SegmentUtilities::SortByPitch(segment, 0, segment->NoteContainer.GetCount() - 1, MaxPitches, m_notesByPitchBuffer);

        TestPatternGenerator::TestPatternRequest patternRequest;
        patternRequest.NoteCount = n;
        patternRequest.Buffer = m_testPatternBuffer;
        patternRequest.RequestedPatternSize = 4;

        int patternLength = m_testPatternGenerator.FindRandomTestPattern(patternRequest);

        TestPatternEvaluator::Output output;
        TestPatternEvaluator::Request request;
        request.Segment = segment;
        request.ReferenceSegment = reference;
		request.Start = 0;
		request.End = segment->NoteContainer.GetCount() - 1;
        request.TestPattern = m_testPatternBuffer;
        request.TestPatternLength = patternLength;
        request.SegmentNotesByPitch = m_notesByPitchBuffer;
		request.Memory = m_memorySpace;

        const bool found = toccata::TestPatternEvaluator::Solve(request, &output);

		if (!found) continue;

		double current_s = output.T.s;
		double current_t = output.T.t;

		toccata::NoteMapper::InjectiveMappingRequest mappingRequest;
		mappingRequest.CorrelationThreshold = 0.1;
		mappingRequest.ReferenceSegment = reference;
		mappingRequest.Segment = segment;
		mappingRequest.Start = 0;
		mappingRequest.End = segment->NoteContainer.GetCount() - 1;
		mappingRequest.Target = m_memorySpace.Mapping;
		mappingRequest.Memory = m_memorySpace.MappingMemory;
		mappingRequest.T.s = current_s;
		mappingRequest.T.t = current_t;
		mappingRequest.T.t_coarse = coarse.t_coarse;

		const int *preciseMapping = toccata::NoteMapper::GetInjectiveMapping(&mappingRequest);

		int validPointCount = 0;
		double *r = m_memorySpace.r;
		double *p = m_memorySpace.p;
		const MusicPoint *referencePoints = reference->NoteContainer.GetPoints();
		const MusicPoint *points = segment->NoteContainer.GetPoints();
		for (int i = 0; i < n; ++i) {
			if (preciseMapping[i] != -1) {
				const int noteIndex = preciseMapping[i];

				r[validPointCount] = reference->Normalize(referencePoints[i].Timestamp);
				p[validPointCount] = segment->Normalize(points[noteIndex].Timestamp);

				++validPointCount;
			}
		}

		toccata::NlsOptimizer::Solution refinedSolution;
		toccata::NlsOptimizer::Problem refineStepRequest;
		refineStepRequest.N = validPointCount;
		refineStepRequest.r_set = r;
		refineStepRequest.p_set = p;
		toccata::NlsOptimizer::Solve(refineStepRequest, &refinedSolution);

		current_s = refinedSolution.s;
		current_t = refinedSolution.t;

		Comparator::Result solutionError;
		Comparator::Request comparatorRequest;
		comparatorRequest.Mapping = preciseMapping;
		comparatorRequest.Reference = request.ReferenceSegment;
		comparatorRequest.Segment = request.Segment;
		comparatorRequest.T.s = refinedSolution.s;
		comparatorRequest.T.t = refinedSolution.t;
		comparatorRequest.T.t_coarse = coarse.t_coarse;

		Comparator::CalculateError(comparatorRequest, &solutionError);

		const int missedNotes = n - solutionError.MappedNotes;
		const int footprint = solutionError.MappingEnd - solutionError.MappingStart + 1;
		const int addedNotes = footprint - solutionError.MappedNotes;
		const int errors = addedNotes + missedNotes;

		const double errorRate = errors / (double)n;

		if (errorRate < minErrorRate) {
			minError = solutionError.AverageError;
			minErrorRate = errorRate;
			best = i;
			best_T.s = refinedSolution.s;
			best_T.t = refinedSolution.t;
			best_T.t_coarse = coarse.t_coarse;
		}
    }

	if (best != -1) {
		result->MatchedSegment = library->GetSegment(best);
		result->Error = minError;
		result->T = best_T;
		result->MatchedNotes = 0;
	}
	else {
		result->MatchedSegment = nullptr;
	}
}
