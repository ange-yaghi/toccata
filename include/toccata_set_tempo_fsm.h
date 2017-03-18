#ifndef TOCCATA_SET_TEMPO_FSM_H
#define TOCCATA_SET_TEMPO_FSM_H

#include "toccata_fsm.h"

class Toccata_SetTempoFSM : public Toccata_FSM
{

public:

	enum FSM_STATE
	{

		STATE_DIGIT_1,
		STATE_DIGIT_2,
		STATE_DIGIT_3,

		STATE_DEACTIVATED,

	};

public:

	Toccata_SetTempoFSM();
	~Toccata_SetTempoFSM();

	bool Run(int midiKey, int velocity);
	virtual void UpdateCore();

protected:

	FSM_STATE m_currentState;

	int m_currentTempo;
	bool m_enableDisableMetronome;
	bool m_forceEnable;

	bool m_updateCore;

};

#endif