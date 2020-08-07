#ifndef TOCCATA_CORE_MUSIC_SEGMENT_H
#define TOCCATA_CORE_MUSIC_SEGMENT_H

#include "music_point.h"
#include "music_point_container.h"

namespace toccata {

    struct MusicSegment {
        MusicPointContainer NoteContainer;
        double Length;
    };

} /* namespace toccata */

#endif /* TOCCATA_CORE_MUSIC_SEGMENT_H */
