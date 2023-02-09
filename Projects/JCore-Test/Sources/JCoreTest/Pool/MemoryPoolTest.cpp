/*
 * 작성자: 윤정도
 * 생성일: 12/15/2022 11:33:33 PM
 * =====================
 *
 */



 /*
	 작성자 : 윤정도
	 오토 오브젝트 테스트
 */


#include <JCoreTest/CoreTest.h>
#include <JCore/Pool/BinarySearchMemoryPool.h>
#include <JCore/Pool/IndexedMemoryPool.h>

using namespace std;

#if TEST_MemoryPoolTest == ON && DebugMode

template <typename TPool>
static void TestPool(TPool& pool);

TEST(MemoryPoolTest, SingleBinarySearch) {
	LeakCheck;
	BinarySearchMemoryPool pool{ false };
	TestPool(pool);
}

TEST(MemoryPoolTest, SingleFullIndexing) {
	LeakCheck;
	IndexedMemoryPool pool{ false };
	TestPool(pool);
}


template <typename TPool>
static void TestPool(TPool& pool) {

	constexpr Int64U uiExpectTotalAllocated = (32 + 2 * 32 + 4 * 32 + 8 * 32 + 16 * 32);

	// ========================================
	// 할당이 제대로 이뤄지는지 테스트
	// ========================================
	pool.Initialize({
		{1,  32},
		{2,  32},
		{4,  32},
		{8,  32},
		{16, 32}
		});

	int leakDetail[Detail::MemoryBlockSizeMapSize_v];
	pool.StartDetectLeak();		// 릭 체크 시작

	EXPECT_TRUE(pool.GetInitAllocated() == uiExpectTotalAllocated);

	void* pAlloc = pool.StaticPop<13>(); // 13바이트 요청 / 16바이트 할당해줌
	Int64U uiTotalLeaked = pool.StopDetectLeak<true>(leakDetail);
	EXPECT_TRUE(leakDetail[Detail::AllocationLengthMapConverter::ToIndex(13)] == 1);		// 16바이트 블록 1개 릭
	EXPECT_TRUE(uiTotalLeaked == 16);	// 총 16바이트 릭

	EXPECT_TRUE(pool.GetNewAllocated() == 0);
	EXPECT_TRUE(pool.GetInitAllocated() == uiExpectTotalAllocated);
	EXPECT_TRUE(pool.GetTotalReturned() == 0);
	pool.StaticPush<16>(pAlloc);


	EXPECT_TRUE(leakDetail[Detail::AllocationLengthMapConverter::ToIndex(13)] == 1);		// 16바이트 블록 1개 릭
	EXPECT_TRUE(uiTotalLeaked == 16);	// 총 16바이트 릭
	EXPECT_TRUE(pool.GetTotalReturned() == 16);
	EXPECT_TRUE(pool.GetTotalUsed() == 16);
	EXPECT_TRUE(pool.GetNewAllocated() == 0);
	EXPECT_TRUE(pool.GetInitAllocated() == uiExpectTotalAllocated);


	int allocated;
	pAlloc = pool.DynamicPop(47, allocated); // 47바이트 요청 / 64바이트 할당
	EXPECT_TRUE(pool.GetTotalReturned() == 16);
	EXPECT_TRUE(pool.GetTotalUsed() == 16 + 64);
	EXPECT_TRUE(pool.GetNewAllocated() == 64);	// 블록이 없으므로 생성해서 반환했으므로
	EXPECT_TRUE(pool.GetInitAllocated() == uiExpectTotalAllocated);


	pool.DynamicPush(pAlloc, allocated);
	EXPECT_TRUE(pool.GetTotalReturned() == 16 + 64);	// 반환했으므로
	EXPECT_TRUE(pool.GetTotalUsed() == 16 + 64);
	EXPECT_TRUE(pool.GetNewAllocated() == 64);
	EXPECT_TRUE(pool.GetInitAllocated() == uiExpectTotalAllocated);

	// ========================================
	// 통계가 제대로 입력되는지 테스트
	// ========================================

	// 현재 풀이 관리중인 모든 블록
	EXPECT_TRUE(pool.GetBlockTotalCounter(Detail::AllocationLengthMapConverter::ToIndex(64)) == 1);
	EXPECT_TRUE(pool.GetBlockTotalCounter(Detail::AllocationLengthMapConverter::ToIndex(16)) == 32);
	EXPECT_TRUE(pool.GetBlockTotalCounter(Detail::AllocationLengthMapConverter::ToIndex(8)) == 32);
	EXPECT_TRUE(pool.GetBlockTotalCounter(Detail::AllocationLengthMapConverter::ToIndex(4)) == 32);
	EXPECT_TRUE(pool.GetBlockTotalCounter(Detail::AllocationLengthMapConverter::ToIndex(2)) == 32);
	EXPECT_TRUE(pool.GetBlockTotalCounter(Detail::AllocationLengthMapConverter::ToIndex(1)) == 32);

	// 새로 할당받은 블록
	EXPECT_TRUE(pool.GetBlockNewAllocCounter(Detail::AllocationLengthMapConverter::ToIndex(64)) == 1);

	// 현재 사용중인 블록
	EXPECT_TRUE(pool.GetBlockUsingCounter(Detail::AllocationLengthMapConverter::ToIndex(64)) == 0);

	// 각 블록별 사용 횟수
	EXPECT_TRUE(pool.GetBlockUsedCounter(Detail::AllocationLengthMapConverter::ToIndex(16)) == 1);
	EXPECT_TRUE(pool.GetBlockUsedCounter(Detail::AllocationLengthMapConverter::ToIndex(64)) == 1);

	pool.StartDetectLeak();		// 릭 체크 시작
	pAlloc = pool.DynamicPop(47, allocated); // 47바이트 요청 / 64바이트 할당
	EXPECT_TRUE(pool.GetTotalReturned() == 16 + 64);
	EXPECT_TRUE(pool.GetTotalUsed() == 16 + 64 + 64);
	EXPECT_TRUE(pool.GetNewAllocated() == 64);	// 기존에 있었던 블록이 반환될 것이기 때문에 올라가지 않음
	EXPECT_TRUE(pool.GetInitAllocated() == uiExpectTotalAllocated);

	// 현재 풀이 관리중인 모든 블록
	EXPECT_TRUE(pool.GetBlockTotalCounter(Detail::AllocationLengthMapConverter::ToIndex(64)) == 1);
	EXPECT_TRUE(pool.GetBlockTotalCounter(Detail::AllocationLengthMapConverter::ToIndex(16)) == 32);
	EXPECT_TRUE(pool.GetBlockTotalCounter(Detail::AllocationLengthMapConverter::ToIndex(8)) == 32);
	EXPECT_TRUE(pool.GetBlockTotalCounter(Detail::AllocationLengthMapConverter::ToIndex(4)) == 32);
	EXPECT_TRUE(pool.GetBlockTotalCounter(Detail::AllocationLengthMapConverter::ToIndex(2)) == 32);
	EXPECT_TRUE(pool.GetBlockTotalCounter(Detail::AllocationLengthMapConverter::ToIndex(1)) == 32);

	// 새로 할당받은 블록
	EXPECT_TRUE(pool.GetBlockNewAllocCounter(Detail::AllocationLengthMapConverter::ToIndex(64)) == 1);

	// 현재 사용중인 블록 // 1로 변경됨
	EXPECT_TRUE(pool.GetBlockUsingCounter(Detail::AllocationLengthMapConverter::ToIndex(64)) == 1);

	// 각 블록별 사용 횟수
	EXPECT_TRUE(pool.GetBlockUsedCounter(Detail::AllocationLengthMapConverter::ToIndex(16)) == 1);
	EXPECT_TRUE(pool.GetBlockUsedCounter(Detail::AllocationLengthMapConverter::ToIndex(64)) == 2);

	void* pAlloc2 = pool.DynamicPop(47, allocated); // 47바이트 요청 / 64바이트 할당

	// 현재 풀이 관리중인 모든 블록
	EXPECT_TRUE(pool.GetBlockTotalCounter(Detail::AllocationLengthMapConverter::ToIndex(64)) == 2);	// 신규 할당
	EXPECT_TRUE(pool.GetBlockTotalCounter(Detail::AllocationLengthMapConverter::ToIndex(16)) == 32);
	EXPECT_TRUE(pool.GetBlockTotalCounter(Detail::AllocationLengthMapConverter::ToIndex(8)) == 32);
	EXPECT_TRUE(pool.GetBlockTotalCounter(Detail::AllocationLengthMapConverter::ToIndex(4)) == 32);
	EXPECT_TRUE(pool.GetBlockTotalCounter(Detail::AllocationLengthMapConverter::ToIndex(2)) == 32);
	EXPECT_TRUE(pool.GetBlockTotalCounter(Detail::AllocationLengthMapConverter::ToIndex(1)) == 32);

	// 새로 할당받은 블록
	EXPECT_TRUE(pool.GetBlockNewAllocCounter(Detail::AllocationLengthMapConverter::ToIndex(64)) == 2); // 신규 할당

	// 현재 사용중인 블록 
	EXPECT_TRUE(pool.GetBlockUsingCounter(Detail::AllocationLengthMapConverter::ToIndex(64)) == 2);	// 신규 할당

	// 각 블록별 사용 횟수
	EXPECT_TRUE(pool.GetBlockUsedCounter(Detail::AllocationLengthMapConverter::ToIndex(16)) == 1);
	EXPECT_TRUE(pool.GetBlockUsedCounter(Detail::AllocationLengthMapConverter::ToIndex(64)) == 3);

	Arrays::Fill(leakDetail, 0);

	uiTotalLeaked = pool.StopDetectLeak(leakDetail);
	EXPECT_TRUE(leakDetail[Detail::AllocationLengthMapConverter::ToIndex(64)] == 2);		// 64바이트 블록 2개 릭
	EXPECT_TRUE(uiTotalLeaked == 64 + 64);	// 총 바이트 릭

	// 꺼냇던 블록 복귀
	pool.DynamicPush(pAlloc2, allocated);
	pool.DynamicPush(pAlloc, allocated);

	// 현재 풀이 관리중인 모든 블록
	EXPECT_TRUE(pool.GetBlockTotalCounter(Detail::AllocationLengthMapConverter::ToIndex(64)) == 2);
	EXPECT_TRUE(pool.GetBlockTotalCounter(Detail::AllocationLengthMapConverter::ToIndex(16)) == 32);
	EXPECT_TRUE(pool.GetBlockTotalCounter(Detail::AllocationLengthMapConverter::ToIndex(8)) == 32);
	EXPECT_TRUE(pool.GetBlockTotalCounter(Detail::AllocationLengthMapConverter::ToIndex(4)) == 32);
	EXPECT_TRUE(pool.GetBlockTotalCounter(Detail::AllocationLengthMapConverter::ToIndex(2)) == 32);
	EXPECT_TRUE(pool.GetBlockTotalCounter(Detail::AllocationLengthMapConverter::ToIndex(1)) == 32);

	// 새로 할당받은 블록
	EXPECT_TRUE(pool.GetBlockNewAllocCounter(Detail::AllocationLengthMapConverter::ToIndex(64)) == 2);

	// 현재 사용중인 블록 
	EXPECT_TRUE(pool.GetBlockUsingCounter(Detail::AllocationLengthMapConverter::ToIndex(64)) == 0);	// 복귀!

	// 각 블록별 사용 횟수
	EXPECT_TRUE(pool.GetBlockUsedCounter(Detail::AllocationLengthMapConverter::ToIndex(16)) == 1);
	EXPECT_TRUE(pool.GetBlockUsedCounter(Detail::AllocationLengthMapConverter::ToIndex(64)) == 3);




	pool.Finalize();
}



#endif // TEST_AutoObjectTest == ON
