/*
 * 작성자: 윤정도
 * 생성일: 4/14/2023 2:17:03 PM
 * =====================
 * 쓰레드 자체 구현용
 */


#pragma once


#include <JCore/Threading/Thread.h>

NS_JC_BEGIN

class RunnableThread : public Thread
{
protected:
	virtual bool PreStart() = 0;		// true시 WorkerThread실행
	virtual void WorkerThread() = 0;
	virtual bool PreStop() = 0;			// true시 Join실행
public:
	RunnableThread(int joinTimeoutMiliSeconds = JCORE_INFINITE);
	~RunnableThread() override;

	void SetJoinTimeout(int joinTimeoutMiliSeconds);
	void Start();
	JoinResult Stop();
protected:
	int m_iTimeoutMiliSecond;
};

NS_JC_END


