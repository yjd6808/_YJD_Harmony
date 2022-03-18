#include <JCore/Core.h>
#include <JCore/Mutex.h>

#include <JCore/Exception.h>
#include <JCore/StringUtil.h>

namespace JCore {
	/*=====================================================================================
								CriticalSectionMutex
	=======================================================================================*/

	CriticalSectionMutex::CriticalSectionMutex() {
		InitializeCriticalSection(&m_CriticalSection);
	}

	CriticalSectionMutex::~CriticalSectionMutex() {
		DeleteCriticalSection(&m_CriticalSection); 
	}

	void CriticalSectionMutex::Lock() {
		EnterCriticalSection(&m_CriticalSection);
		m_bLocked = true;
	}

	void CriticalSectionMutex::Unlock() {
		LeaveCriticalSection(&m_CriticalSection);
		m_bLocked = false;
	}

	bool CriticalSectionMutex::TryLock() {
		BOOL ret = TryEnterCriticalSection(&m_CriticalSection);
		return m_bLocked = (bool)ret;
	}

	bool CriticalSectionMutex::IsLocked() const {
		return m_bLocked;
	}

	/*=====================================================================================
								EventMutex
	=======================================================================================*/
	EventMutex::EventMutex() {
		m_LockEvent = CreateEvent(NULL, FALSE, TRUE, NULL);
	}

	EventMutex::~EventMutex() {
		CloseHandle(m_LockEvent);
	}

	void EventMutex::Lock() {
		DWORD dwResult = WaitForSingleObject(m_LockEvent, INFINITE);


		if (dwResult == WAIT_FAILED) {
			// WaitForSingleObject 함수 호출이 실패한 경우
			// 자세한 정보는 GetLastError() 코드 참고해야함

			throw RuntimeException(StringUtil::Format("락이 실패하였습니다. WAIT_FAILED 오류코드 : %d", GetLastError()));
		} else if (dwResult == WAIT_ABANDONED) {
			// 이건 Mutex 핸들을 넣어줬을 때 오류나는 경우 인듯?
			throw RuntimeException(StringUtil::Format("락이 실패하였습니다. WAIT_ABANDONED"));
		}

		m_bLocked = true;
		ResetEvent(m_LockEvent);
	}

	void EventMutex::Unlock() {
		SetEvent(m_LockEvent);
		m_bLocked = false;
	}

	bool EventMutex::TryLock() {
		if (m_bLocked)
			return false;

		Lock();
		return true;
	}

	bool EventMutex::IsLocked() const {
		return m_bLocked;
	}
}
