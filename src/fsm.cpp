#include "../include/fsm.h"

toccata::Fsm::Fsm(const char *typeID) : ysObject(typeID) {
    m_stateChanged = false;
    m_enabled = false;
    m_locked = false;
}

toccata::Fsm::~Fsm() {
    /* void */
}

toccata::Fsm::ErrorCode toccata::Fsm::ClearFlag() {
    if (!m_stateChanged) return ErrorCode::None;

    if (!m_locked) {
        return ErrorCode::FsmNotLocked;
    }

    m_stateChanged = false;

    return ErrorCode::None;
}

void toccata::Fsm::Lock() {
    m_fsmLock.lock();
    m_locked = true;
}

void toccata::Fsm::Unlock() {
    m_fsmLock.unlock();
    m_locked = false;
}
