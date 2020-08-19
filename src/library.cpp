#include "../include/library.h"

toccata::Library::Library() {
    /* void */
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
    m_bars.push_back(newBar);

    return newBar;
}

toccata::Bar *toccata::Library::GetBar(int index) const {
    return m_bars[index];
}

int toccata::Library::GetBarCount() const {
    return (int)m_bars.size();
}
