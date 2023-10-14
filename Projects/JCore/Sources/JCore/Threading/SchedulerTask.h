/*
 * 작성자: 윤정도
 * 생성일: 8/23/2023 2:50:43 PM
 * =====================
 *
 */


#pragma once

#include <JCore/Time.h>
#include <JCore/Functional.h>
#include <JCore/Sync/SpinLock.h>

NS_JC_BEGIN

class SchedulerTask;
using SchedulerTaskCallback = Action<SchedulerTask*>;
class JCORE_NOVTABLE SchedulerTask
{
public:
	virtual ~SchedulerTask() = default;
	virtual DateTime At() = 0;
	virtual TimeSpan Interval() = 0;
	virtual bool CanNextCall() = 0;
	virtual bool HasCallbackFunc() const { return false; }
	virtual bool Executed() const { return m_bExecuted; }
	virtual Int32U MaxRepeatCount() { return 1; }
	virtual void CallCallback() = 0;
protected:
	AtomicBool m_bExecuted = false;
};

// 스케쥴 작업을 상속받은 클래스 자체에 반복작업 내용을 정의 가능하도록 한다.
class JCORE_NOVTABLE SchedulerTaskRunnable : public SchedulerTask
{
public:
	SchedulerTaskRunnable() : m_At(DateTime::Now()) {}
	SchedulerTaskRunnable(DateTime at) : m_At(at) {}

	DateTime At() override { return m_At; }
	bool CanNextCall() override { return true; }
	void CallCallback() override {
		m_At = DateTime::Now() + Interval();
		if (!m_bExecuted) {
			OnFirstScheduled();
			m_bExecuted = true;
		}

		OnScheduled();
	}

	virtual void Initialize() = 0;
	virtual void Finalize() = 0;
	virtual void OnScheduled() = 0;			// 처음을 포함해서 스케쥴링 될떄마다 호출
	virtual void OnFirstScheduled() = 0;	// 제일 처음에만 호출
protected:
	DateTime m_At;
};

template <typename TCallback>
constexpr bool IsSchedulerTaskCallback_v = IsCallable_v<TCallback> &&
IsSameType_v<CallableSignatureReturn_t<TCallback>, void> &&
IsSameType_v<CallableSignatureParam_t<TCallback>, Detail::ParameterPack<SchedulerTask*>>;

class SchedulerTaskOnce : public SchedulerTask
{
public:
	template <typename TCallback, DefaultEnableIf_t<IsSchedulerTaskCallback_v<TCallback>> = nullptr>
	SchedulerTaskOnce(DateTime at, TCallback&& callback)
		: m_At(at)
		, m_Callback(Forward<TCallback>(callback))
	{}

	DateTime At() override { return m_At; }
	TimeSpan Interval() override { return { 0 }; }
	bool CanNextCall() override { return false; }
	void CallCallback() override { m_bExecuted = true; m_Callback(this); }
	bool HasCallbackFunc() const override { return true; }
private:
	DateTime m_At;
	SchedulerTaskCallback m_Callback;
};

class SchedulerTaskRepeat : public SchedulerTask
{
public:
	template <typename TCallback>
	SchedulerTaskRepeat(DateTime at, TimeSpan interval, TCallback&& callback, Int32U repeat = INFINITE)
		: m_At(at.Tick)
		, m_Interval(interval)
		, m_Callback(Forward<TCallback>(callback))
		, m_uiCurRepeat(0)
		, m_uiMaxRepeat(repeat)
	{}

	DateTime At() override { return { m_At.Load() }; }
	TimeSpan Interval() override { return m_Interval; }
	bool CanNextCall() override { return m_uiCurRepeat < m_uiMaxRepeat; }
	Int32U MaxRepeatCount() override { return m_uiMaxRepeat; }
	void CallCallback() override {
		m_bExecuted = true;
		m_At = (DateTime::Now() + m_Interval).Tick;
		m_Callback(this);
		m_uiCurRepeat += 1;
	}
	bool HasCallbackFunc() const override { return true; }
private:
	AtomicInt64 m_At;
	TimeSpan m_Interval;
	SchedulerTaskCallback m_Callback;
	AtomicInt32U m_uiCurRepeat;
	Int32U m_uiMaxRepeat;
};

NS_JC_END

