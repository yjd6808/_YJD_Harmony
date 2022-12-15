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
#include <JCore/Allocator/DefaultPoolAllocator.h>
#include <JCore/Pool/MemoryPool.h>

using namespace std;

#if TEST_MemoryPoolTest == ON


TEST(MemoryPoolTest, Leak) {
	MemoryPoolLeakCheck;
	UniquePtr<int> pt = MakeUnique<int[], DefaultPoolAllocator>(16);


}


#endif // TEST_AutoObjectTest == ON
