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
			// WaitForSingleObject �Լ� ȣ���� ������ ���
			// �ڼ��� ������ GetLastError() �ڵ� �����ؾ���

			throw RuntimeException(StringUtil::Format("���� �����Ͽ����ϴ�. WAIT_FAILED �����ڵ� : %d", GetLastError()));
		} else if (dwResult == WAIT_ABANDONED) {
			// �̰� Mutex �ڵ��� �־����� �� �������� ��� �ε�?
			throw RuntimeException(StringUtil::Format("���� �����Ͽ����ϴ�. WAIT_ABANDONED"));
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
