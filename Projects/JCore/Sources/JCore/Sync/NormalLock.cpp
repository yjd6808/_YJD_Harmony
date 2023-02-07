/*
 * 작성자: 윤정도
 * =====================
 *
 */

#include <JCore/Core.h>
#include <JCore/Exception.h>
#include <JCore/Sync/LockGuard.h>
#include <JCore/Sync/NormalLock.h>


NS_JC_BEGIN

template class LockGuard<NormalLock>;

NormalLock::NormalLock() {
	InitializeCriticalSection(reinterpret_cast<CRITICAL_SECTION*>(&m_CriticalSection));
}

NormalLock::~NormalLock() {
	DeleteCriticalSection(reinterpret_cast<CRITICAL_SECTION*>(&m_CriticalSection));
}

void NormalLock::Lock() {
	if (m_hOwnThread != nullptr)
		throw RuntimeException("이미 잠겨있습니다.");

	EnterCriticalSection(reinterpret_cast<CRITICAL_SECTION*>(&m_CriticalSection));
	m_hOwnThread = m_CriticalSection.OwningThread;
}

void NormalLock::Unlock() {
	m_hOwnThread.Store(nullptr);
	LeaveCriticalSection(reinterpret_cast<CRITICAL_SECTION*>(&m_CriticalSection));
}

bool NormalLock::TryLock() {
	
    if (IsLocked())
        return false;

	if ((bool)TryEnterCriticalSection(reinterpret_cast<CRITICAL_SECTION*>(&m_CriticalSection))) {
		m_hOwnThread = m_CriticalSection.OwningThread;
		return true;
	}
	return false;
}

bool NormalLock::IsLocked() {
	return m_hOwnThread != nullptr;
}

NS_JC_END