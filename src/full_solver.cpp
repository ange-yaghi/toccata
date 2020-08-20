#include "../include/full_solver.h"

#include "../include/segment_utilities.h"
#include "../include/nls_optimizer.h"
#include "../include/memory.h"

toccata::FullSolver::FullSolver() {
    m_testPatternBuffer = nullptr;
    m_notesByPitchBuffer = nullptr;
}

toccata::FullSolver::~FullSolver() { 
    /* void */
}

void toccata::FullSolver::Initialize() {
    m_testPatternBuffer = Memory::Allocate<int>(NoteBufferSize);
    m_notesByPitchBuffer = Memory::Allocate2d<int>(MaxPitches, NoteBufferSize);

    TestPatternEvaluator::AllocateMemorySpace(&m_memorySpace, NoteBufferSize, NoteBufferSize, NoteBufferSize);

    m_testPatternGenerator.Seed(0);
}

void toccata::FullSolver::Release() {
    toccata::TestPatternEvaluator::FreeMemorySpace(&m_memorySpace);

    Memory::Free(m_testPatternBuffer);
    Memory::Free2d(m_notesByPitchBuffer);
}

bool toccata::FullSolver::Solve(const Request &request, Result *result) {
	const MusicSegment *reference = request.Reference;
	const MusicSegment *segment = request.Segment;

	SegmentUtilities::SortByPitch(
		segment, request.StartIndex, request.EndIndex, MaxPitches, m_notesByPitchBuffer);

	const int n = reference->NoteContainer.GetCount();

	TestPatternGenerator::TestPatternRequest patternRequest;
	patternRequest.NoteCount = n;
	patternRequest.Buffer = m_testPatternBuffer;
	patternRequest.RequestedPatternSize = request.PatternLength;

	const int patternLength = m_testPatternGenerator.FindRandomTestPattern(patternRequest);

	TestPatternEvaluator::Output output;
	TestPatternEvaluator::Request te_request;
	te_request.Segment = segment;
	te_request.ReferenceSegment = reference;
	te_request.Start = request.StartIndex;
	te_request.End = request.EndIndex;
	te_request.TestPattern = m_testPatternBuffer;
	te_request.TestPatternLength = patternLength;
	te_request.SegmentNotesByPitch = m_notesByPitchBuffer;
	te_request.Memory = m_memorySpace;

	const bool found = toccata::TestPatternEvaluator::Solve(te_request, &output);
	if (!found) return false;

	toccata::NoteMapper::InjectiveMappingRequest mappingRequest;
	mappingRequest.CorrelationThreshold = request.CorrelationThreshold;
	mappingRequest.ReferenceSegment = reference;
	mappingRequest.Segment = segment;
	mappingRequest.Start = request.StartIndex;
	mappingRequest.End = request.EndIndex;
	mappingRequest.Target = m_memorySpace.Mapping;
	mappingRequest.Memory = m_memorySpace.MappingMemory;
	mappingRequest.s = output.s;
	mappingRequest.t = output.t;

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
	bool solvable = toccata::NlsOptimizer::Solve(refineStepRequest, &refinedSolution);
	if (!solvable) return false;

	Comparator::Result solutionData;
	Comparator::Request comparatorRequest;
	comparatorRequest.Mapping = preciseMapping;
	comparatorRequest.Reference = reference;
	comparatorRequest.Segment = segment;
	comparatorRequest.s = refinedSolution.s;
	comparatorRequest.t = refinedSolution.t;
	Comparator::CalculateError(comparatorRequest, &solutionData);

	int missedNotes = n - solutionData.MappedNotes;
	double missedNoteRatio = missedNotes / (double)n;

	if (missedNoteRatio < request.MissingNoteThreshold) {
		result->Fit = solutionData;
		result->s = refinedSolution.s;
		result->t = refinedSolution.t;
		return true;
	}
	else return false;
}
