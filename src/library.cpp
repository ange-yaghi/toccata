#include "../include/library.h"

toccata::Library::Library() {
    m_currentBarId = 0;
}

toccata::Library::~Library() {
    /* void */
}

toccata::MusicSegment *toccata::Library::NewSegment() {
    MusicSegment *newSegment = new MusicSegment;
    m_segments.push_back(newSegment);
    
    return newSegment;
}

toccata::MusicSegment *toccata::Library::GetSegment(int index) const {
    return m_segments[index];
}

int toccata::Library::GetSegmentCount() const {
    return (int)m_segments.size();
}

toccata::Bar *toccata::Library::NewBar() {
    Bar *newBar = new Bar;
    newBar->SetId(m_currentBarId++);
    m_bars.push_back(newBar);

    return newBar;
}

toccata::Bar *toccata::Library::GetBar(int index) const {
    return m_bars[index];
}

int toccata::Library::GetBarCount() const {
    return (int)m_bars.size();
}

toccata::Piece *toccata::Library::NewPiece() {
    Piece *newPiece = new Piece;
    m_pieces.push_back(newPiece);

    return newPiece;
}

toccata::Piece *toccata::Library::GetPiece(int index) const {
    return m_pieces[index];
}

int toccata::Library::GetPieceCount() const {
    return (int)m_pieces.size();
}
