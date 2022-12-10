/*
 * 작성자: 윤정도
 * =====================
 *
 */

#include <JCore/Core.h>
#include <JCore/Exception.h>
#include <JCore/Sync/LockGuard.h>
#include <JCore/Sync/NormalLock.h>


namespace JCore {

	template class LockGuard<NormalLock>;

	NormalLock::NormalLock() {
		InitializeCriticalSection(&m_CriticalSection);
	}

	NormalLock::~NormalLock() {
		DeleteCriticalSection(&m_CriticalSection);
	}

	void NormalLock::Lock() {
		Int32U uiThreadId = WinApi::GetCurrentThreadId();
		if (uiThreadId == m_uiThreadId.Load())
			throw RuntimeException("이미 잠겨있습니다.");

		EnterCriticalSection(&m_CriticalSection);
		m_uiThreadId = uiThreadId;
	}

	void NormalLock::Unlock() {
		LeaveCriticalSection(&m_CriticalSection);
		m_uiThreadId = 0;
	}

	bool NormalLock::TryLock() {
		
        if (IsLocked())
            return false;

		if ((bool)TryEnterCriticalSection(&m_CriticalSection)) {
			m_uiThreadId = WinApi::GetCurrentThreadId();
			return true;
		}
		return false;
	}

	bool NormalLock::IsLocked() {
		return m_uiThreadId.Load() != 0;
	}

} // namespace JCore