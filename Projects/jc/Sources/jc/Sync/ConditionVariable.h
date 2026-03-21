/*
 * 작성자: 윤정도
 * 생성일: 6/3/2023 8:39:55 PM
 * =====================
 * @참고: https://learn.microsoft.com/en-us/windows/win32/sync/using-condition-variables
 * 내 라이브러리에 호환성있는 조건변수 구현
 */


#pragma once

#include "jc/Sync/NormalLock.h"
#include "jc/Time.h"

NS_JC_BEGIN

struct CvStatus { enum _ { eTimeout, eNoTimeout }; };

class ConditionVariable final
{
	struct Handle { void* Ptr; };
public:
	ConditionVariable();

	void Wait(LockGuard<NormalLock>& _lockGuard);

	template <typename TPredicate>
	void Wait(LockGuard<NormalLock>& _lockGuard, TPredicate&& _predicate) {
		while (!_predicate()) {
			Wait(_lockGuard);
		}
	}

	int WaitFor(LockGuard<NormalLock>& _lockGuard, const TimeSpan& _timeSpan);

	template <typename TPredicate>
	bool WaitFor(LockGuard<NormalLock>& _lockGuard, const TimeSpan& _timeSpan, TPredicate&& _predicate) {
		while (!_predicate()) {
			const int status = WaitFor(_lockGuard, _timeSpan);
			if (status == CvStatus::eTimeout) {
				return _predicate();
			}
		}
		return true;
	}


	int WaitUntil(LockGuard<NormalLock>& _lockGuard, const DateTime& _dateTime);

	template <typename TPredicate>
	bool WaitUntil(LockGuard<NormalLock>& _lockGuard, const DateTime& _dateTime, TPredicate&& _predicate) {
		TimeSpan timeSpan = _dateTime.Diff(DateTime::Now());
		return WaitFor(_lockGuard, timeSpan, _predicate);
	}

	void NotifyOne();
	void NotifyAll();
	
private:
	Handle m_Handle;
};

NS_END
