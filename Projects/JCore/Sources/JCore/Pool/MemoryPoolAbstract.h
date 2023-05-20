/*
 * 작성자: 윤정도
 * 생성일: 12/13/2022 4:01:33 PM
 * =====================
 * 모든 메모라풀의 공통 기능!
 */

#pragma once

#include <JCore/Primitives/SmartPtr.h>
#include <JCore/Container/HashMap.h>

#include <JCore/Pool/MemoryPoolStatistics.h>
#include <JCore/Pool/MemoryPoolCaptured.h>

NS_JC_BEGIN

class MemoryPoolAbstract
{
public:
	MemoryPoolAbstract(int slot, const String& name, bool skipIntialize) : m_iSlot(slot), m_Name(name), m_bInitialized(skipIntialize) {}
	MemoryPoolAbstract(bool skipIntialize) : MemoryPoolAbstract(Detail::InvalidSlot_v, nullptr, skipIntialize) {}
	
	virtual ~MemoryPoolAbstract() = default;

	virtual void Initialize(const HashMap<int, int>& allocationMap) = 0;
	virtual void Finalize() = 0;
	virtual void* DynamicPop(int requestSize, int& realAllocatedSize) = 0;	// 요청한 바이트 크기와 반환된 바이트 크기
	virtual void DynamicPush(void* memory, int returnSize) = 0;
	virtual int Algorithm() = 0;
	int Slot() { return m_iSlot; }
	const String& Name() { return m_Name; }
	bool IsInitialized() { return m_bInitialized; }
	
#if DebugMode 
	Int64U GetTotalAllocated() { return m_Statistics.GetTotalAllocated();  }
	Int64U GetTotalUsed() { return m_Statistics.GetTotalUsed(); }
	Int64U GetTotalReturned() { return m_Statistics.GetTotalReturned(); }
	Int64U GetInitAllocated() { return m_Statistics.GetInitAllocated(); }
	Int64U GetNewAllocated() { return m_Statistics.GetNewAllocated(); }

	int GetBlockTotalCounter(int blockIndex) {
		DebugAssertMsg(blockIndex >= 0 && blockIndex <= Detail::MemoryBlockSizeMapSize_v, "유효한 범위의 블록인덱스가 아닙니다.");
		return m_Statistics.m_pBlockTotalCounter[blockIndex];
	}

	int GetBlockUsedCounter(int blockIndex) {
		DebugAssertMsg(blockIndex >= 0 && blockIndex <= Detail::MemoryBlockSizeMapSize_v, "유효한 범위의 블록인덱스가 아닙니다.");
		return m_Statistics.m_pBlockUsedCounter[blockIndex];
	}

	int GetBlockNewAllocCounter(int blockIndex) {
		DebugAssertMsg(blockIndex >= 0 && blockIndex <= Detail::MemoryBlockSizeMapSize_v, "유효한 범위의 블록인덱스가 아닙니다.");
		return m_Statistics.m_pBlockNewAllocCounter[blockIndex];
	}

	int GetBlockUsingCounter(int blockIndex) {
		DebugAssertMsg(blockIndex >= 0 && blockIndex <= Detail::MemoryBlockSizeMapSize_v, "유효한 범위의 블록인덱스가 아닙니다.");
		return m_Statistics.m_pBlockUsingCounter[blockIndex];
	}

	void ResetStatistics() { m_Statistics.Reset(); }
	bool HasUsingBlock() { return m_Statistics.HasUsingBlock();  }
	

	void StartDetectLeak() {
		CancelDetectLeak();
		m_bDetecting = true;
	}

	template <bool KeepDetectingState = true>	// 릭 디텍팅 상태를 복구할지
	Int64U StopDetectLeak(JCORE_OUT_OPT int* detail = nullptr) {
		DebugAssertMsg(Detecting(), "어라? StartDetectLeak()이 호출되지 않았어요.");
		Int64U uiLeakedBytes = 0;

		for (int i = 0; i < Detail::MemoryBlockSizeMapSize_v; ++i) {
			int iLeakedBlockCount = m_pBlockUsedCounter[i] - m_pBlockReturnedCounter[i];
			uiLeakedBytes += static_cast<Int64U>(iLeakedBlockCount) * Detail::AllocationLengthMapConverter::ToSize(i);
			if (detail) detail[i] = iLeakedBlockCount;

			if (i == 0) Console::WriteLine("┌ [릭 탐지 결과]");
			if (iLeakedBlockCount > 0) Console::WriteLine("│ [%8d]: %d", Detail::AllocationLengthMapConverter::ToSize(i), iLeakedBlockCount);
			if (i == Detail::MemoryBlockSizeMapSize_v - 1) Console::WriteLine("└──────────────── %llu바이트 릭 <", uiLeakedBytes);
		}

		if constexpr (!KeepDetectingState)
			m_bDetecting = false;

		return uiLeakedBytes;
	}

	void CancelDetectLeak() {
		m_bDetecting = false;
		Memory::Copy(m_pBlockUsedCounter, Detail::MemoryBlockSizeMapSize_v, m_pBlockUsedCounter, 0);
		Memory::Copy(m_pBlockReturnedCounter, Detail::MemoryBlockSizeMapSize_v, m_pBlockReturnedCounter, 0);
	}

	bool Detecting() { return m_bDetecting; }

protected:
	void AddInitBlock(Int32 blockIndex, Int32 blockCount) { m_Statistics.AddInitBlock(blockIndex, blockCount); }
	void AddAllocated(Int32 blockIndex, bool createNew) {
		m_Statistics.AddAllocated(blockIndex, createNew);
		if (m_bDetecting) ++m_pBlockUsedCounter[blockIndex];
	}
	void AddDeallocated(Int32 blockIndex) {
		m_Statistics.AddDeallocated(blockIndex);
		if (m_bDetecting) ++m_pBlockReturnedCounter[blockIndex];
	}

	bool m_bDetecting{};
	MemoryPoolStatistics m_Statistics;

	// TODO: 멀티 쓰레딩 디텍션을 수행할려면 쓰레드로컬처럼 동작해야함.
	// 근데 멤버변수로는 쓰레드로컬로 둘 수 없으므로 다른 방식으로 구현해야함.
	// 이거 해결안하면 멀티쓰레드기반환경에서 메모리릭 제대로 못잡아냄;
	// 일단 싱글쓰레드로 가정하고 릭 감지기능 자체만 구현먼저 해놓자.
	int m_pBlockUsedCounter[Detail::MemoryBlockSizeMapSize_v]{};			// 블록 종류별로 몇번 사용되었는지
	int m_pBlockReturnedCounter[Detail::MemoryBlockSizeMapSize_v]{};		// 블록 종류별로 몇번 반환되었는지
#else
public:
	Int64U GetTotalAllocated() { return 0; }
	Int64U GetTotalUsed() { return 0; }
	Int64U GetTotalReturned() { return 0; }
	Int64U GetInitAllocated() { return 0; }
	Int64U GetNewAllocated() { return 0; }

	int GetBlockTotalCounter(int blockIndex) { return 0; }
	int GetBlockUsedCounter(int blockIndex) { return 0; }
	int GetBlockNewAllocCounter(int blockIndex) { return 0;  }
	int GetBlockUsingCounter(int blockIndex) { return 0; }
	void ResetStatistics() { }
	bool HasUsingBlock() { return false; }

	void StartDetectLeak() {}
	template <bool KeepDetectingState = true>
	Int64U StopDetectLeak(JCORE_OUT_OPT int* detail = nullptr) { return 0; }
	bool Detecting() { return false; }
	void CancelDetectLeak() {}
protected:
	void AddInitBlock(Int32 blockIndex, Int32 blockCount) { }
	void AddAllocated(Int32 blockIndex, bool createNew) { }
	void AddDeallocated(Int32 blockIndex) { }
#endif 
protected:
	int m_iSlot;
	String m_Name;
	bool m_bInitialized;

	friend class MemoryPoolManager;
};

using MemoryPoolAbstractPtr = SharedPtr<MemoryPoolAbstract>;


NS_JC_END