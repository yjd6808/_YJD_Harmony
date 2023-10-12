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
	NormalLockGuard lg(m_Lock);
	while (m_bWriteFlag || m_iReadCount)
		m_CondVar.Wait(lg);

	m_bWriteFlag = true;
}

bool NormalRwLock::TryWriteLock() {
	NormalLockGuard lg(m_Lock);
	if (m_bWriteFlag || m_iReadCount)
		return false;

	m_bWriteFlag = true;
	return true;
}

void NormalRwLock::WriteUnlock() {
	NormalLockGuard lg(m_Lock);
	m_bWriteFlag = false;
	m_CondVar.NotifyAll();
}

bool NormalRwLock::IsWriteLocked() {
	NormalLockGuard lg(m_Lock);
	return m_bWriteFlag;
}

void NormalRwLock::ReadLock() {
	NormalLockGuard lg(m_Lock);
	while (m_bWriteFlag)
		m_CondVar.Wait(lg);

	++m_iReadCount;
}

bool NormalRwLock::TryReadLock() {
	NormalLockGuard lg(m_Lock);
	if (m_bWriteFlag)
		return false;

	++m_iReadCount;
	return true;
}

void NormalRwLock::ReadUnlock() {
	NormalLockGuard lg(m_Lock);
	if (m_iReadCount > 0)
		--m_iReadCount;

	m_CondVar.NotifyAll();
}

bool NormalRwLock::IsReadLocked() {
	NormalLockGuard lg(m_Lock);
	return m_iReadCount > 0;
}


NS_JC_END