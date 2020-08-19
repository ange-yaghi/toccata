#ifndef TOCCATA_CORE_LIBRARY_H
#define TOCCATA_CORE_LIBRARY_H

#include "music_segment.h"

#include <vector>

namespace toccata {

    class Library {
    public:
        Library();
        ~Library();

        MusicSegment *NewSegment();
        MusicSegment *GetSegment(int index) const;
        int GetSegmentCount() const;

    protected:
        std::vector<MusicSegment *> m_segments;
    };

} /* namespace toccata */

#endif /* TOCCATA_CORE_LIBRARY_H */
