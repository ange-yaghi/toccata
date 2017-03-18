#ifndef TOCCATA_FSM_H
#define TOCCATA_FSM_H

#include <yds_base.h>

#include <mutex>

class Toccata_FSM : public ysObject
{

	friend class Toccata_Core;

public:

	Toccata_FSM(const char *typeID);
	virtual ~Toccata_FSM();

	Toccata_Core *Core() { return m_core; }

	void UpdateCoreTopLevel();

	void Lock();
	void Unlock();

	void SetEnable(bool enable) { Lock(); m_enabled = enable; Unlock(); }
	bool IsEnabled() const { return m_enabled; }

protected:

	virtual void UpdateCore() = 0;

private:

	bool m_enabled;
	std::mutex m_fsmLock;

	Toccata_Core *m_core;

};

#endif