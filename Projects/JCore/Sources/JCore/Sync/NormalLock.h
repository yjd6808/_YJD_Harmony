/*
 * 작성자: 윤정도
 * =====================
 *
 */


#pragma once

#include <JCore/Sync/ILock.h>
#include <JCore/Sync/LockGuard.h>
#include <JCore/Primitives/Atomic.h>

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

namespace JCore {

	class NormalLock final : public ILock
	{
	public:
		NormalLock();
		~NormalLock() override;

		void Lock() override;
		void Unlock() override;
		bool TryLock() override;
		bool IsLocked() override;
	private:
		CRITICAL_SECTION m_CriticalSection;
		Atomic<bool> m_bLocked;
	};

	/* ================================================================================= */
	// 공부한 내용을 드디어 써먹네;
	// Cpp 파일에 Explicit Template Instantiation을 해놓았기 때문에 헤더 파일에서는 외부 소스파일에 이 템플릿 타입이 만들어져있다고 알려주기만 하면됨
	// 이렇게하면 오브젝트 파일(.obj)마다 중복된 코드 정보가 생성되는 것을 막을 수 있다.
	// 학습 내용 : https://blog.naver.com/wjdeh313/222622599396

	using NormalLockGuard = LockGuard<NormalLock>;
	extern template         LockGuard<NormalLock>;
} // namespace JCore 