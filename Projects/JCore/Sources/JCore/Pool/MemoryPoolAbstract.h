/*
 * 작성자: 윤정도
 * 생성일: 12/13/2022 4:01:33 PM
 * =====================
 * 모든 메모라풀의 공통 기능!
 */

#pragma once

#include <JCore/Pool/MemoryPoolStatistics.h>

namespace JCore {

	template <typename, typename>
	class HashMap;
	class MemoryPoolAbstract
	{
	public:
		MemoryPoolAbstract(int slot, const String& name, bool skipIntialize): m_iSlot(slot), m_Name(name), m_bInitialized(skipIntialize) {}
		virtual ~MemoryPoolAbstract() = default;

		virtual void Initialize(const HashMap<int, int>& allocationMap) = 0;
		virtual void Finalize() = 0;
		virtual void* DynamicPop(int requestSize, int& realAllocatedSize) = 0;	// 요청한 바이트 크기와 반환된 바이트 크기
		virtual void DynamicPush(void* memory, int returnSize) = 0;
		virtual int Strategy() = 0;
		virtual int Algorithm() = 0;
		int Slot() { return m_iSlot; }
		const String& Name() { return m_Name; }

		Int64U TotalAllocatedBytes() { return m_Statistics.GetTotalAllocated();  }
		Int64U TotalUsedBytes() { return m_Statistics.GetTotalUsed(); }
		Int64U InitAllocatedBytes() { return m_Statistics.GetInitAllocated(); }
		Int64U NewAllocatedBytes() { return m_Statistics.GetNewAllocated(); }
		void ResetStatistics() { m_Statistics.Reset(); }

		void AddInitBlock(Int32 blockIndex) { m_Statistics.AddInitBlock(blockIndex);  }
		void AddAllocated(Int32 blockIndex, bool createNew) {
			m_Statistics.AddAllocated(blockIndex, createNew);
			if (m_bDetecting) ++m_pBlockUsedCounter[blockIndex];
		}
		void AddDeallocated(Int32 blockIndex) {
			m_Statistics.AddDeallocated(blockIndex);
			if (m_bDetecting) ++m_pBlockReturnedCounter[blockIndex];
		}
		bool HasUsingBlock() { return m_Statistics.HasUsingBlock();  }
		bool IsInitialized() { return m_bInitialized; }

		void StartDetectLeak() {
			CaencelDetectLeak();
			m_bDetecting = true;
		}

		Int64U StopDetectLeak(OutOpt_ int* detail = nullptr) {
			DebugAssertMessage(Detecting(), "어라? StartDetectLeak()이 호출되지 않았어요.");
			Int64U uiLeakedBytes = 0;

			for (int i = 0; i < Detail::MemoryBlockSizeMapSize_v; ++i) {
				int iLeakedBlockCount = m_pBlockUsedCounter[i] - m_pBlockReturnedCounter[i];
				uiLeakedBytes += static_cast<Int64U>(iLeakedBlockCount) * Detail::AllocationLengthMapConverter::ToSize(i);
				if (detail) detail[i] = iLeakedBlockCount;
			}

			m_bDetecting = false;
			return uiLeakedBytes;
		}

		void CaencelDetectLeak() {
			m_bDetecting = false;
			Memory::Copy(m_pBlockUsedCounter, Detail::MemoryBlockSizeMapSize_v, m_pBlockUsedCounter, 0);
			Memory::Copy(m_pBlockReturnedCounter, Detail::MemoryBlockSizeMapSize_v, m_pBlockReturnedCounter, 0);
		}

		
		bool Detecting() { return m_bDetecting; }

	protected:
		int m_iSlot;
		String m_Name;
		bool m_bInitialized;

		bool m_bDetecting{};
		MemoryPoolStatistics m_Statistics;

		// TODO: 멀티 쓰레딩 디텍션을 수행할려면 쓰레드로컬처럼 동작해야함.
		// 근데 멤버변수로는 쓰레드로컬로 둘 수 없으므로 다른 방식으로 구현해야함.
		// 이거 해결안하면 멀티쓰레드기반환경에서 메모리릭 제대로 못잡아냄;
		//
		// 일단 싱글쓰레드로 가정하고 릭 감지기능 자체만 구현먼저 해놓자.
		

		int m_pBlockUsedCounter[Detail::MemoryBlockSizeMapSize_v]{};			// 블록 종류별로 몇번 사용되었는지
		int m_pBlockReturnedCounter[Detail::MemoryBlockSizeMapSize_v]{};		// 블록 종류별로 몇번 사용되었는지

		friend class MemoryPoolManager;
	};

} // namespace JCore