/*
 * 작성자: 윤정도
 * =====================
 *
 */

#include <JCore/Core.h>
#include <JCore/Sync/EventLock.h>

namespace JCore {

	template class LockGuard<EventLock>;

	EventLock::EventLock(): m_LockEvent(true, false) {}
	EventLock::~EventLock() {}

	void EventLock::Lock() {
		m_LockEvent.Wait();
		m_bLocked = true;
	}

	void EventLock::Unlock() {
        DebugAssertMsg(m_bLocked.Load(), "이미 잠금이 해제된 상태입니다.");
        m_bLocked = false;
        m_LockEvent.Signal();
	}

	bool EventLock::TryLock() {
		if (m_bLocked)
			return false;

		Lock();
		return true;
	}

	bool EventLock::IsLocked() {
		return m_bLocked;
	}

}