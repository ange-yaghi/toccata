#ifndef TOCCATA_CORE_SET_TIME_FSM_H
#define TOCCATA_CORE_SET_TIME_FSM_H

#include "fsm.h"

namespace toccata {

    class SetTimeFsm : public Fsm {
    public:
        enum class FsmState {
            Digit_1,
            Wait,
            Deactivated,
        };

    public:
        SetTimeFsm();
        ~SetTimeFsm();

        bool Run(int midiKey, int velocity);
        virtual void OnUpdate();

        int GetCurrentMeter() const { return m_currentTime; }
        bool GetMetronomeEnabled() const { return m_enableDisableMetronome; }

    protected:
        FsmState m_currentState;

        int m_currentTime;
        bool m_enableDisableMetronome;
    };

} /* namespace toccata */

#endif /* TOCCATA_CORE_SET_TIME_FSM_H */
