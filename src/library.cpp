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
