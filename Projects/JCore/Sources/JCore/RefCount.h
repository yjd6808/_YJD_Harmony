/*
	작성자 : 윤정도
*/

#pragma once

#include <JCore/LockGuard.h>
#include <atomic>

namespace JCore {

// 이거 atomic으로 할 수 있을 것 같은데..
// Release를 어떻게 처리할 지 생각이 안나서 일단 뮤텍스로둠
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

	// 계산을 똑바로해서 Release 호출하도록 할 것
	// m_iRef가 0이하가 되는 순간이 2회 이상인 경우 오류가 발생할 수 있겠다.
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
	// std::atomic<int> m_iRef;
};

} // namespace JCore