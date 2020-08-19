#ifndef TOCCATA_CORE_BAR_H
#define TOCCATA_CORE_BAR_H

#include "music_segment.h"

#include <vector>

namespace toccata {

    class Bar {
    public:
        Bar();
        ~Bar();

        void AddNext(int next);
        int GetNext(int index);
        int GetNextCount() const { return (int)m_next.size(); }

        void SetSegment(MusicSegment *segment) { m_segment = segment; }
        MusicSegment *GetSegment() const { return m_segment; }

    protected:
        std::vector<int> m_next;
        MusicSegment *m_segment;
    };

} /* namespace toccata */

#endif /* TOCCATA_CORE_BAR_H */
