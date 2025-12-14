#include <JCore/Core.h>
#include <JCore/Sync/Semaphore.h>

NS_JC_BEGIN
template class LockGuard<Semaphore>;

//////////////////////////////////////////////////////////////////////////////////////////
// 디폴트는 바이너리 세마포어로..
Semaphore::Semaphore()
: maxCount_(1)
, usableCount_(1)
{
}

//////////////////////////////////////////////////////////////////////////////////////////
Semaphore::Semaphore(int _maxCount, int _initialUsableCount)
: maxCount_(_maxCount)
, usableCount_(_initialUsableCount)
{
	DebugAssert(usableCount_ <= maxCount_);
}

//////////////////////////////////////////////////////////////////////////////////////////
void Semaphore::Lock()
{
	NormalLockGuard lockGuard(lock_);

	while (usableCount_ <= 0)
	{
		condVar_.Wait(lockGuard);
	}

	--usableCount_;
}

//////////////////////////////////////////////////////////////////////////////////////////
void Semaphore::Unlock()
{
	NormalLockGuard lockGuard(lock_);

	if (usableCount_ < maxCount_)
	{
		++usableCount_;
		condVar_.NotifyOne();
		return;
	}

	DebugAssertMsg(false, "몽미");
}

//////////////////////////////////////////////////////////////////////////////////////////
bool Semaphore::TryLock()
{
	NormalLockGuard lockGuard(lock_);
	if (usableCount_ > 0)
	{
		usableCount_--;
		return true;
	}

	return false;
}

//////////////////////////////////////////////////////////////////////////////////////////
bool Semaphore::IsLocked()
{
	NormalLockGuard lockGuard(lock_);
	return usableCount_ == 0;
}

//////////////////////////////////////////////////////////////////////////////////////////
int Semaphore::UsableCount()
{
	NormalLockGuard lockGuard(lock_);
	return usableCount_;
}

//////////////////////////////////////////////////////////////////////////////////////////
void Semaphore::Release(int _count)
{
	for (int i = 0; i < _count; i++)
		Unlock();
}

NS_JC_END
