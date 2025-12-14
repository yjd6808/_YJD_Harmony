/*
 * 작성자: 윤정도
 * 생성일: 6/3/2023 8:39:55 PM
 */



#include <JCore/Core.h>
#include <JCore/Sync/ConditionVariable.h>

NS_JC_BEGIN

//////////////////////////////////////////////////////////////////////////////////////////
ConditionVariable::ConditionVariable()
{
	InitializeConditionVariable((PCONDITION_VARIABLE)&m_Handle);
}

//////////////////////////////////////////////////////////////////////////////////////////
void ConditionVariable::Wait(LockGuard<NormalLock>& _lockGuard)
{
	if (!SleepConditionVariableCS((PCONDITION_VARIABLE)&m_Handle, (PCRITICAL_SECTION)&_lockGuard.Handle()->criticalSection_, INFINITE))
	{
		DebugAssert(false);
	}
}

//////////////////////////////////////////////////////////////////////////////////////////
int ConditionVariable::WaitFor(LockGuard<NormalLock>& _lockGuard, const TimeSpan& _timeSpan)
{
	int milliSeconds = int(_timeSpan.GetTotalMiliSecondsInt());
	if (milliSeconds <= 0)
	{
		return CvStatus::eTimeout;
	}
	if (!SleepConditionVariableCS((PCONDITION_VARIABLE)&m_Handle, (PCRITICAL_SECTION)&_lockGuard.Handle()->criticalSection_, milliSeconds))
	{
		return CvStatus::eTimeout;
	}
	return CvStatus::eNoTimeout;
}

//////////////////////////////////////////////////////////////////////////////////////////
int ConditionVariable::WaitUntil(LockGuard<NormalLock>& _lockGuard, const DateTime& _dateTime)
{
	return WaitFor(_lockGuard, _dateTime.Diff(DateTime::Now()));
}

//////////////////////////////////////////////////////////////////////////////////////////
void ConditionVariable::NotifyOne()
{
	WakeConditionVariable((PCONDITION_VARIABLE)&m_Handle);
}

//////////////////////////////////////////////////////////////////////////////////////////
void ConditionVariable::NotifyAll()
{
	WakeAllConditionVariable((PCONDITION_VARIABLE)&m_Handle);
}

NS_JC_END
