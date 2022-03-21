#pragma once

#include <JCore/Type.h>

#include <Windows.h>
#include <atomic>
#include <mutex>
#include <condition_variable>

namespace JCore {

	struct Locker
	{
		Locker() = default;
		virtual ~Locker() = default;

		virtual void Lock() = 0;
		virtual bool TryLock() = 0;
		virtual void Unlock() = 0;
		virtual bool IsLocked() = 0;
	};

	struct ReaderWriterLock
	{
		ReaderWriterLock() = default;
		virtual ~ReaderWriterLock() = default;

		virtual void WriteLock() = 0;
		virtual bool TryWriteLock() = 0;
		virtual void WriteUnlock() = 0;
		virtual bool IsWriteLocked() = 0;

		virtual void ReadLock() = 0;
		virtual bool TryReadLock() = 0;
		virtual void ReadUnlock() = 0;
		virtual bool IsReadLocked() = 0;
	};

	/// <summary>
	/// 유저모드기반의 뮤텍스
	/// </summary>
	class CriticalSectionMutex final : public Locker
	{
	public:
		CriticalSectionMutex();
		~CriticalSectionMutex() override;

		void Lock() override;
		void Unlock() override;
		bool TryLock() override;
		bool IsLocked() override;
	private:
		CRITICAL_SECTION m_CriticalSection;
		std::atomic<bool> m_bLocked;
	};


	
	// 그냥 WaitForSingleObject를 사용해서도 뮤텍스를 구현할 수 있지 않을까해서 상상만으로 만들어본 뮤텍스이다.
	// 테스트 해보니 잠금이 제대로 동작하지만 성능이 많이 안좋다.
	// 
	// 이벤트 사용법
	// @참고 : https://docs.microsoft.com/en-us/windows/win32/api/synchapi/nf-synchapi-waitforsingleobject#return-value - WaitForSingleObject 반환값 정보
	class EventMutex final : public Locker
	{
	public:
		EventMutex();
		~EventMutex() override;

		void Lock() override;
		void Unlock() override;
		bool TryLock() override;
		bool IsLocked() override;
	private:
		HANDLE m_LockEvent;
		std::atomic<bool> m_bLocked;
	};


	// 스핀락 구현
	// @참고1 : https://faydoc.tripod.com/cpu/xchg.htm - XCHG 명령어
	// @참고2 : https://en.wikipedia.org/wiki/Spinlock - Wikipedia SpinLock이란
	// @참고3 : https://rigtorp.se/spinlock/ - 완벽한 스핀락 (읽진 않음)
	class SpinLock final : public Locker
	{
	public:
		SpinLock();
		~SpinLock() override;

		void Lock() override;
		void Unlock() override;
		bool TryLock() override;
		bool IsLocked() override;
	private:
		std::atomic<bool> m_bLocked;
	};

	template <Int32U AllowThreadCount>
	class Semaphore final : public Locker
	{
		static_assert(AllowThreadCount >= 1, "... AllowThreadCount must be greater than 0");
	public:
		Semaphore() : m_iAcquiredThreadCount(0) {}
		~Semaphore() override {}
		
		void Lock() override {
			std::unique_lock<std::mutex> uk(m_Mtx);
			while (m_iAcquiredThreadCount >= AllowThreadCount) {
				m_Cv.wait(uk);
			}

			m_iAcquiredThreadCount++;
		}

		void Unlock() override {
			std::lock_guard<std::mutex> guard(m_Mtx);
			if (m_iAcquiredThreadCount <= 0)
				return;

			m_iAcquiredThreadCount--;

			if (m_iAcquiredThreadCount < AllowThreadCount)
				m_Cv.notify_one();
		}

		bool TryLock() override {
			std::lock_guard<std::mutex> guard(m_Mtx);
			if (m_iAcquiredThreadCount >= AllowThreadCount)
				return false;

			m_iAcquiredThreadCount++;
			return true;
		}

		bool IsLocked() override {
			std::lock_guard<std::mutex> guard(m_Mtx);
			return m_iAcquiredThreadCount >= AllowThreadCount;
		}

	private:
		std::mutex m_Mtx;
		std::condition_variable m_Cv;
		int m_iAcquiredThreadCount;		// 잠금을 획득한 쓰레드 수
	};

	template <Int32U AllowReaderThreadCount>
	class _ReaderWriterLockSlim final : public ReaderWriterLock
	{
		static_assert(AllowReaderThreadCount >= 1, "... AllowReaderThreadCount must be greater than 0");
	public:
		void WriteLock() override {
		}


		void WriteUnlock() override {
		}

		
		void ReadLock() override {
		}

		void ReadUnlock() override {
		}


		bool IsWriteLocked() override {
			return true;
		}


		bool TryWriteLock() override {
			return true;
		}

		bool TryReadLock() override {
			return true;
		}

		
		bool IsReadLocked() override {
			return true;
		}
	public:
		std::mutex m_ReadLock;				// 동시에 여러 함수가 실행되는 걸 막자.
		std::mutex m_WriteLock;				// 동시에 여러 함수가 실행되는 걸 막자.
		std::atomic<int> m_iReaderCount;
		std::atomic<bool> m_bWriting;
		std::condition_variable m_Cv;
	};


	using ReaderWriterLockSlim = typename _ReaderWriterLockSlim<3>;
}

