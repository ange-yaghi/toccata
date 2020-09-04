#ifndef TOCCATA_CORE_NOTE_MAPPER_H
#define TOCCATA_CORE_NOTE_MAPPER_H

#include "music_segment.h"
#include "munkres_solver.h"
#include "transform.h"

#include <random>

namespace toccata {

    class NoteMapper {
    public:
        struct NNeighborMappingRequest {
            const MusicSegment *ReferenceSegment;
            const MusicSegment *Segment;
            int Start = -1;
            int End = -1;
            Transform T;
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

            Transform T;
            double CorrelationThreshold;

            MemorySpace Memory;

            int *Target; // size = n
        };

        static int GetClosestNote(const MusicSegment *segment, const Transform &coarse, int start, int end, double timestamp, int pitch);
        static int GetClosestNote(const MusicSegment *segment, const Transform &coarse, const int *indices, int n, double timestamp);

        static void AllocateMemorySpace(InjectiveMappingRequest::MemorySpace *memory, int referenceNoteCount, int noteCount);
        static void FreeMemorySpace(InjectiveMappingRequest::MemorySpace *memory);
        static int *GetInjectiveMapping(InjectiveMappingRequest *request);
    };

} /* namespace toccata */

#endif /* TOCCATA_CORE_NOTE_MAPPER_H */
