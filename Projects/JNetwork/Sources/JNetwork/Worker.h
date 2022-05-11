/*
 *	�ۼ��� : ������
 */

#pragma once

#include <thread>

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

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
	virtual void JoinWait(HANDLE waitHandle) = 0;
	virtual void Join() = 0;
	virtual void Pause(HANDLE waitHandle) = 0;
	virtual void Resume() = 0;
	virtual void WorkerThread(void* param) = 0;
protected:
	std::thread m_Thread;
	State m_eState;
	HANDLE m_hPauseEvent;
};

} // namespace JNetwork