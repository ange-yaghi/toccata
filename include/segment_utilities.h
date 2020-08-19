#ifndef TOCCATA_CORE_SEGMENT_UTILITIES_H
#define TOCCATA_CORE_SEGMENT_UTILITIES_H

#include "music_segment.h"

#include <random>

namespace toccata {

    class SegmentUtilities {
    public:
        static void SortByPitch(const MusicSegment *segment, int start, int end, int pitchCount, int **target);
    };

} /* namespace toccata */

#endif /* TOCCATA_CORE_SEGMENT_UTILITIES_H */
