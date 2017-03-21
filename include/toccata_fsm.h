#ifndef TOCCATA_FSM_H
#define TOCCATA_FSM_H

#include <yds_base.h>

#include <mutex>

class Toccata_FSM : public ysObject
{

	friend class Toccata_Core;

public:

	enum ERROR_CODE
	{

		ERROR_NONE = 0x0,
		ERROR_FSM_NOT_LOCKED,

	};

public:

	Toccata_FSM(const char *typeID);
	virtual ~Toccata_FSM();

	void Lock();
	void Unlock();

	virtual void OnUpdate() = 0;

	void SetEnable(bool enable) { Lock(); m_enabled = enable; Unlock(); }
	bool IsEnabled() const { return m_enabled; }

	bool IsStateChanged() const { return m_stateChanged; }
	ERROR_CODE ClearFlag();

protected:

	void SetUpdateNeeded() { m_stateChanged = true; }

private:

	bool m_enabled;
	bool m_stateChanged;
	bool m_locked;

	std::mutex m_fsmLock;

};

#endif