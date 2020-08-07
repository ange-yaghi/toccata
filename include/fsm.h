#ifndef TOCCATA_CORE_FSM_H
#define TOCCATA_CORE_FSM_H

#include "delta.h"

#include <mutex>

namespace toccata {

	class Core;

	class Fsm : public ysObject {
		friend Core;

	public:
		enum ErrorCode {
			None = 0x0,
			FsmNotLocked,
		};

	public:
		Fsm(const char *typeID);
		virtual ~Fsm();

		void Lock();
		void Unlock();

		virtual void OnUpdate() = 0;

		void SetEnable(bool enable) { Lock(); m_enabled = enable; Unlock(); }
		bool IsEnabled() const { return m_enabled; }

		bool IsStateChanged() const { return m_stateChanged; }
		ErrorCode ClearFlag();

	protected:
		void SetUpdateNeeded() { m_stateChanged = true; }

	private:
		bool m_enabled;
		bool m_stateChanged;
		bool m_locked;

		std::mutex m_fsmLock;
	};

} /* namespace toccata */

#endif /* TOCCATA_CORE_FSM_H */
