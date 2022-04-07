/*
	�ۼ��� : ������
*/

#pragma once

#include <JCore/LockGuard.h>
#include <atomic>

namespace JCore {

// �̰� atomic���� �� �� ���� �� ������..
// Release�� ��� ó���� �� ������ �ȳ��� �ϴ� ���ؽ��ε�
struct RefCount
{
	RefCount() = default;
	virtual ~RefCount() = default;

	void AddRef() {
		CriticalSectionLockGuard g(m_Mtx);
		m_iRef++;
	}
	void AddRef(int cnt) {
		CriticalSectionLockGuard g(m_Mtx);
		m_iRef += cnt;
	}
	int  GetRefCount() {
		CriticalSectionLockGuard g(m_Mtx);
		return m_iRef;
	}

	virtual void Release(int cnt = 1) {
		CriticalSectionLockGuard g(m_Mtx);
		m_iRef -= cnt;

		if (m_iRef <= 0) {
			delete this;
		}
	}

protected:
	CriticalSectionMutex m_Mtx;
	int m_iRef = 0;
};

} // namespace JCore