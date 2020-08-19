#include "../include/bar.h"

toccata::Bar::Bar() {
    m_segment = nullptr;
}

toccata::Bar::~Bar() {
    /* void */
}

void toccata::Bar::AddNext(int next) {
    m_next.push_back(next);
}

int toccata::Bar::GetNext(int index) {
    return m_next[index];
}
