#ifndef TOCCATA_CORE_TEST_PATTERN_GENERATOR_H
#define TOCCATA_CORE_TEST_PATTERN_GENERATOR_H

#include "music_segment.h"

#include <random>

namespace toccata { 

    class TestPatternGenerator {
    public:
        static constexpr int MaxPatternSize = 8;

    public:
        struct TestPatternRequest {
            int *Buffer;
            int RequestedPatternSize;
            int NoteCount;
        };

        struct SafeTestPatternRequest {
            const MusicSegment *Segment;
            int *Buffer;
            int RequestedPatternSize;
        };

    public:
        TestPatternGenerator();
        ~TestPatternGenerator();

        void Seed(unsigned int seed);
        int FindRandomTestPattern(TestPatternRequest &request);
        int FindExtremeTestPattern(TestPatternRequest &request);

    protected:
        std::default_random_engine m_generator;
    };

} /* namespace toccata */

#endif /* TOCCATA_CORE_TEST_PATTERN_GENERATOR_H */
