/*
 * 작성자: 윤정도
 * =====================
 *
 */


#pragma once

#include <JCore/Sync/ILock.h>
#include <JCore/Sync/WaitHandle.h>
#include <JCore/Primitives/Atomic.h>

NS_JC_BEGIN

// 그냥 WaitForSingleObject를 사용해서도 뮤텍스를 구현할 수 있지 않을까해서 상상만으로 만들어본 뮤텍스이다.
// 테스트 해보니 잠금이 제대로 동작하지만 성능이 많이 안좋다.
// 
// 이벤트 사용법
// @참고 : https://docs.microsoft.com/en-us/windows/win32/api/synchapi/nf-synchapi-waitforsingleobject#return-value - WaitForSingleObject 반환값 정보

class EventLock final : public ILock
{
public:
	using TGuard = LockGuard<EventLock>;

	EventLock();
	~EventLock() override;

	void Lock() override;
	void Unlock() override;
	bool TryLock() override;
	bool IsLocked() override;
private:
	WaitHandle m_LockEvent;
	Atomic<bool> m_bLocked;
};

using EventLockGuard = LockGuard<EventLock>;
extern template         LockGuard<EventLock>;

NS_JC_END;
