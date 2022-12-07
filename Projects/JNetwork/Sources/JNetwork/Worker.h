/*
 *	작성자 : 윤정도
 */

#pragma once

#include <JCore/Type.h>
#include <thread>

namespace JNetwork {

class Worker
{
public:
	enum class State
	{
		Prepared,
		Running,
		JoinWait,
		Paused,
		Joined
	};

	Worker() : m_eState(State::Prepared)
	         , m_hPauseEvent(CreateEvent(NULL, FALSE, FALSE, NULL)) {
	};
	virtual ~Worker() = default;;

	virtual void Run(void* param = nullptr) = 0;
	virtual void JoinWait(WinHandle waitHandle) = 0;
	virtual void Join() = 0;
	virtual void Pause(WinHandle waitHandle) = 0;
	virtual void Resume() = 0;
	virtual void WorkerThread(void* param) = 0;
protected:
	std::thread m_Thread;
	State m_eState;
	WinHandle m_hPauseEvent;
};

} // namespace JNetwork