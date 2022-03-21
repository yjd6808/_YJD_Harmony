#include <JCore/Core.h>
#include <JCore/Lock.h>

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

	bool CriticalSectionMutex::IsLocked() {
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

	bool EventMutex::IsLocked() {
		return m_bLocked;
	}



	/*=====================================================================================
								SpinLock
	=======================================================================================*/

	SpinLock::SpinLock() : m_bLocked(false) {
	}

	SpinLock::~SpinLock() {
	}

	// @����1 : https://en.cppreference.com/w/cpp/atomic/atomic/compare_exchange - compare_exchange_weak
	// @����2 : https://jacking75.github.io/C++_std_atomic_compare_exchange_strong/ - �ѱ۷� ����� ��(��ŷ75)
	// bool compare_exchange_weak( T& expected, T desired )
	//		return		: ���簪�� expected�� ��ġ�� ��� true ��ġ���� ���� ��� false�� ��ȯ�Ѵ�.
	//      expected	: �� ���
	//      desired		: �ٲٰ����ϴ� ��
	// 
	// [����]
	// ���� ���簪�� expected���� Ʋ���� expected�� ���簪���� ������
	// ���� ���簪�� expected���� ������ ���簪�� desired������ ������
	/*
	   @�ñ���
	   �ٵ� �ᱹ CPU ������ ����ؼ� busy-waiting ���¸� ��� �����ϴ°ǵ�
	   
	   while (m_bLocked) {
	       Sleep(0);
	   }
	   m_bLocked = true;
	   �� �̰� �ȵǰڳ�. while���� ���ÿ� 2���� �����尡 ���� ���͹����� ��찡 ���� �� ���� �� ������?
	 */

	// @�н� : https://modoocode.com/271
	// memory_order_acquire : �ش� ��� ���Ŀ� ���� ��� �޸� ��ɵ��� �ش� ��� �������� ���ġ �Ǵ� ���� �����Ѵ�
	// memory_order_release : �ش� ��� ������ ���� ��� �޸� ��ɵ��� �ش� ��� ���ķ�   ���ġ �Ǵ� ���� �����Ѵ�.
	// memory_order_seq_cst : �޸� ����� ������ �ϰ����� �����Ѵ�.
	// memory_order_acq_rel : acquire �� release �� ��� �����ϴ� ���Դϴ�. �̴�, �б�� ���⸦ ��� �����ϴ� ��ɵ�, ���� �� fetch_add �� ���� �Լ����� ���� �� �ֽ��ϴ�.
	// ������ �ϰ����̶�, �޸� ��� ���ġ�� ����, ��� �����忡�� ��� ������ ������ ���� ������ �� �ִ�, �������� �����ϴ� �״�� CPU �� �۵��ϴ� ����̶� �����ϸ� �˴ϴ�.

	// compare_exchange_strong �Լ��� �⺻������ memory_order_seq_cst�� ����ϵ��� �ϰ� �ִµ�
	// ���ڿ����� ����� �׷��� ũ�� ������ ARM CPU�� �ϰ����� �����ϱ� ���� ����� �ſ� ũ�ٰ� �Ѵ�.
	void SpinLock::Lock() {
		bool expected = false;

		while (!m_bLocked.compare_exchange_strong(expected, true)) {
			
			
			// ���� ���� ��� �̹� �ٸ� �����尡 ���� ȹ���� �����̴�.
			//   = ���� ��(m_bLocked)�� true�̱� ������ compare_exchange_strong �Լ� ����� false�� ��ȯ�ߴ�.
			// 
			// �׳� �ظ��� �ݺ��� ��� ���鼭 �������� ���� ��� �ѵ��

			expected = false;
			// �̰� ���ִ� ������ m_bLocked�� true�� ��� expected�� ���� ��(true)�� ����Ǿ� ������ ������
			// �ٽ� compare_exchange_strong�� ȣ�� �� �� �׳� ������ �ݺ����� Ż���ع����� ���� �����尡 
			// Critical Section�� �����ϴ� ��찡 ���� �� �ְ� �ȴ�.
		}
	}

	void SpinLock::Unlock() {
		m_bLocked = false;
	}

	bool SpinLock::TryLock() {
		bool expected = false;
		return m_bLocked.compare_exchange_strong(expected, true);
	}

	bool SpinLock::IsLocked() {
		return m_bLocked;
	}
}
