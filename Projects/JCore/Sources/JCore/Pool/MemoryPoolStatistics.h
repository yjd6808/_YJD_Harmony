/*
 * 작성자: 윤정도
 * 생성일: 12/14/2022 1:38:21 PM
 * =====================
 * 메모리풀 통계 기록
 * 실제 성능에 영향을 줄 수 있으므로 디버그시에만 사용토록 해야한다.
 * 아예 런타임중에 영향을 주지 않도록 구현할 것
 */


#pragma once

#include <JCore/Sync/ILock.h>
#include <JCore/Pool/MemoryPoolDetail.h>


NS_JC_BEGIN

class MemoryPoolStatistics
{
public:
	Int64U GetInitAllocated() { return m_uiInitAllocted; }
	Int64U GetNewAllocated() { return m_uiNewAlloctaed; }
	Int64U GetTotalAllocated() { return m_uiInitAllocted + m_uiNewAlloctaed; }	// 메모리풀이 관리중인 메모리 크기
	Int64U GetTotalUsed() { return m_uiTotalUsed; }
	Int64U GetTotalReturned() { return m_uiTotalReturned; }

	int GetBlockTotalCounter(int blockIndex) { return m_pBlockTotalCounter[blockIndex]; }
	int GetBlockUsedCounter(int blockIndex) { return m_pBlockUsedCounter[blockIndex]; }
	int GetBlockNewAllocCounter(int blockIndex) { return m_pBlockNewAllocCounter[blockIndex]; }
	int GetBlockUsingCounter(int blockIndex) { return m_pBlockUsingCounter[blockIndex]; }
protected:
	void AddInitBlock(Int32 blockIndex, Int32 blockCount) {
		m_uiInitAllocted += static_cast<Int64U>(Detail::AllocationLengthMapConverter::ToSize(blockIndex)) * blockCount;
		m_pBlockTotalCounter[blockIndex] += blockCount;
	}

	void AddAllocated(Int32 blockIndex, bool createNew) {
		int iSize = Detail::AllocationLengthMapConverter::ToSize(blockIndex);

		m_uiTotalUsed += iSize;

		if (createNew) {
			m_uiNewAlloctaed += iSize;
			++m_pBlockTotalCounter[blockIndex];
			++m_pBlockNewAllocCounter[blockIndex];
		}
		++m_pBlockUsingCounter[blockIndex];
		++m_pBlockUsedCounter[blockIndex];
	}

	void AddDeallocated(Int32 blockIndex) {
		--m_pBlockUsingCounter[blockIndex];
		m_uiTotalReturned += Detail::AllocationLengthMapConverter::ToSize(blockIndex);
	}



	void Reset() {
		m_uiInitAllocted = 0;
		m_uiNewAlloctaed = 0;
		m_uiTotalUsed = 0;
		m_uiTotalReturned = 0;

		Arrays::Fill(m_pBlockTotalCounter, 0);
		Arrays::Fill(m_pBlockUsedCounter, 0);
		Arrays::Fill(m_pBlockNewAllocCounter, 0);
		Arrays::Fill(m_pBlockUsingCounter, 0);
	}

	// 현재 사용중인 블록이 있는지
	bool HasUsingBlock() { return Arrays::FindIf(m_pBlockUsingCounter, [](const int& count) { return count > 0; }) != Detail::InvalidIndex_v; }
protected:
	Int64U m_uiInitAllocted{};		//	MemoryPool::Initialize()시 제일 처음 할당된 메모리양
	Int64U m_uiNewAlloctaed{};		//	MemoryPool::Initialize()때 할당된 메모리외에! 추가로 새로 할당된 메모리양 (누적)
	Int64U m_uiTotalUsed{};			//	메모리풀을 얼마나 사용했는지
	Int64U m_uiTotalReturned{};		//  메모리풀로 얼마나 반환되었는지

	int m_pBlockTotalCounter[Detail::MemoryBlockSizeMapSize_v]{};			// 블록 종류별로 현재 몇개가 있는지 기록
	int m_pBlockUsedCounter[Detail::MemoryBlockSizeMapSize_v]{};			// 블록 종류별로 몇번 사용되었는지
	int m_pBlockNewAllocCounter[Detail::MemoryBlockSizeMapSize_v]{};		// 블록 종류별로 몇번 생성 할당되었는지 기록
	int m_pBlockUsingCounter[Detail::MemoryBlockSizeMapSize_v]{};			// 블록 종류별로 사용중인 블록 수 기록

	friend class MemoryPoolAbstract;
};

NS_JC_END

