#ifndef TOCCATA_CORE_BAR_H
#define TOCCATA_CORE_BAR_H

#include "music_segment.h"

#include <vector>

namespace toccata {

    class Bar {
    public:
        Bar();
        ~Bar();

        void AddNext(Bar *next);
        Bar *GetNext(int index) const;
        int GetNextCount() const { return (int)m_next.size(); }

        void SetSegment(MusicSegment *segment) { m_segment = segment; }
        MusicSegment *GetSegment() const { return m_segment; }

        void SetId(int id) { m_id = id; }
        int GetId() const { return m_id; }

        bool FindNext(const Bar *next, int skipsAllowed) const;

    protected:
        std::vector<Bar *> m_next;
        MusicSegment *m_segment;

        int m_id;
    };

} /* namespace toccata */

#endif /* TOCCATA_CORE_BAR_H */
