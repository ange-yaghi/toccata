#include "../include/bar.h"

toccata::Bar::Bar() {
    m_segment = nullptr;
    m_piece = nullptr;
    m_id = -1;
}

toccata::Bar::~Bar() {
    /* void */
}

void toccata::Bar::AddNext(Bar *next) {
    m_next.push_back(next);
}

toccata::Bar *toccata::Bar::GetNext(int index) const {
    return m_next[index];
}

toccata::Bar::SearchResult toccata::Bar::FindNext(const Bar *next, int skipsAllowed) const {
    const double length = GetSegment()->GetNormalizedLength();
    for (const Bar *n : m_next) {
        if (n == next) return { 0, 0.0 };
        else if (skipsAllowed > 0) {
            const int nextLength = next->GetSegment()->NoteContainer.GetCount();
            const SearchResult result = n->FindNext(next, skipsAllowed - 1);
            if (result.Offset != -1) {
                return { result.Offset + nextLength, result.Distance + length };
            }
        }
    }

    return { -1, -1 };
}
