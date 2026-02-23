/*
 * 작성자: 윤정도
 * =====================
 */



#include <jc/Core.h>
#include <jc/Sync/LockGuard.h>
#include <jc/Sync/RecursiveLock.h>
#include <jc/Exception.h>

#include <cassert>

NS_JC_BEGIN

template class LockGuard<RecursiveLock>;

//////////////////////////////////////////////////////////////////////////////////////////
RecursiveLock::RecursiveLock()
: m_lockedThreadId(0)
, m_recursion(0)
{
}

//////////////////////////////////////////////////////////////////////////////////////////
void RecursiveLock::Lock()
{
	const _u32 lockedThreadId = WinApi::GetCurrentThreadId();

	// 일반 변수를 여러 쓰레드에서 접근해서 undefined behavior라고 생각할 수 있지만
	// 동시에 여러 쓰레드가 Lock()을 통과하더라도 결국 m_lock.Lock()에서 하나의 쓰레드만 통과 가능하므로 문제가 안된다.
	if (m_lockedThreadId != lockedThreadId)
	{
		m_lock.Lock();
		m_lockedThreadId = lockedThreadId;
		m_recursion = 1;
		return;
	}

	++m_recursion;
}

//////////////////////////////////////////////////////////////////////////////////////////
bool RecursiveLock::TryLock()
{
	const _u32 lockedThreadId = WinApi::GetCurrentThreadId();

	if (m_lock.TryLock())
	{
		m_lockedThreadId = lockedThreadId;
		m_recursion = 1;
		return true;
	}

	if (m_lockedThreadId == lockedThreadId)
	{
		++m_recursion;
		return true;
	}

	return false;
}

//////////////////////////////////////////////////////////////////////////////////////////
void RecursiveLock::Unlock()
{
	const _u32 lockedThreadId = WinApi::GetCurrentThreadId();
	jc_assert(m_lockedThreadId == lockedThreadId);
	jc_assert(m_recursion > 0);

	if ((--m_recursion) == 0)
	{
		m_lockedThreadId = 0;
		m_lock.Unlock();
	}
}

//////////////////////////////////////////////////////////////////////////////////////////
bool RecursiveLock::IsLocked()
{
	throw NotImplementedException("RecursiveLock::IsLocked()");
}

/*size_t RecursiveLock::GetCurrentThreadId() {
	return std::hash<std::thread::id>{}(std::this_thread::get_id());
}*/
NS_END