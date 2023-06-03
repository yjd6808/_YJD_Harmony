/*
 * 작성자: 윤정도
 * =====================
 *
 */

#include <JCore/Core.h>
#include <JCore/Exception.h>
#include <JCore/Sync/LockGuard.h>
#include <JCore/Sync/NormalLock.h>
#include <JCore/Threading/Thread.h>


NS_JC_BEGIN
	template class LockGuard<NormalLock>;

NormalLock::NormalLock() {
	InitializeCriticalSection(reinterpret_cast<CRITICAL_SECTION*>(&m_CriticalSection));
}

NormalLock::~NormalLock() {
	DeleteCriticalSection(reinterpret_cast<CRITICAL_SECTION*>(&m_CriticalSection));
}

void NormalLock::Lock() {
	if (m_hOwnThreadId == (int)Thread::GetThreadId()) {
		// DebugAssertMsg(false, "같은 쓰레드에서 재잠금을 시도했습니다.");
		throw RuntimeException("같은 쓰레드에서 재잠금을 시도했습니다.");
	}

	EnterCriticalSection(reinterpret_cast<CRITICAL_SECTION*>(&m_CriticalSection));
	m_hOwnThreadId = Thread::GetThreadId();
}

void NormalLock::Unlock() {
	m_hOwnThreadId = -1;
	LeaveCriticalSection(reinterpret_cast<CRITICAL_SECTION*>(&m_CriticalSection));
}

bool NormalLock::TryLock() {
	
    if (IsLocked())
        return false;

	if ((bool)TryEnterCriticalSection(reinterpret_cast<CRITICAL_SECTION*>(&m_CriticalSection))) {
		m_hOwnThreadId = Thread::GetThreadId();
		return true;
	}
	return false;
}

bool NormalLock::IsLocked() {
	return m_hOwnThreadId != -1;
}

NS_JC_END