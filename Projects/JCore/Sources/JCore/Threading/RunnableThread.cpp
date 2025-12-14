/*
 * 작성자: 윤정도
 * 생성일: 4/14/2023 2:17:13 PM
 * =====================
 *
 */


#include <JCore/Core.h>
#include <JCore/Threading/RunnableThread.h>

NS_JC_BEGIN

//////////////////////////////////////////////////////////////////////////////////////////
RunnableThread::RunnableThread(int _joinTimeoutMiliSeconds)
	: m_timeoutMiliSecond(_joinTimeoutMiliSeconds)
{
}

//////////////////////////////////////////////////////////////////////////////////////////
RunnableThread::~RunnableThread()
{
	DebugAssertMsg(m_hHandle == nullptr, "스레드가 정상적으로 종료되지 않았습니다.");
}

//////////////////////////////////////////////////////////////////////////////////////////
void RunnableThread::SetJoinTimeout(int _joinTimeoutMiliSeconds)
{
	m_timeoutMiliSecond = _joinTimeoutMiliSeconds;
}

//////////////////////////////////////////////////////////////////////////////////////////
void RunnableThread::Start()
{
	if (!PreStart())
		return;

	const int startError = Thread::Start([this](void*)
	{
		WorkerThread();
	});

	if (startError != 0)
	{
		_LogError_("쓰레드 시작 실패 (%d)", startError);
	}
}

//////////////////////////////////////////////////////////////////////////////////////////
Thread::JoinResult RunnableThread::Stop()
{
	if (PreStop())
	{
		return Join(m_timeoutMiliSecond);
	}

	return eNotJoinable;
}

NS_JC_END
