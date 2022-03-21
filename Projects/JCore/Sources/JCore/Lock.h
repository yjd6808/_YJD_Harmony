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
	/// ����������� ���ؽ�
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


	
	// �׳� WaitForSingleObject�� ����ؼ��� ���ؽ��� ������ �� ���� �������ؼ� ������� ���� ���ؽ��̴�.
	// �׽�Ʈ �غ��� ����� ����� ���������� ������ ���� ������.
	// 
	// �̺�Ʈ ����
	// @���� : https://docs.microsoft.com/en-us/windows/win32/api/synchapi/nf-synchapi-waitforsingleobject#return-value - WaitForSingleObject ��ȯ�� ����
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


	// ���ɶ� ����
	// @����1 : https://faydoc.tripod.com/cpu/xchg.htm - XCHG ��ɾ�
	// @����2 : https://en.wikipedia.org/wiki/Spinlock - Wikipedia SpinLock�̶�
	// @����3 : https://rigtorp.se/spinlock/ - �Ϻ��� ���ɶ� (���� ����)
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
		int m_iAcquiredThreadCount;		// ����� ȹ���� ������ ��
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
		std::mutex m_ReadLock;				// ���ÿ� ���� �Լ��� ����Ǵ� �� ����.
		std::mutex m_WriteLock;				// ���ÿ� ���� �Լ��� ����Ǵ� �� ����.
		std::atomic<int> m_iReaderCount;
		std::atomic<bool> m_bWriting;
		std::condition_variable m_Cv;
	};


	using ReaderWriterLockSlim = typename _ReaderWriterLockSlim<3>;
}

