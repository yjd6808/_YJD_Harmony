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

//////////////////////////////////////////////////////////////////////////////////////////
NormalLock::NormalLock()
{
	InitializeCriticalSection(reinterpret_cast<CRITICAL_SECTION*>(&criticalSection_));
}

//////////////////////////////////////////////////////////////////////////////////////////
NormalLock::~NormalLock()
{
	DeleteCriticalSection(reinterpret_cast<CRITICAL_SECTION*>(&criticalSection_));
}

//////////////////////////////////////////////////////////////////////////////////////////
void NormalLock::Lock()
{
	if (ownedThreadId_ == (int)Thread::GetThreadId())
	{
		// DebugAssertMsg(false, "같은 쓰레드에서 재잠금을 시도했습니다.");
		throw RuntimeException("같은 쓰레드에서 재잠금을 시도했습니다.");
	}

	EnterCriticalSection(reinterpret_cast<CRITICAL_SECTION*>(&criticalSection_));
	ownedThreadId_ = Thread::GetThreadId();
}

//////////////////////////////////////////////////////////////////////////////////////////
void NormalLock::Unlock()
{
	ownedThreadId_ = -1;
	LeaveCriticalSection(reinterpret_cast<CRITICAL_SECTION*>(&criticalSection_));
}

//////////////////////////////////////////////////////////////////////////////////////////
bool NormalLock::TryLock()
{
	if (IsLocked())
		return false;

	if ((bool)TryEnterCriticalSection(reinterpret_cast<CRITICAL_SECTION*>(&criticalSection_)))
	{
		ownedThreadId_ = Thread::GetThreadId();
		return true;
	}
	return false;
}

//////////////////////////////////////////////////////////////////////////////////////////
bool NormalLock::IsLocked()
{
	return ownedThreadId_ != -1;
}

NS_JC_END
