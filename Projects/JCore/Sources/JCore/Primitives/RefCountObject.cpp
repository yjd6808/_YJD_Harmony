/*
 * 작성자: 윤정도
 * 생성일: 7/30/2023 6:38:28 PM
 * =====================
 *
 */

#include <JCore/Core.h>
#include <JCore/Primitives/RefCountObject.h>

NS_JC_BEGIN

void RefCountObject::Release(int cnt) {
    int desired = m_iRef - cnt;
    int expected = desired + cnt;

    for (;;) {
        DebugAssertMsg(desired >= 0, "레퍼런스 카운트 계산을 똑바로 해주세요.");

        if (m_iRef.CompareExchange(expected, desired)) {
            if (desired <= 0)
                ReleaseAction();

            break;
        }

        desired = m_iRef - cnt;
        expected = desired + cnt;
    }
}

NS_JC_END

