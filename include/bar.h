#ifndef TOCCATA_CORE_BAR_H
#define TOCCATA_CORE_BAR_H

#include "music_segment.h"
#include "piece.h"

#include <vector>

namespace toccata {

    class Bar {
    public:
        struct SearchResult {
            int Offset;
            double Distance;
        };

    public:
        Bar();
        ~Bar();

        void AddNext(Bar *next);
        Bar *GetNext(int index) const;
        int GetNextCount() const { return (int)m_next.size(); }

        void SetPiece(Piece *piece) { m_piece = piece; }
        Piece *GetPiece() const { return m_piece; }

        void SetSegment(MusicSegment *segment) { m_segment = segment; }
        MusicSegment *GetSegment() const { return m_segment; }

        void SetId(int id) { m_id = id; }
        int GetId() const { return m_id; }

        void SetIndex(int index) { m_index = index; }
        int GetIndex() const { return m_index; }

        SearchResult FindNext(const Bar *next, int skipsAllowed) const;

    protected:
        std::vector<Bar *> m_next;
        MusicSegment *m_segment;
        Piece *m_piece;

        int m_id;
        int m_index;
    };

} /* namespace toccata */

#endif /* TOCCATA_CORE_BAR_H */
