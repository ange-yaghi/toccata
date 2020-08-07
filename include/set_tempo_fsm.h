#ifndef TOCCATA_CORE_SET_TEMPO_FSM_H
#define TOCCATA_CORE_SET_TEMPO_FSM_H

#include "fsm.h"

namespace toccata {

    class SetTempoFsm : public Fsm {
    public:
        enum FsmState {
            Digit_1,
            Digit_2,
            Digit_3,

            Deactivated,
        };

    public:
        SetTempoFsm();
        ~SetTempoFsm();

        bool Run(int midiKey, int velocity);
        virtual void OnUpdate();

        int GetTempo() { return m_currentTempo; }
        bool GetForceMetronomeEnable() { return m_forceEnable; }
        bool GetSwitchMetronomeOnOff() { return m_enableDisableMetronome; }

    protected:
        FsmState m_currentState;

        int m_currentTempo;
        bool m_enableDisableMetronome;
        bool m_forceEnable;
    };

} /* namespace toccata */

#endif /* TOCCATA_CORE_SET_TEMPO_FSM_H */
