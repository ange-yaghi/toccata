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
	double best_s = 0.0;
	double best_t = 0.0;
	
	for (int i = m_searchStart; i <= m_searchEnd; ++i) {
        const MusicSegment *reference = library->GetSegment(i);

        int n = reference->NoteContainer.GetCount();

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

        bool found = toccata::TestPatternEvaluator::Solve(request, &output);

		if (!found) continue;

		double current_s = output.s;
		double current_t = output.t;

		toccata::NoteMapper::InjectiveMappingRequest mappingRequest;
		mappingRequest.CorrelationThreshold = 0.1;
		mappingRequest.ReferenceSegment = reference;
		mappingRequest.Segment = segment;
		mappingRequest.Start = 0;
		mappingRequest.End = segment->NoteContainer.GetCount() - 1;
		mappingRequest.Target = m_memorySpace.Mapping;
		mappingRequest.Memory = m_memorySpace.MappingMemory;
		mappingRequest.s = current_s;
		mappingRequest.t = current_t;

		const int *preciseMapping = toccata::NoteMapper::GetInjectiveMapping(&mappingRequest);

		int validPointCount = 0;
		double *r = m_memorySpace.r;
		double *p = m_memorySpace.p;
		const MusicPoint *referencePoints = reference->NoteContainer.GetPoints();
		const MusicPoint *points = segment->NoteContainer.GetPoints();
		for (int i = 0; i < n; ++i) {
			if (preciseMapping[i] != -1) {
				const int noteIndex = preciseMapping[i];

				const MusicPoint &referencePoint = referencePoints[i];
				const MusicPoint &point = points[noteIndex];

				r[validPointCount] = referencePoint.Timestamp;
				p[validPointCount] = point.Timestamp;

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
		comparatorRequest.s = refinedSolution.s;
		comparatorRequest.t = refinedSolution.t;

		Comparator::CalculateError(comparatorRequest, &solutionError);

		int missedNotes = n - solutionError.MappedNotes;
		int footprint = solutionError.MappingEnd - solutionError.MappingStart + 1;
		int addedNotes = footprint - solutionError.MappedNotes;
		int errors = addedNotes + missedNotes;

		double errorRate = errors / (double)n;

		if (errorRate < minErrorRate)
		{
			minError = solutionError.AverageError;
			minErrorRate = errorRate;
			best = i;
			best_s = refinedSolution.s;
			best_t = refinedSolution.t;
		}
    }

	if (best != -1) {
		result->MatchedSegment = library->GetSegment(best);
		result->Error = minError;
		result->s = best_s;
		result->t = best_t;
		result->MatchedNotes = 0;
	}
	else {
		result->MatchedSegment = nullptr;
	}
}
