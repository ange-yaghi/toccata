#include "../include/bar.h"

toccata::Bar::Bar() {
    m_segment = nullptr;
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
