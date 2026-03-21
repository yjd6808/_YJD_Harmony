/*
 * 작성자: 윤정도
 * =====================
 *
 */

#include "jc/Sync/NormalRwLock.h"

NS_JC_BEGIN

template class RwLockGuard<NormalRwLock, RwLockMode::Write>;
template class RwLockGuard<NormalRwLock, RwLockMode::Read>;

//////////////////////////////////////////////////////////////////////////////////////////
NormalRwLock::NormalRwLock()
	: m_readCount(0)
	, m_writeFlag(false)
{
}

//////////////////////////////////////////////////////////////////////////////////////////
void NormalRwLock::WriteLock()
{
	NormalLockGuard lockGuard(m_lock);
	while (m_writeFlag || m_readCount)
	{
		m_condVar.Wait(lockGuard);
	}

	m_writeFlag = true;
}

//////////////////////////////////////////////////////////////////////////////////////////
bool NormalRwLock::TryWriteLock()
{
	NormalLockGuard lockGuard(m_lock);
	if (m_writeFlag || m_readCount)
	{
		return false;
	}

	m_writeFlag = true;
	return true;
}

//////////////////////////////////////////////////////////////////////////////////////////
void NormalRwLock::WriteUnlock()
{
	NormalLockGuard lockGuard(m_lock);
	m_writeFlag = false;
	m_condVar.NotifyAll();
}

//////////////////////////////////////////////////////////////////////////////////////////
bool NormalRwLock::IsWriteLocked()
{
	NormalLockGuard lockGuard(m_lock);
	return m_writeFlag;
}

//////////////////////////////////////////////////////////////////////////////////////////
void NormalRwLock::ReadLock()
{
	NormalLockGuard lockGuard(m_lock);
	while (m_writeFlag)
	{
		m_condVar.Wait(lockGuard);
	}

	++m_readCount;
}

//////////////////////////////////////////////////////////////////////////////////////////
bool NormalRwLock::TryReadLock()
{
	NormalLockGuard lockGuard(m_lock);
	if (m_writeFlag)
	{
		return false;
	}

	++m_readCount;
	return true;
}

//////////////////////////////////////////////////////////////////////////////////////////
void NormalRwLock::ReadUnlock()
{
	NormalLockGuard lockGuard(m_lock);
	if (m_readCount > 0)
	{
		--m_readCount;
	}

	m_condVar.NotifyAll();
}

//////////////////////////////////////////////////////////////////////////////////////////
bool NormalRwLock::IsReadLocked()
{
	NormalLockGuard lockGuard(m_lock);
	return m_readCount > 0;
}


NS_END
