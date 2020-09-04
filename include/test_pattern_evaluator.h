#ifndef TOCCATA_CORE_TEST_PATTERN_EVALUATOR_H
#define TOCCATA_CORE_TEST_PATTERN_EVALUATOR_H

#include "music_segment.h"
#include "note_mapper.h"
#include "transform.h"

#include <random>

namespace toccata {

    class TestPatternEvaluator {
    public:
        static constexpr bool EnablePreciseMapping = false;
        static constexpr bool UsePitchCachingInMappingStep = true;

    public:
        struct Request {
            struct MemorySpace {
                NoteMapper::InjectiveMappingRequest::MemorySpace MappingMemory;
                int *Stack; // size = test pattern size

                double *r; // size = test pattern size
                double *p; // size = test pattern size

                int *Mapping; // size = # of reference notes
            };

            const MusicSegment *ReferenceSegment;
            const MusicSegment *Segment;

            int Start = -1;
            int End = -1;

            int *const *SegmentNotesByPitch;

            const int *TestPattern;
            int TestPatternLength;

            MemorySpace Memory;
        };

        struct Output {
            Transform T;
            double AverageError;

            int MappedNotes;
            int MappingStart;
            int MappingEnd;
        };

        static void AllocateMemorySpace(
            Request::MemorySpace *memory,
            int testPatternSize,
            int referenceSegmentNotes,
            int segmentNotes
        );

        static void FreeMemorySpace(Request::MemorySpace *memory);
        static bool Solve(const Request &request, Output *output);

    private:
        static const int StackInitialValue = -2;
        static const int NotFound = -1;
        static const int NotMapped = -3;

        static bool Advance(const Request &request);
        static bool IsValid(const Request &request);
    };

} /* namespace toccata */

#endif /* TOCCATA_CORE_TEST_PATTERN_EVALUATOR_H */
