#include "toccata_fsm.h"

Toccata_FSM::Toccata_FSM(const char *typeID) : ysObject(typeID)
{

	m_stateChanged = false;
	m_enabled = false;
	m_locked = false;

}

Toccata_FSM::~Toccata_FSM()
{

}

//void Toccata_FSM::UpdateCoreTopLevel()
//{
//
//	Lock();
//
//	UpdateCore();
//
//	Unlock();
//
//}

Toccata_FSM::ERROR_CODE Toccata_FSM::ClearFlag()
{

	if (!m_stateChanged) return ERROR_NONE;

	if (!m_locked)
	{

		return ERROR_FSM_NOT_LOCKED;

	}

	m_stateChanged = false;

	return ERROR_NONE;

}

void Toccata_FSM::Lock()
{

	m_fsmLock.lock();
	m_locked = true;

}

void Toccata_FSM::Unlock()
{

	m_fsmLock.unlock();
	m_locked = false;

}