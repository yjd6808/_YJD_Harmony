/*
 * 작성자: 윤정도
 * 생성일: 12/10/2022 6:27:55 PM
 * =====================
 *
 */


#include <JCoreTest/CoreTest.h>
#include <JCore/Threading/Thread.h>
#include <JCore/Container/Vector.h>

USING_NS_STD;

#if TEST_ThreadTest == ON

TEST(Test, Thread) {
	{
		// 시작안하는 경우
		LeakCheck;
		Thread thread("정도의 쓰레드");
		EXPECT_TRUE(thread.Join() != Thread::eSuccess);	// 실패해야함
		thread.AutoJoin(true);
	}

	{
		// 안전 종료
		LeakCheck;
		Thread thread("정도의 쓰레드");
		thread.AutoJoin(true);
		thread.Start([](void*){});
	}

	{
		// 강제 종료 (주석 해제)
		LeakCheck;
		Thread thread("정도의 쓰레드");
		/*thread.Start([](void*) {
			for (int i = 0; i < 1'100'000'000; i++) {
				std::cout << i << "\n";
			}
		});*/
	}
}

std::atomic<int> g = 0;
TEST(Vector, Thread) {
	LeakCheck;
	Vector<Thread> threads;

	for (int i = 0; i < 48; ++i) {
		for (int j = 0; j < 16; ++j) 
			threads.EmplaceBack([](void* v) {
				++g;
			});
		for (int j = 0; j < 16; ++j) 
			EXPECT_TRUE(threads[j].Join() == Thread::eSuccess);

		EXPECT_TRUE(g == 16 * (i + 1));
		threads.Clear();
	}
}

#endif // TEST_ThreadTest == ON
