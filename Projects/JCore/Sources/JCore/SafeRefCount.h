/*
 *	작성자 : 윤정도
 */

#pragma once

#include <JCore/Assert.h>
#include <JCore/Primitives/Atomic.h>

namespace JCore {

struct SafeRefCount
{
	SafeRefCount() = default;
	virtual ~SafeRefCount() = default;

	void AddRef() {
		++m_iRef;
	}
	void AddRef(int cnt) {
		m_iRef += cnt;
	}
	int  GetRefCount() {
		return m_iRef;
	}

	// 계산을 똑바로해서 Release 호출하도록 할 것
	// m_iRef가 0이하가 되는 순간이 2회 이상인 경우가 되면 안됨.
	void Release(int cnt = 1) {
        int desired = m_iRef - cnt;
        int expected = desired + cnt;

        for (;;) {
            DebugAssertMessage(desired >= 0, "레퍼런스 카운트 계산을 똑바로 해주세요.");

            if (m_iRef.CompareExchange(expected, desired)) {
                if (desired == 0) 
                    ReleaseAction();

                break;
            } 

            desired = m_iRef - cnt;
            expected = desired + cnt;
        }
	}
protected:
    virtual void ReleaseAction() = 0;
protected:
    Atomic<int> m_iRef;
};

} // namespace JCore