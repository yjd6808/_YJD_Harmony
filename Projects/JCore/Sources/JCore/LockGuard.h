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
	// 공부한 내용을 드디어 써먹네;
	// LockGuard.cpp에 Explicit Template Instantiation을 해놓았기 때문에 헤더 파일에서는 외부 소스파일에 이 템플릿 타입이 만들어져있다고 알려주기만 하면됨
	// 이렇게하면 오브젝트 파일(.obj)마다 중복된 코드 정보가 생성되는 것을 막을 수 있다.
	// 학습 내용 : https://blog.naver.com/wjdeh313/222622599396

	extern template EventLockGuard;
	extern template CriticalSectionLockGuard;
	extern template ReaderWriterLockSlimGuard<ReaderWriterLockMode::Read>;
	extern template ReaderWriterLockSlimGuard<ReaderWriterLockMode::Write>;
}

