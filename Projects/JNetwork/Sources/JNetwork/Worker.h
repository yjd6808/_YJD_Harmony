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

	Worker() : m_eState(State::ePrepared) {}
	virtual ~Worker() = default;

	virtual void Run(void* param = nullptr) = 0;
	virtual void JoinWait(JCore::WaitHandle* waitHandle) = 0;
	virtual void Join() = 0;
	virtual void WorkerThread(void* param) = 0;
	Int32U GetThreadId() { return m_Thread.GetId(); }
protected:
	JCore::Thread m_Thread;
	JCore::AtomicInt m_eState;
	
};

using WorkerPtr = JCore::SharedPtr<Worker>;

NS_JNET_END