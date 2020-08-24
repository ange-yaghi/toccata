#include "../include/bar.h"

toccata::Bar::Bar() {
    m_segment = nullptr;
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

bool toccata::Bar::FindNext(const Bar *next, int skipsAllowed) const {
    for (const Bar *n : m_next) {
        if (n == next) return true;
        else if (skipsAllowed > 0) {
            if (n->FindNext(next, skipsAllowed - 1)) {
                return true;
            }
        }
    }

    return false;
}
