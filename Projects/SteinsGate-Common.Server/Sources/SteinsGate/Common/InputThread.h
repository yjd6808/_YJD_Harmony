/*
	작성자 : 윤정도
	
	입력받기 전용 쓰레드
*/

#pragma once

#include <JCore/Threading/RunnableThread.h>

#include <JCore/Container/ArrayQueue.h>
#include <JCore/Container/HashMap.h>

#include <SteinsGate/Common/ServerEnum.h>

class InputThread : public JCore::RunnableThread
{
	using TInputLock = JCore::NormalLock;
	using TInputLockGuard = JCore::NormalLockGuard;

protected:
	bool PreStart() override;
	bool PreStop() override;
	void WorkerThread() override;
public:
	InputThread();

	void SetEventMap(const JCore::HashMap<JCore::String, int>& eventMap);

	int PopEvent();
	void PopAllEvents(JCore::Vector<int>& eventList);
protected:
	volatile bool m_bRunning;
	TInputLock m_Lock;
	JCore::ArrayQueue<int> m_qInputEvent;
	JCore::HashMap<JCore::String, int> m_hInputEventMap;
	int m_iMaxInputEventCount;
};





