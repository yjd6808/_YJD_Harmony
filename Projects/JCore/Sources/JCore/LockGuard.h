#pragma once

#include <JCore/Lock.h>

namespace JCore {

	template <typename Lock>
	class LockGuard final
	{
		using TLockGuard = LockGuard<Lock>;
	public:
		LockGuard(Lock& mtx) : m_Mtx(&mtx) {
			m_Mtx->Lock();
		}

		~LockGuard() {
			m_Mtx->Unlock();
		}

		LockGuard(const TLockGuard& mtx) = delete;
		Lock& operator=(const TLockGuard& mtx) = delete;
	private:
		Lock* m_Mtx;
	};





	enum class ReaderWriterLockMode
	{
		Read,
		Write
	};

	template <typename ReaderWriterLock, ReaderWriterLockMode Mode>
	class ReaderWriterLockGuard final
	{
		using TLockGuard = ReaderWriterLockGuard<ReaderWriterLock, Mode>;
	public:
		ReaderWriterLockGuard(ReaderWriterLock& mtx) : m_Mtx(&mtx) {
			if constexpr (Mode == ReaderWriterLockMode::Read)
				m_Mtx->ReadLock();
			else 
				m_Mtx->WriteLock();
		}

		~ReaderWriterLockGuard() {
			if constexpr (Mode == ReaderWriterLockMode::Read)
				m_Mtx->ReadUnlock();
			else
				m_Mtx->WriteUnlock();
		}

		
		ReaderWriterLockGuard(TLockGuard&& mtx) = delete;
		void operator=(const TLockGuard& mtx) = delete;
	private:
		ReaderWriterLock* m_Mtx;
	};


	using EventLockGuard = LockGuard<EventMutex>;
	using CriticalSectionLockGuard = LockGuard<CriticalSectionMutex>;
	using SpinLockGuard = LockGuard<SpinLock>;

	template <ReaderWriterLockMode Mode>
	using ReaderWriterLockSlimGuard = ReaderWriterLockGuard<ReaderWriterLockSlim, Mode>;



	/* ================================================================================= */
	// ������ ������ ���� ��Գ�;
	// LockGuard.cpp�� Explicit Template Instantiation�� �س��ұ� ������ ��� ���Ͽ����� �ܺ� �ҽ����Ͽ� �� ���ø� Ÿ���� ��������ִٰ� �˷��ֱ⸸ �ϸ��
	// �̷����ϸ� ������Ʈ ����(.obj)���� �ߺ��� �ڵ� ������ �����Ǵ� ���� ���� �� �ִ�.
	// �н� ���� : https://blog.naver.com/wjdeh313/222622599396

	extern template EventLockGuard;
	extern template CriticalSectionLockGuard;
	extern template ReaderWriterLockSlimGuard<ReaderWriterLockMode::Read>;
	extern template ReaderWriterLockSlimGuard<ReaderWriterLockMode::Write>;
}

