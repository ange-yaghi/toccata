#ifndef TOCCATA_SONG_GENERATOR_H
#define TOCCATA_SONG_GENERATOR_H

#include "library.h"
#include "segment_generator.h"

namespace toccata {

    class SongGenerator {
    public:
        SongGenerator();
        ~SongGenerator();

        void GenerateSong(Library *library, int sections, int sectionBars);

        void Seed(unsigned int seed);

    protected:
        SegmentGenerator m_generator;

        std::default_random_engine m_engine;
    };

} /* namespace toccata */

#endif /* TOCCATA_SONG_GENERATOR_H */
