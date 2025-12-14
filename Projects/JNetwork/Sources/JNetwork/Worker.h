/*
 *	작성자 : 윤정도
 */

#pragma once

#include <JCore/Type.h>

#include <JCore/Threading/Thread.h>

#include <JNetwork/Namespace.h>

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
	virtual void JoinWait(JCore::WaitHandle* _pWaitHandle) = 0;
	virtual void Join() = 0;
	virtual void WorkerThread(void* _pParam) = 0;
	Int32U GetThreadId()
	{
		return thread_.GetId();
	}

protected:
	JCore::Thread thread_;
	JCore::AtomicInt state_;
};

using WorkerPtr = JCore::SharedPtr<Worker>;

NS_JNET_END
