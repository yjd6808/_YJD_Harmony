/*
 *	작성자 : 윤정도
 */

#pragma once

#include <jc/Type.h>

#include <jc/Threading/Thread.h>

#include <jnet/Namespace.h>

NS_JNET_BEGIN

class Worker
{
public:
	enum State
	{
		ePrepared,
		eRunning,
		eJoinWait,
		eJoined
	};

	Worker()
		: state_(State::ePrepared)
	{
	}

	virtual ~Worker() = default;

	virtual void Run(void* _pParam = nullptr) = 0;
	virtual void JoinWait(jc::WaitHandle* _pWaitHandle) = 0;
	virtual void Join() = 0;
	virtual void WorkerThread(void* _pParam) = 0;
	Int32U GetThreadId()
	{
		return thread_.GetId();
	}

protected:
	jc::Thread thread_;
	jc::AtomicInt state_;
};

using WorkerPtr = jc::SharedPtr<Worker>;

NS_JNET_END
