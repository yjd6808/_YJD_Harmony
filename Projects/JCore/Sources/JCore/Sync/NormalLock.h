/*
 * 작성자: 윤정도
 * =====================
 * CriticalSection 자체가 RecursiveLock인듯?
 */


#pragma once

#include <JCore/Sync/ILock.h>
#include <JCore/Primitives/Atomic.h>
#include <JCore/Wrapper/WinApi.h>

NS_JC_BEGIN

class NormalLock final : public ILock
{
public:
	using TGuard = LockGuard<NormalLock>;

	NormalLock();
	~NormalLock() override;

	void Lock() override;
	void Unlock() override;
	bool TryLock() override;
	bool IsLocked() override;
private:
	WinApi::CriticalSection m_CriticalSection;
	Atomic<int> m_hOwnThread;
};

/* ================================================================================= */
// 공부한 내용을 드디어 써먹네;
// Cpp 파일에 Explicit Template Instantiation을 해놓았기 때문에 헤더 파일에서는 외부 소스파일에 이 템플릿 타입이 만들어져있다고 알려주기만 하면됨
// 이렇게하면 오브젝트 파일(.obj)마다 중복된 코드 정보가 생성되는 것을 막을 수 있다.
// 학습 내용 : https://blog.naver.com/wjdeh313/222622599396

using NormalLockGuard = LockGuard<NormalLock>;
extern template         LockGuard<NormalLock>;

NS_JC_END
