/*
 * 작성자: 윤정도
 * =====================
 *
 */


#pragma once

#include <JCore/Sync/ILock.h>
#include <JCore/Primitives/Atomic.h>

namespace JCore {

	class SpinLock final : public ILock
	{
	public:
		SpinLock();
		~SpinLock() override;

		void Lock() override;
		void Unlock() override;
		bool TryLock() override;
		bool IsLocked() override;
	private:
		Atomic<bool> m_bLocked;
	};

	using SpinLockGuard = LockGuard<SpinLock>;
	extern template       LockGuard<SpinLock>;

} // namespace JCore