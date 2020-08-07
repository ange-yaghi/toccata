#include "toccata_set_time_fsm.h"

#include "toccata_core.h"

Toccata_SetTimeFSM::Toccata_SetTimeFSM() : Toccata_FSM("Toccata_SetTempoFSM")
{

	m_currentState = STATE_DEACTIVATED;

	m_currentTime = 4;
	m_updateCore = true;

	m_enableDisableMetronome = false;

}

Toccata_SetTimeFSM::~Toccata_SetTimeFSM()
{
}

bool Toccata_SetTimeFSM::Run(int midiKey, int velocity)
{

	if (!IsEnabled()) return false;

	Lock();

	FSM_STATE nextState = m_currentState;

	if (velocity > 0)
	{

		// Highest B is pressed
		if (midiKey == 107)
		{

			nextState = STATE_DIGIT_1;

		}

		if (m_currentState != STATE_DEACTIVATED)
		{

			int amount = 0;
			switch (midiKey)
			{

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

			if (amount == -1)
			{

				nextState = STATE_DIGIT_1;

			}

			else if (amount == -2)
			{

				m_enableDisableMetronome = true;
				nextState = STATE_DIGIT_1;
				m_updateCore = true;

			}

			else
			{

				if (m_currentState == STATE_DIGIT_1)
				{

					m_currentTime = amount;
					m_updateCore = true;
					m_enableDisableMetronome = false;

					nextState = STATE_DEACTIVATED;

				}

			}

		}

	}

	else
	{

		if (m_currentState != STATE_DEACTIVATED)
		{

			if (midiKey == 107)
			{

				nextState = STATE_DEACTIVATED;
				printf("Set Time to %d\n", m_currentTime);

				// No need to update if nothing has changed
				if (m_currentState != STATE_DIGIT_1)
				{

					m_enableDisableMetronome = false;
					m_updateCore = true;

				}

			}

		}

	}

	bool returnValue = false;
	if (m_currentState == STATE_DEACTIVATED)
	{

		if (nextState != STATE_DEACTIVATED)
			returnValue = true;

	}

	else
		returnValue = true;

	m_currentState = nextState;

	Unlock();

	return returnValue;

}

void Toccata_SetTimeFSM::UpdateCore()
{

	if (m_updateCore)
	{

		Core()->SetTime(m_currentTime);

		m_updateCore = false;

	}

}