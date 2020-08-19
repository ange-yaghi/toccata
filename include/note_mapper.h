#ifndef TOCCATA_CORE_NOTE_MAPPER_H
#define TOCCATA_CORE_NOTE_MAPPER_H

#include "music_segment.h"
#include "munkres_solver.h"

#include <random>

namespace toccata {

    class NoteMapper {
    public:
        struct NNeighborMappingRequest {
            const MusicSegment *ReferenceSegment;
            const MusicSegment *Segment;
            int Start = -1;
            int End = -1;
            double s;
            double t;
            double CorrelationThreshold;
            int *const *NotesByPitch = nullptr; // Optional

            int *Target;
        };

        static int *GetMapping(NNeighborMappingRequest *request);

        struct InjectiveMappingRequest {
            // n = notes in reference
            // m = notes in segment
            // k = max(n, m)

            struct MemorySpace {
                MunkresSolver::Request::MemorySpace MunkresMemory;

                double **Costs; // size = n x k
                bool **Disallowed; // size = n x k
            };

            const MusicSegment *ReferenceSegment;
            const MusicSegment *Segment;

            int Start = -1;
            int End = -1;

            double s;
            double t;
            double CorrelationThreshold;

            MemorySpace Memory;

            int *Target; // size = n
        };

        static int GetClosestNote(const MusicPoint *points, int start, int end, double timestamp, int pitch);
        static int GetClosestNote(const MusicPoint *points, const int *indices, int n, double timestamp);

        static void AllocateMemorySpace(InjectiveMappingRequest::MemorySpace *memory, int referenceNoteCount, int noteCount);
        static void FreeMemorySpace(InjectiveMappingRequest::MemorySpace *memory);
        static int *GetInjectiveMapping(InjectiveMappingRequest *request);
    };

} /* namespace toccata */

#endif /* TOCCATA_CORE_NOTE_MAPPER_H */
