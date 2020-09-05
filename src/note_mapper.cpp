#include "../include/note_mapper.h"

#include "../include/math.h"
#include "../include/transform.h"
#include "../include/memory.h"

int toccata::NoteMapper::GetClosestNote(
    const MusicSegment *segment, const Transform &coarse, int start, int end, double timestamp, int pitch)
{
    const MusicPoint *points = segment->NoteContainer.GetPoints();

    int prev = -1;
    for (int i = start; i <= end; ++i) {
        if (points[i].Pitch == pitch) {
            if (prev != -1) {
                const double currTimestamp = 
                    segment->Normalize(coarse.Local(points[i].Timestamp));

                if (currTimestamp >= timestamp) {
                    const double prevTimestamp = 
                        segment->Normalize(coarse.Local(points[prev].Timestamp));

                    const double diffPrev = Math::Abs(prevTimestamp - timestamp);
                    const double diffCurr = Math::Abs(currTimestamp - timestamp);

                    return diffPrev < diffCurr
                        ? prev
                        : i;
                }
            }

            prev = i;
        }
    }

    return prev;
}

int toccata::NoteMapper::GetClosestNote(
    const MusicSegment *segment, const Transform &coarse, const int *indices, int n, double timestamp)
{
    const MusicPoint *points = segment->NoteContainer.GetPoints();
    
    int prev = -1;
    for (int i = 0; i < n; ++i) {
        const int index = indices[i];
        if (prev != -1) {
            const double currTimestamp = 
                segment->Normalize(coarse.Local(points[index].Timestamp));

            if (currTimestamp >= timestamp) {
                const double prevTimestamp = 
                    segment->Normalize(coarse.Local(points[prev].Timestamp));

                const double diffPrev = Math::Abs(prevTimestamp - timestamp);
                const double diffCurr = Math::Abs(currTimestamp - timestamp);

                return diffPrev < diffCurr
                    ? prev
                    : index;
            }
        }

        prev = index;
    }

    return prev;
}

int *toccata::NoteMapper::GetMapping(NNeighborMappingRequest *request) {
    assert(request->Start >= 0);
    assert(request->End >= request->Start);

    const Transform coarse = { 1.0, 0.0, request->T.t_coarse };
    const int n = request->ReferenceSegment->NoteContainer.GetCount();

    const MusicSegment *reference = request->ReferenceSegment;
    const MusicSegment *segment = request->Segment;

    const double correlationThreshold = request->CorrelationThreshold;

    const MusicPoint *referencePoints = reference->NoteContainer.GetPoints();
    const MusicPoint *points = segment->NoteContainer.GetPoints();

    for (int i = 0; i < n; ++i) {
        const MusicPoint &referencePoint = referencePoints[i];
        const double refTimestamp =
            request->ReferenceSegment->Normalize(referencePoint.Timestamp);
        const double refTimestampSegmentSpace =
            request->T.inv_f(refTimestamp);

        int closest = -1;
        
        if (request->NotesByPitch == nullptr) {
            closest = GetClosestNote(
                request->Segment,
                coarse,
                request->Start,
                request->End,
                refTimestampSegmentSpace,
                referencePoint.Pitch
            );
        }
        else {
            int n_pitch = 0;
            const int *points = request->NotesByPitch[referencePoint.Pitch];
            while (points[n_pitch] != -1) { ++n_pitch; }

            closest = GetClosestNote(
                request->Segment,
                coarse,
                points,
                n_pitch,
                refTimestampSegmentSpace
            );
        }

        if (closest == -1) {
            request->Target[i] = closest;
        }
        else {
            const MusicPoint &closestPoint = points[closest];
            const double timestamp = segment->Normalize(coarse.Local(closestPoint.Timestamp));
            const double diff = Math::Abs(timestamp - refTimestampSegmentSpace);

            request->Target[i] = (diff < correlationThreshold / request->T.s)
                ? closest
                : -1;
        }
    }

    return request->Target;
}

void toccata::NoteMapper::AllocateMemorySpace(
    InjectiveMappingRequest::MemorySpace *memory, int referenceNoteCount, int noteCount)
{
    const int n = referenceNoteCount;
    const int m = noteCount;
    const int k = m > n ? m : n;

    memory->Costs = Memory::Allocate2d<double>(n, k);
    memory->Disallowed = Memory::Allocate2d<bool>(n, k);

    MunkresSolver::AllocateMemorySpace(&memory->MunkresMemory, n, k);
}

void toccata::NoteMapper::FreeMemorySpace(
    InjectiveMappingRequest::MemorySpace *memory)
{
    Memory::Free2d(memory->Costs);
    Memory::Free2d(memory->Disallowed);
    
    MunkresSolver::FreeMemorySpace(&memory->MunkresMemory);
}

int *toccata::NoteMapper::GetInjectiveMapping(InjectiveMappingRequest *request) {
    assert(request->Start >= 0);
    assert(request->End >= request->Start);

    const MusicSegment *reference = request->ReferenceSegment;
    const MusicSegment *segment = request->Segment;

    const int n = request->ReferenceSegment->NoteContainer.GetCount();
    const int m = request->End - request->Start + 1;
    const int k = m > n ? m : n;

    double **C = request->Memory.Costs;
    bool **D = request->Memory.Disallowed;

    const MusicPoint *referencePoints = request->ReferenceSegment->NoteContainer.GetPoints();
    const MusicPoint *points = request->Segment->NoteContainer.GetPoints();

    const double correlationThreshold = request->CorrelationThreshold;

    for (int i = 0; i < n; ++i) {
        const MusicPoint &referencePoint = referencePoints[i];
        const double refTimestamp = 
            request->ReferenceSegment->Normalize(referencePoint.Timestamp);

        for (int j = 0; j < k; ++j) {
            if (j >= m) {
                C[i][j] = 0.0;
                D[i][j] = true;
            }
            else {
                const MusicPoint &point = points[j + request->Start];
                if (point.Pitch != referencePoint.Pitch) {
                    C[i][j] = 0.0;
                    D[i][j] = true;
                }
                else {
                    const double timestamp = request->T.f(
                        segment->Normalize(request->T.Local(point.Timestamp))
                    );

                    const double diff = Math::Abs(refTimestamp - timestamp);

                    if (diff > correlationThreshold) {
                        C[i][j] = 0.0;
                        D[i][j] = true;
                    }
                    else {
                        C[i][j] = diff;
                        D[i][j] = false;
                    }
                }
            }
        }
    }

    MunkresSolver::Request munkresRequest;
    munkresRequest.n = n;
    munkresRequest.m = k;
    munkresRequest.Costs = C;
    munkresRequest.DisallowedMappings = D;
    munkresRequest.Memory = request->Memory.MunkresMemory;
    munkresRequest.Target = request->Target;

    MunkresSolver::InitializeRequest(&munkresRequest);
    MunkresSolver::Solve(&munkresRequest);

    for (int i = 0; i < n; ++i) {
        if (request->Target[i] != -1) {
            request->Target[i] += request->Start;
        }
    }

    return munkresRequest.Target;
}
