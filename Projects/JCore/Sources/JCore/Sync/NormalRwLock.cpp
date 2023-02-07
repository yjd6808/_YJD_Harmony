/*
 * 작성자: 윤정도
 * =====================
 *
 */

#include <JCore/Core.h>
#include <JCore/Sync/NormalRwLock.h>

NS_JC_BEGIN

template class RwLockGuard<NormalRwLock, RwLockMode::Write>;
template class RwLockGuard<NormalRwLock, RwLockMode::Read>;

NormalRwLock::NormalRwLock() :
	m_iReadCount(0),
	m_bWriteFlag(false) {}

void NormalRwLock::WriteLock() {
	std::unique_lock lg(m_Mtx);
	while (m_bWriteFlag || m_iReadCount)
		m_Condvar.wait(lg);

	m_bWriteFlag = true;
}

bool NormalRwLock::TryWriteLock() {
	std::unique_lock lg(m_Mtx);
	if (m_bWriteFlag || m_iReadCount)
		return false;

	m_bWriteFlag = true;
	return true;
}

void NormalRwLock::WriteUnlock() {
	std::unique_lock lg(m_Mtx);
	m_bWriteFlag = false;
	m_Condvar.notify_all();
}

bool NormalRwLock::IsWriteLocked() {
	std::unique_lock lg(m_Mtx);
	return m_bWriteFlag;
}

void NormalRwLock::ReadLock() {
	std::unique_lock lg(m_Mtx);
	while (m_bWriteFlag)
		m_Condvar.wait(lg);

	++m_iReadCount;
}

bool NormalRwLock::TryReadLock() {
	std::unique_lock lg(m_Mtx);
	if (m_bWriteFlag)
		return false;

	++m_iReadCount;
	return true;
}

void NormalRwLock::ReadUnlock() {
	std::unique_lock lg(m_Mtx);
	if (m_iReadCount > 0)
		--m_iReadCount;

	m_Condvar.notify_all();
}

bool NormalRwLock::IsReadLocked() {
	std::unique_lock lg(m_Mtx);
	return m_iReadCount > 0;
}


NS_JC_END