#ifndef TOCCATA_SONG_GENERATOR_H
#define TOCCATA_SONG_GENERATOR_H

#include "library.h"

namespace toccata {

    class SongGenerator {
    public:
        static void GenerateSong(Library *library, int sections, int sectionBars);
    };

} /* namespace toccata */

#endif /* TOCCATA_SONG_GENERATOR_H */
