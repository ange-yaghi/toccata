#include "../include/test_pattern_evaluator.h"

#include "../include/nls_optimizer.h"
#include "../include/comparator.h"
#include "../include/memory.h"

void toccata::TestPatternEvaluator::AllocateMemorySpace(
    Request::MemorySpace *memory,
    int testPatternSize,
    int referenceSegmentNotes,
    int segmentNotes)
{
    NoteMapper::AllocateMemorySpace(&memory->MappingMemory, referenceSegmentNotes, segmentNotes);

    memory->Stack = Memory::Allocate<int>(testPatternSize);
    memory->p = Memory::Allocate<double>(referenceSegmentNotes);
    memory->r = Memory::Allocate<double>(referenceSegmentNotes);
    memory->Mapping = Memory::Allocate<int>(referenceSegmentNotes);
}

void toccata::TestPatternEvaluator::FreeMemorySpace(Request::MemorySpace *memory) {
    NoteMapper::FreeMemorySpace(&memory->MappingMemory);

    Memory::Free(memory->Stack);
    Memory::Free(memory->r);
    Memory::Free(memory->p);
    Memory::Free(memory->Mapping);
}

bool toccata::TestPatternEvaluator::Solve(const Request &request, Output *output) {
    const int patternLength = request.TestPatternLength;

    const MusicPoint *referencePoints = request.ReferenceSegment->NoteContainer.GetPoints();
    const MusicPoint *points = request.Segment->NoteContainer.GetPoints();

    const int n = request.ReferenceSegment->NoteContainer.GetCount();

    int *mapping = request.Memory.Stack;
    int *const *notesByPitch = request.SegmentNotesByPitch;
    const int *testPattern = request.TestPattern;
    
    for (int i = 0; i < patternLength; ++i) {
        mapping[i] = StackInitialValue;
    }

    double *r = request.Memory.r;
    double *p = request.Memory.p;

    double best_s = 1.0;
    double best_t = 0.0;

    Comparator::Result bestMatchData;
    bestMatchData.AverageError = DBL_MAX;
    bestMatchData.MappedNotes = 0;
    bestMatchData.MappingEnd = -1;
    bestMatchData.MappingStart = -1;
    
    while (true) {
        const bool complete = Advance(request);
        if (complete) break;
        else if (!IsValid(request)) continue;

        int validPointCount = 0;
        for (int i = 0; i < patternLength; ++i) {
            if (mapping[i] != NotFound && mapping[i] != NotMapped) {
                const int refNoteIndex = testPattern[i];
                const MusicPoint &referencePoint = referencePoints[refNoteIndex];

                int noteIndex = notesByPitch[referencePoint.Pitch][mapping[i]];
                const MusicPoint &point = points[noteIndex];

                r[validPointCount] = referencePoint.Timestamp;
                p[validPointCount] = point.Timestamp;

                ++validPointCount;
            }
        }

        NlsOptimizer::Solution solution;
        NlsOptimizer::Problem problem;
        problem.N = validPointCount;
        problem.p_set = p;
        problem.r_set = r;

        bool solvable = NlsOptimizer::Solve(problem, &solution);
        if (!solvable) continue;

        NoteMapper::NNeighborMappingRequest nnMappingRequest;
        nnMappingRequest.s = solution.s;
        nnMappingRequest.t = solution.t;
        nnMappingRequest.Target = request.Memory.Mapping;
        nnMappingRequest.ReferenceSegment = request.ReferenceSegment;
        nnMappingRequest.Start = request.Start;
        nnMappingRequest.End = request.End;
        nnMappingRequest.Segment = request.Segment;
        nnMappingRequest.CorrelationThreshold = 0.1;

        if (UsePitchCachingInMappingStep) {
            nnMappingRequest.NotesByPitch = notesByPitch;
        }
        
        int *fullMapping = NoteMapper::GetMapping(&nnMappingRequest);
        int notesMatched = 0;
        for (int i = 0; i < n; ++i) {
            if (fullMapping[i] != -1) ++notesMatched;
        }

        if (EnablePreciseMapping) {
            NoteMapper::InjectiveMappingRequest mappingRequest;
            mappingRequest.CorrelationThreshold = 0.1;
            mappingRequest.ReferenceSegment = request.ReferenceSegment;
            mappingRequest.Segment = request.Segment;
            mappingRequest.Target = request.Memory.Mapping;
            mappingRequest.Memory = request.Memory.MappingMemory;
            mappingRequest.Start = request.Start;
            mappingRequest.End = request.End;
            mappingRequest.s = solution.s;
            mappingRequest.t = solution.t;

            fullMapping = NoteMapper::GetInjectiveMapping(&mappingRequest);

            notesMatched = 0;
            for (int i = 0; i < n; ++i) {
                if (fullMapping[i] != -1) ++notesMatched;
            }
        }

        if (notesMatched == 0) continue;
        if (notesMatched < bestMatchData.MappedNotes) continue;
        
        Comparator::Result solutionData;
        Comparator::Request comparatorRequest;
        comparatorRequest.Mapping = fullMapping;
        comparatorRequest.Reference = request.ReferenceSegment;
        comparatorRequest.Segment = request.Segment;
        comparatorRequest.s = solution.s;
        comparatorRequest.t = solution.t;

        Comparator::CalculateError(comparatorRequest, &solutionData);

        if (solutionData.AverageError < bestMatchData.AverageError ||
            notesMatched > bestMatchData.MappedNotes)
        {
            bestMatchData = solutionData;
            best_s = solution.s;
            best_t = solution.t;
        }
    }

    if (bestMatchData.MappedNotes == 0) return false;
    else {
        output->AverageError = bestMatchData.AverageError;
        output->MappedNotes = bestMatchData.MappedNotes;
        output->MappingEnd = bestMatchData.MappingEnd;
        output->MappingStart = bestMatchData.MappingStart;
        output->s = best_s;
        output->t = best_t;

        return true;
    }
}

bool toccata::TestPatternEvaluator::Advance(const Request &request) {
    const int patternLength = request.TestPatternLength;
    const int *testPattern = request.TestPattern;
    int *currentMapping = request.Memory.Stack;

    int *const *notesByPitch = request.SegmentNotesByPitch;
    const MusicPoint *referencePoints = request.ReferenceSegment->NoteContainer.GetPoints();

    int activeCount = 0;
    int rollCount = 0;

    for (int i = patternLength - 1; i >= 0; --i) {
        int pitch = referencePoints[testPattern[i]].Pitch;

        int prev = currentMapping[i];
        if (prev == NotFound) continue;

        ++activeCount;

        int next = (prev == StackInitialValue || prev == NotMapped)
            ? 0
            : prev + 1;

        if (prev == NotMapped) ++rollCount;

        if (notesByPitch[pitch][next] == -1) {
            if (prev == StackInitialValue) {
                currentMapping[i] = NotFound;
            }
            else {
                currentMapping[i] = NotMapped;
                break;
            }
        }
        else {
            currentMapping[i] = next;

            if (prev != StackInitialValue && prev != NotMapped) {      
                break;
            }
        } 
    }

    return rollCount == activeCount;
}

bool toccata::TestPatternEvaluator::IsValid(const Request &request) {
    const int patternLength = request.TestPatternLength;
    const int *testPattern = request.TestPattern;

    const int *mapping = request.Memory.Stack;
    int *const *notesByPitch = request.SegmentNotesByPitch;

    const MusicPoint *referencePoints = request.ReferenceSegment->NoteContainer.GetPoints();

    for (int i = 0; i < patternLength; ++i) {
        for (int j = 0; j < patternLength; ++j) {
            if (i == j) continue;
 
            const MusicPoint &p0 = referencePoints[testPattern[i]];
            const MusicPoint &p1 = referencePoints[testPattern[j]];

            const int pitch0 = p0.Pitch;
            const int pitch1 = p1.Pitch;

            if (mapping[i] == NotMapped || mapping[i] == NotFound) continue;
            if (mapping[j] == NotMapped || mapping[j] == NotFound) continue;

            if (notesByPitch[pitch0][mapping[i]] == notesByPitch[pitch1][mapping[j]]) return false;
        }
    }

    return true;
}
