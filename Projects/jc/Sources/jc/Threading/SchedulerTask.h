/*
 * 작성자: 윤정도
 * 생성일: 8/23/2023 2:50:43 PM
 * =====================
 *
 */


#pragma once

#include <jc/Time.h>
#include <jc/Functional.h>
#include <jc/Sync/SpinLock.h>

NS_JC_BEGIN

class SchedulerTask;
using SchedulerTaskCallback = Action<SchedulerTask*>;
class JC_NOVTABLE SchedulerTask
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
class JC_NOVTABLE SchedulerTaskRunnable : public SchedulerTask
{
public:
	SchedulerTaskRunnable() : at_(DateTime::Now()) {}
	SchedulerTaskRunnable(DateTime _at) : at_(_at) {}

	DateTime At() override { return at_; }
	bool CanNextCall() override { return true; }
	void CallCallback() override {
		at_ = DateTime::Now() + Interval();
		if (!m_bExecuted) 
		{
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
	DateTime at_;
};

template <typename TCallback>
constexpr bool IsSchedulerTaskCallback_v = IsCallable_v<TCallback> &&
IsSameType_v<CallableSignatureReturn_t<TCallback>, void> &&
IsSameType_v<CallableSignatureParam_t<TCallback>, detail::ParameterPack<SchedulerTask*>>;

class SchedulerTaskOnce : public SchedulerTask
{
public:
	template <typename TCallback, DefaultEnableIf_t<IsSchedulerTaskCallback_v<TCallback>> = nullptr>
	SchedulerTaskOnce(DateTime _at, TCallback&& _callback)
	: at_(_at)
	, fnCallback_(Forward<TCallback>(_callback))
	{}

	DateTime At() override { return at_; }
	TimeSpan Interval() override { return { 0 }; }
	bool CanNextCall() override { return false; }
	void CallCallback() override { m_bExecuted = true; fnCallback_(this); }
	bool HasCallbackFunc() const override { return true; }
private:
	DateTime at_;
	SchedulerTaskCallback fnCallback_;
};

class SchedulerTaskRepeat : public SchedulerTask
{
public:
	template <typename TCallback>
	SchedulerTaskRepeat(DateTime _at, TimeSpan _interval, TCallback&& _callback, Int32U _repeat = INFINITE)
	: at_(_at.Tick)
	, interval_(_interval)
	, callback_(Forward<TCallback>(_callback))
	, curRepeat_(0)
	, maxRepeat_(_repeat)
	{}

	DateTime At() override { return { at_.Load() }; }
	TimeSpan Interval() override { return interval_; }
	bool CanNextCall() override { return curRepeat_ < maxRepeat_; }
	Int32U MaxRepeatCount() override { return maxRepeat_; }
	void CallCallback() override 
	{
		m_bExecuted = true;
		at_ = (DateTime::Now() + interval_).Tick;
		callback_(this);
		curRepeat_ += 1;
	}
	bool HasCallbackFunc() const override { return true; }
private:
	AtomicInt64 at_;
	TimeSpan interval_;
	SchedulerTaskCallback callback_;
	AtomicInt32U curRepeat_;
	Int32U maxRepeat_;
};

NS_END

