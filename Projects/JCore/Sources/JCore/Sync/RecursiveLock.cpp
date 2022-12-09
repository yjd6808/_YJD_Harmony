/*
 * 작성자: 윤정도
 * =====================
 */



#include <JCore/Core.h>
#include <JCore/Sync/LockGuard.h>
#include <JCore/Sync/RecursiveLock.h>
#include <JCore/Exception.h>

#include <cassert>

namespace JCore {

	template class LockGuard<RecursiveLock>;

	RecursiveLock::RecursiveLock() :
		m_uiLockedThreadId(0),
		m_iRecursion(0) {}

	void RecursiveLock::Lock() {
		const size_t m_uiId = GetCurrentThreadId();

        // 일반 변수를 여러 쓰레드에서 접근해서 undefined behavior라고 생각할 수 있지만
        // 동시에 여러 쓰레드가 Lock()을 통과하더라도 결국 m_Lock.Lock()에서 하나의 쓰레드만 통과 가능하므로 문제가 안된다.
		if (m_uiLockedThreadId != m_uiId) {     
			m_Lock.Lock();
			m_uiLockedThreadId = m_uiId;
			m_iRecursion = 1;
			return;
		}

		++m_iRecursion;
	}

	bool RecursiveLock::TryLock() {
		const size_t m_uiId = GetCurrentThreadId();

		if (m_Lock.TryLock()) {
			m_uiLockedThreadId = m_uiId;
			m_iRecursion = 1;
			return true;
		}

		if (m_uiLockedThreadId == m_uiId) {
			++m_iRecursion;
			return true;
		}

		return false;
	}

	void RecursiveLock::Unlock() {
		const size_t m_uiId = GetCurrentThreadId();
		DebugAssert(m_uiLockedThreadId == m_uiId);
        DebugAssert(m_iRecursion > 0);

		if ((--m_iRecursion) == 0)
		{
			m_uiLockedThreadId = 0;
			m_Lock.Unlock();
		}
	}

	bool RecursiveLock::IsLocked() {
		throw NotImplementedException("RecursiveLock::IsLocked()");
	}

	size_t RecursiveLock::GetCurrentThreadId() {
		return std::hash<std::thread::id>{}(std::this_thread::get_id());
	}
} // namespace JCore