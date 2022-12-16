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
#include <JCore/Pool/MemoryPoolManager.h>

using namespace std;

#if TEST_MemoryPoolTest == ON


TEST(MemoryPoolTest, General) {
	LeakCheck;
	MemoryPoolManager manager;
	manager.Register<eSingle, eBinarySearch>(0, "정도");
	manager.Initialize<eSingle, eBinarySearch>(0, {
		{1, 32},
		{2, 32},
		{4, 32},
		{8, 32},
		{16, 32}
	});
	MemoryPool<eSingle, eBinarySearch>* pool = manager.Get<eSingle, eBinarySearch>(0);

	Int64U uiExpectTotalAllocated = (32 + 2 * 32 + 4 * 32 + 8 * 32 + 16 * 32);

	EXPECT_TRUE(pool->GetInitAllocated() == uiExpectTotalAllocated);
	
	void* pAlloc = pool->StaticPop<13>(); // 13바이트 요청 / 16바이트 할당해줌
	EXPECT_TRUE(pool->GetNewAllocated() == 0);
	EXPECT_TRUE(pool->GetInitAllocated() == uiExpectTotalAllocated);
	EXPECT_TRUE(pool->GetTotalReturned() == 0);
	pool->StaticPush<16>(pAlloc);
	EXPECT_TRUE(pool->GetTotalReturned() == 16);
	EXPECT_TRUE(pool->GetTotalUsed() == 16);
	EXPECT_TRUE(pool->GetNewAllocated() == 0);
	EXPECT_TRUE(pool->GetInitAllocated() == uiExpectTotalAllocated);
	

	int allocated;
	pAlloc = pool->DynamicPop(47, allocated); // 47바이트 요청 / 64바이트 할당
	EXPECT_TRUE(pool->GetTotalReturned() == 16);
	EXPECT_TRUE(pool->GetTotalUsed() == 16 + 64);
	EXPECT_TRUE(pool->GetNewAllocated() == 64);	// 블록이 없으므로 생성해서 반환했으므로
	EXPECT_TRUE(pool->GetInitAllocated() == uiExpectTotalAllocated);


	pool->DynamicPush(pAlloc, allocated);
	EXPECT_TRUE(pool->GetTotalReturned() == 16 + 64);	// 반환했으므로
	EXPECT_TRUE(pool->GetTotalUsed() == 16 + 64);
	EXPECT_TRUE(pool->GetNewAllocated() == 64);
	EXPECT_TRUE(pool->GetInitAllocated() == uiExpectTotalAllocated);

	manager.FinalizeAll();
}


#endif // TEST_AutoObjectTest == ON
