/*
 * 작성자: 윤정도
 * 생성일: 6/3/2023 8:39:55 PM
 * =====================
 * @참고: https://learn.microsoft.com/en-us/windows/win32/sync/using-condition-variables
 * 내 라이브러리에 호환성있는 조건변수 구현
 */


#pragma once

#include <JCore/Sync/NormalLock.h>
#include <JCore/Time.h>

NS_JC_BEGIN

struct CvStatus { enum _ { eTimeout, eNoTimeout }; };

class ConditionVariable final
{
	struct Handle { PVOID Ptr; };
public:
	ConditionVariable();

	void Wait(LockGuard<NormalLock>& lockGuard);

	template <typename TPredicate>
	void Wait(LockGuard<NormalLock>& lockGuard, TPredicate&& predicate) {
		while (!predicate()) { Wait(lockGuard); }
	}

	int WaitFor(LockGuard<NormalLock>& lockGuard, const TimeSpan& ts);

	template <typename TPredicate>
	bool WaitFor(LockGuard<NormalLock>& lockGuard, const TimeSpan& ts, TPredicate&& predicate) {
		while (!predicate()) {
			if (WaitFor(lockGuard, ts) == CvStatus::eTimeout) {
				return predicate();
			}
		}
		return true;
	}


	int WaitUntil(LockGuard<NormalLock>& lockGuard, const DateTime& dt);

	template <typename TPredicate>
	bool WaitUntil(LockGuard<NormalLock>& lockGuard, const DateTime& dt, TPredicate&& predicate) {
		return WaitFor(lockGuard, dt.Diff(DateTime::Now()), predicate);
	}

	void NotifyOne();
	void NotifyAll();
	
private:
	Handle m_Handle;
};

NS_JC_END
