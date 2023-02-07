

#include <JCore/Core.h>
#include <JCore/Sync/Semaphore.h>

#include <cassert>

#include "JCore/Exception.h"

NS_JC_BEGIN

template class LockGuard<Semaphore>;

// 디폴트는 바이너리 세마포어로..
Semaphore::Semaphore():
	m_iUsableCount(1),
	m_iMaxCount(1) {
}

Semaphore::Semaphore(int maxCount, int initialUsableCount) :
	m_iMaxCount(maxCount),
	m_iUsableCount(initialUsableCount) {
	DebugAssert(m_iUsableCount <= m_iMaxCount);
}

void Semaphore::Lock() {
	std::unique_lock ul(m_Mtx);

    while (m_iUsableCount <= 0) {
        m_Condvar.wait(ul);
    }

    --m_iUsableCount;
}

void Semaphore::Unlock() {
	std::unique_lock ul(m_Mtx);

    if (m_iUsableCount < m_iMaxCount) {
        ++m_iUsableCount;
        m_Condvar.notify_one();
        return;
    }

    DebugAssertMsg(false, "몽미");
}

bool Semaphore::TryLock() {
	std::unique_lock ul(m_Mtx);
	if (m_iUsableCount > 0) {
		m_iUsableCount--;
		return true;
	}

	return false;
}

bool Semaphore::IsLocked() {
	std::unique_lock ul(m_Mtx);
	return m_iUsableCount == 0;
}

int Semaphore::UsableCount() {
	std::unique_lock ul(m_Mtx);
	return m_iUsableCount;
}

void Semaphore::Release(int count) {
    for (int i = 0; i < count; i++) 
        Unlock();
}
NS_JC_END
