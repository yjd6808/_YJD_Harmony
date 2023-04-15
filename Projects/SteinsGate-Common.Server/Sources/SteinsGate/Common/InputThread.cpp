/*
 * 작성자 : 윤정도
 */

#include "Server.h"
#include "ServerCoreHeader.h"
#include "InputThread.h"

#include <csignal>

USING_NS_JC;
USING_NS_JNET;

InputThread::InputThread()
	: m_bRunning(false)
	, m_iMaxInputEventCount(4)
{}

// std::cin.get을 강제로 
bool InputThread::PreStart() {
	m_bRunning = true;
	return true;
}

bool InputThread::PreStop() {
	m_bRunning = false;
	return true;
}


void InputThread::WorkerThread() {
	while (m_bRunning) {
		String read = Console::ReadLine();
		TInputLockGuard guard(m_Lock);
		const int* pEventCode = m_hInputEventMap.Find(read);

		if (pEventCode == nullptr)
			continue;

		if (m_qInputEvent.Size() >= m_iMaxInputEventCount) {
			_LogWarn_("이벤트 큐에서 아직 처리되지 않은 이벤트가 최대 수치에 도달했습니다. 입력된 이벤트 입력을 무시합니다. %s", read.Source());
			return;
		}

		m_qInputEvent.Enqueue(*pEventCode);
	}
}

void InputThread::SetEventMap(const HashMap<String, int>& eventMap) {
	TInputLockGuard guard(m_Lock);
	m_hInputEventMap = eventMap;
}

int InputThread::PopEvent() {
	TInputLockGuard guard(m_Lock);

	if (m_qInputEvent.Size() <= 0) 
		return CommonInputEvent::NoEvent;

	const int inputEvent = m_qInputEvent.Front();
	m_qInputEvent.Dequeue();
	return inputEvent;
}


void InputThread::PopAllEvents(Vector<int>& eventList) {
	eventList.Clear();

	TInputLockGuard guard(m_Lock);

	while (!m_qInputEvent.IsEmpty()) {
		eventList.PushBack(m_qInputEvent.Front());
		m_qInputEvent.Dequeue();

	}
}