#include "../include/set_time_fsm.h"

toccata::SetTimeFsm::SetTimeFsm() : Fsm("Toccata_SetTempoFSM") {
    m_currentState = FsmState::Deactivated;
    m_currentTime = 0;
    m_enableDisableMetronome = false;
}

toccata::SetTimeFsm::~SetTimeFsm() {
    /* void */
}

bool toccata::SetTimeFsm::Run(int midiKey, int velocity) {
    if (!IsEnabled()) return false;

    Lock();

    FsmState nextState = m_currentState;

    if (velocity > 0) {
        // Highest B is pressed
        if (midiKey == 107) {
            nextState = FsmState::Digit_1;
        }

        if (m_currentState != FsmState::Deactivated) {
            int amount = 0;
            switch (midiKey) {
                case 64: amount = -2; break;
                case 65: amount = 0; break;
                case 66: amount = 1; break;
                case 67: amount = 2; break;
                case 68: amount = 3; break;
                case 69: amount = 4; break;
                case 70: amount = 5; break;
                case 71: amount = 6; break;
                case 72: amount = 7; break;
                case 73: amount = 8; break;
                case 74: amount = 9; break;
                default: amount = -1; break;
            }

            if (amount == -1) {
                nextState = FsmState::Digit_1;
            }
            else if (amount == -2) {
                m_enableDisableMetronome = true;
                nextState = FsmState::Digit_1;

                SetUpdateNeeded();
            }
            else {
                if (m_currentState == FsmState::Digit_1) {
                    m_currentTime = amount;
                    nextState = FsmState::Wait;

                    SetUpdateNeeded();
                }
                else if (m_currentState == FsmState::Wait) {
                    m_currentTime = amount;
                    nextState = FsmState::Wait;

                    SetUpdateNeeded();
                }
            }
        }
    }
    else {
        if (m_currentState != FsmState::Deactivated) {
            if (midiKey == 107) {
                nextState = FsmState::Deactivated;
            }
        }
    }

    bool returnValue = false;
    if (m_currentState == FsmState::Deactivated) {
        if (nextState != FsmState::Deactivated) {
            returnValue = true;
        }
    }
    else {
        returnValue = true;
    }

    m_currentState = nextState;

    Unlock();

    return returnValue;
}

void toccata::SetTimeFsm::OnUpdate() {
    m_enableDisableMetronome = false;
}
