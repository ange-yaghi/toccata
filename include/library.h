#ifndef TOCCATA_CORE_LIBRARY_H
#define TOCCATA_CORE_LIBRARY_H

#include "music_segment.h"
#include "bar.h"
#include "piece.h"

#include <vector>

namespace toccata {

    class Library {
    public:
        Library();
        ~Library();

        MusicSegment *NewSegment();
        MusicSegment *GetSegment(int index) const;
        int GetSegmentCount() const;

        Bar *NewBar();
        Bar *GetBar(int index) const;
        int GetBarCount() const;

        Piece *NewPiece();
        Piece *GetPiece(int index) const;
        int GetPieceCount() const;

    protected:
        std::vector<MusicSegment *> m_segments;
        std::vector<Bar *> m_bars;
        std::vector<Piece *> m_pieces;

        int m_currentBarId;
    };

} /* namespace toccata */

#endif /* TOCCATA_CORE_LIBRARY_H */
