#pragma once

#include <JCore/Type.h>

#include <Windows.h>
#include <atomic>

namespace JCore {

	struct Mutex
	{
		Mutex() = default;
		virtual ~Mutex() = default;

		virtual void Lock() = 0;
		virtual bool TryLock() = 0;
		virtual void Unlock() = 0;
		virtual bool IsLocked() const = 0;
	};

	struct ReaderWriterMutex
	{
		ReaderWriterMutex() = default;
		virtual ~ReaderWriterMutex() = default;

		virtual void WriteLock() = 0;
		virtual bool TryWriteLock() = 0;
		virtual void WriteUnlock() = 0;
		virtual bool IsWriteLocked() const = 0;

		virtual void ReadLock() = 0;
		virtual bool TryReadLock() = 0;
		virtual void ReadUnlock() = 0;
		virtual bool IsReadLocked() const = 0;
	};

	/// <summary>
	/// 유저모드기반의 뮤텍스
	/// </summary>
	class CriticalSectionMutex final : public Mutex
	{
	public:
		CriticalSectionMutex();
		~CriticalSectionMutex() override;

		void Lock() override;
		void Unlock() override;
		bool TryLock() override;
		bool IsLocked() const override;
	private:
		CRITICAL_SECTION m_CriticalSection;
		std::atomic<bool> m_bLocked;
	};


	// 이벤트 사용법
	// @참고(반환값) : https://docs.microsoft.com/en-us/windows/win32/api/synchapi/nf-synchapi-waitforsingleobject#return-value
	class EventMutex final : public Mutex
	{
	public:
		EventMutex();
		~EventMutex() override;

		void Lock() override;
		void Unlock() override;
		bool TryLock() override;
		bool IsLocked() const override;
	private:
		HANDLE m_LockEvent;
		std::atomic<bool> m_bLocked;
	};


	class Semaphore final : public Mutex
	{

	};

	template <typename Mtx = CriticalSectionMutex>
	class _ReaderWriterLockSlim final : public ReaderWriterMutex
	{
		void WriteLock() override {
		}

		bool TryWriteLock() override {
			return true;
		}

		void WriteUnlock() override {
		}

		bool IsWriteLocked() const override {
			return true;
		}

		void ReadLock() override {
		}

		bool TryReadLock() override {
			return true;
		}

		void ReadUnlock() override {
		}

		bool IsReadLocked() const override {
			return true;
		}
	public:
		Mtx m_WriterLock;
		Mtx m_ReaderLock;
		std::atomic<bool> m_bReadLocked;
		std::atomic<bool> m_bWriteLocked;
	};


	using ReaderWriterLockSlim = _ReaderWriterLockSlim<>;
}

