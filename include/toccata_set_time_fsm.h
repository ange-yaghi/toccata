#ifndef TOCCATA_SET_TIME_FSM_H
#define TOCCATA_SET_TIME_FSM_H

#include "toccata_fsm.h"

class Toccata_SetTimeFSM : public Toccata_FSM
{

public:

	enum FSM_STATE
	{

		STATE_DIGIT_1,

		STATE_DEACTIVATED,

	};

public:

	Toccata_SetTimeFSM();
	~Toccata_SetTimeFSM();

	bool Run(int midiKey, int velocity);
	virtual void UpdateCore();

protected:

	FSM_STATE m_currentState;

	int m_currentTime;

	bool m_enableDisableMetronome;

	bool m_updateCore;

};

#endif