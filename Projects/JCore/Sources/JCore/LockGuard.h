#pragma once

#include <JCore/Lock.h>

namespace JCore {

	template <typename _Lock>
	class LockGuard final
	{
		using TLockGuard = typename LockGuard<_Lock>;
	public:
		LockGuard(_Lock& mtx) : m_Mtx(&mtx) {
			m_Mtx->Lock();
		}

		~LockGuard() {
			m_Mtx->Unlock();
		}

		LockGuard(const TLockGuard& mtx) = delete;
		_Lock& operator=(const TLockGuard& mtx) = delete;
	private:
		_Lock* m_Mtx;
	};





	enum class ReaderWriterLockMode
	{
		Read,
		Write
	};

	template <typename _ReaderWriterLock, ReaderWriterLockMode Mode>
	class ReaderWriterLockGuard final
	{
		using TLockGuard = typename ReaderWriterLockGuard<_ReaderWriterLock, Mode>;
	public:
		ReaderWriterLockGuard(_ReaderWriterLock& mtx) : m_Mtx(&mtx) {
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
		_ReaderWriterLock* m_Mtx;
	};


	using EventLockGuard = LockGuard<EventMutex>;
	using CriticalSectionLockGuard = LockGuard<CriticalSectionMutex>;

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

