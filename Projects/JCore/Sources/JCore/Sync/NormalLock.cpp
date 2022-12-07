/*
 * 작성자: 윤정도
 * =====================
 *
 */

#include <JCore/Core.h>
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
        // TODO: 쓰레드 ID 초기화
		EnterCriticalSection(&m_CriticalSection);
		m_bLocked = true;
	}

	void NormalLock::Unlock() {
		LeaveCriticalSection(&m_CriticalSection);
		m_bLocked = false;
	}

	bool NormalLock::TryLock() {
        if (m_bLocked) 
            return false;

		BOOL ret = TryEnterCriticalSection(&m_CriticalSection);
		return m_bLocked = (bool)ret;
	}

	bool NormalLock::IsLocked() {
		return m_bLocked;
	}

} // namespace JCore