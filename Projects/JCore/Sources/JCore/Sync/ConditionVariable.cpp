/*
 * 작성자: 윤정도
 * 생성일: 6/3/2023 8:39:55 PM
 */



#include <JCore/Core.h>
#include <JCore/Sync/ConditionVariable.h>

NS_JC_BEGIN

ConditionVariable::ConditionVariable() { InitializeConditionVariable((PCONDITION_VARIABLE)&m_Handle); }

void ConditionVariable::Wait(LockGuard<NormalLock>& lockGuard) {
	if (!SleepConditionVariableCS((PCONDITION_VARIABLE)&m_Handle, (PCRITICAL_SECTION)&lockGuard.Handle()->m_CriticalSection, INFINITE)) {
		DebugAssert(false);
	}
}

int ConditionVariable::WaitFor(LockGuard<NormalLock>& lockGuard, const TimeSpan& ts) {
	int ms = ts.GetTotalMiliSecondsInt();
	if (ms <= 0) { return CvStatus::eTimeout; }
	if (!SleepConditionVariableCS((PCONDITION_VARIABLE)&m_Handle, (PCRITICAL_SECTION)&lockGuard.Handle()->m_CriticalSection, ms)) {
		return CvStatus::eTimeout;
	}
	return CvStatus::eNoTimeout;
}

int ConditionVariable::WaitUntil(LockGuard<NormalLock>& lockGuard, const DateTime& dt) {
	return WaitFor(lockGuard, dt.Diff(DateTime::Now()));
}

void ConditionVariable::NotifyOne() {
	WakeConditionVariable((PCONDITION_VARIABLE)&m_Handle);
}

void ConditionVariable::NotifyAll() {
	WakeAllConditionVariable((PCONDITION_VARIABLE)&m_Handle);
}

NS_JC_END
