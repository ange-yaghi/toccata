#include "../include/set_tempo_fsm.h"

toccata::SetTempoFsm::SetTempoFsm() : Fsm("SetTempoFsm") {
    m_currentState = FsmState::Deactivated;

    m_currentTempo = 0;
    m_enableDisableMetronome = false;
    m_forceEnable = false;
}

toccata::SetTempoFsm::~SetTempoFsm() {
    /* void */
}

bool toccata::SetTempoFsm::Run(int midiKey, int velocity) {
    if (!IsEnabled()) return false;

    Lock();

    FsmState nextState = m_currentState;

    if (velocity > 0) {
        // Highest C is pressed
        if (midiKey == 108) {
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
                m_currentTempo = 0;
            }
            else if (amount == -2) {
                m_enableDisableMetronome = true;
                nextState = FsmState::Digit_1;
                
                SetUpdateNeeded();
            }
            else {
                if (m_currentState == FsmState::Digit_1) {
                    m_currentTempo = amount;
                    nextState = FsmState::Digit_2;
                }
                else if (m_currentState == FsmState::Digit_2) {
                    m_currentTempo *= 10;
                    m_currentTempo += amount;
                    nextState = FsmState::Digit_3;
                }
                else if (m_currentState == FsmState::Digit_3) {
                    m_currentTempo *= 10;
                    m_currentTempo += amount;
                    nextState = FsmState::Deactivated;

                    SetUpdateNeeded();
                    
                    m_forceEnable = true;
                }
            }
        }
    }
    else {
        if (m_currentState != FsmState::Deactivated) {
            if (midiKey == 108) {
                nextState = FsmState::Deactivated;

                // No need to update if nothing has changed
                if (m_currentState != FsmState::Digit_1) {

                    m_forceEnable = true;
                    SetUpdateNeeded();
                }
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

void toccata::SetTempoFsm::OnUpdate() {
    if (IsStateChanged()) {
        m_forceEnable = false;
        m_enableDisableMetronome = false;
    }
}
