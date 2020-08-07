#include "toccata_fsm.h"

Toccata_FSM::Toccata_FSM(const char *typeID) : ysObject(typeID)
{

	m_core = NULL;
	m_enabled = false;

}

Toccata_FSM::~Toccata_FSM()
{

}

void Toccata_FSM::UpdateCoreTopLevel()
{

	Lock();

	UpdateCore();

	Unlock();

}

void Toccata_FSM::Lock()
{

	m_fsmLock.lock();

}

void Toccata_FSM::Unlock()
{

	m_fsmLock.unlock();

}