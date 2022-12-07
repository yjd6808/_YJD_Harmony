/*
 * 작성자: 윤정도
 * =====================
 *
 */

#include <JCore/Core.h>
#include <JCore/Exception.h>
#include <JCore/Sync/EventLock.h>
#include <JCore/Primitives/StringUtil.h>

namespace JCore {

	template class LockGuard<EventLock>;

	EventLock::EventLock()
		: m_LockEvent(CreateEvent(NULL, FALSE, TRUE, NULL)) {

	}

	EventLock::~EventLock() {
		CloseHandle(m_LockEvent);
	}

	void EventLock::Lock() {
		const Int32UL dwResult = WaitForSingleObject(m_LockEvent, INFINITE);


		if (dwResult == WAIT_FAILED) {
			// WaitForSingleObject 함수 호출이 실패한 경우
			// 자세한 정보는 GetLastError() 코드 참고해야함

			throw JCore::RuntimeException(JCore::StringUtil::Format("락이 실패하였습니다. WAIT_FAILED 오류코드 : %d", GetLastError()));
		}


		if (dwResult == WAIT_ABANDONED) {
			// 이건 Mutex 핸들을 넣어줬을 때 오류나는 경우 인듯?
			throw JCore::RuntimeException(JCore::StringUtil::Format("락이 실패하였습니다. WAIT_ABANDONED"));
		}

		m_bLocked = true;
		ResetEvent(m_LockEvent);
	}

	void EventLock::Unlock() {
		SetEvent(m_LockEvent);
		m_bLocked = false;
	}

	bool EventLock::TryLock() {
		if (m_bLocked)
			return false;

		Lock();
		return true;
	}

	bool EventLock::IsLocked() {
		return m_bLocked;
	}

}