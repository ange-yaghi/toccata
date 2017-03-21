#include "toccata_set_tempo_fsm.h"

#include "toccata_core.h"

Toccata_SetTempoFSM::Toccata_SetTempoFSM() : Toccata_FSM("Toccata_SetTempoFSM")
{

	m_currentState = STATE_DEACTIVATED;

	m_currentTempo = 0;
	m_enableDisableMetronome = false;
	m_forceEnable = false;

}

Toccata_SetTempoFSM::~Toccata_SetTempoFSM()
{
}

bool Toccata_SetTempoFSM::Run(int midiKey, int velocity)
{

	if (!IsEnabled()) return false;

	Lock();

	FSM_STATE nextState = m_currentState;

	if (velocity > 0)
	{

		// Highest C is pressed
		if (midiKey == 108)
		{

			nextState = STATE_DIGIT_1;

			//SoundSystem.Test();

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
				m_currentTempo = 0;

			}

			else if (amount == -2)
			{

				m_enableDisableMetronome = true;
				nextState = STATE_DIGIT_1;
				
				SetUpdateNeeded();

			}

			else
			{

				if (m_currentState == STATE_DIGIT_1)
				{

					m_currentTempo = amount;
					nextState = STATE_DIGIT_2;

				}

				else if (m_currentState == STATE_DIGIT_2)
				{

					m_currentTempo *= 10;
					m_currentTempo += amount;
					nextState = STATE_DIGIT_3;

				}

				else if (m_currentState == STATE_DIGIT_3)
				{

					m_currentTempo *= 10;
					m_currentTempo += amount;
					nextState = STATE_DEACTIVATED;

					//printf("Set Tempo to %d\n", m_currentTempo);
					SetUpdateNeeded();

					m_forceEnable = true;

				}

			}

		}

	}

	else
	{

		if (m_currentState != STATE_DEACTIVATED)
		{

			if (midiKey == 108)
			{

				nextState = STATE_DEACTIVATED;
				//printf("Set Tempo to %d\n", m_currentTempo);

				// No need to update if nothing has changed
				if (m_currentState != STATE_DIGIT_1)
				{

					m_forceEnable = true;
					SetUpdateNeeded();

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

void Toccata_SetTempoFSM::OnUpdate()
{

	if (IsStateChanged())
	{

		//Core()->SetTempo(m_currentTempo);

		//if (m_forceEnable)
		//	Core()->EnableMetronome(true);

		//else if (m_enableDisableMetronome)
		//	Core()->EnableMetronome(!Core()->IsMetronomeEnabled());

		m_forceEnable = false;
		m_enableDisableMetronome = false;
		//m_updateCore = false;

	}

}