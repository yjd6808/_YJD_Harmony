/*
 * 작성자: 윤정도
 * =====================
 *
 */


#pragma once

#include <JCore/Sync/ILock.h>
#include <JCore/Sync/NormalLock.h>

namespace JCore {

	class RecursiveLock final : public ILock
	{
	public:
		RecursiveLock();
		~RecursiveLock() override = default;

		void Lock() override;
		bool TryLock() override;
		void Unlock() override;
		bool IsLocked() override;
	private:
		static size_t GetCurrentThreadId();
	private:
		NormalLock m_Lock;
		size_t m_uiLockedThreadId;
		int m_iRecursion;
	};

	using RecursiveLockGuard = LockGuard<RecursiveLock>;
	extern template             LockGuard<RecursiveLock>;

} // namespace JCore
