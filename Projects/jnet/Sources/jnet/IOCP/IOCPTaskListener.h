/*
 * 작성자 : 윤정도
 */

#pragma once

NS_JNET_BEGIN

class IOCPTaskAbstract;

class IOCPTaskListener
{
public:
	using FnOnTaskCompleted = jc::Action<IOCPTaskAbstract*>;

	IOCPTaskListener() = default;
	virtual ~IOCPTaskListener() = default;

	virtual void OnTaskCompleted(IOCPTaskAbstract* _pTask)
	{
		if (fnOnTaskCompleted_) 
			fnOnTaskCompleted_(_pTask);
	}

	void SetTaskCompletedCallback(const FnOnTaskCompleted& _fn) { fnOnTaskCompleted_ = _fn; }
	void SetTaskCompletedCallback(FnOnTaskCompleted&& _fn) { fnOnTaskCompleted_ = jc::Move(_fn); }

protected:
	FnOnTaskCompleted fnOnTaskCompleted_;
};

NS_END
