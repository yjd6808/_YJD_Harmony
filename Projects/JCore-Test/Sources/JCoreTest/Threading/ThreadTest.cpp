/*
 * 작성자: 윤정도
 * 생성일: 12/10/2022 6:27:55 PM
 * =====================
 *
 */


#include <JCoreTest/CoreTest.h>
#include <JCore/Threading/Thread.h>
#include <JCore/Container/Vector.h>

using namespace std;

#if TEST_ThreadTest == ON

TEST(Test, Thread) {
	{
		// 시작안하는 경우
		LeakCheck;
		Thread thread("정도의 쓰레드");
		EXPECT_FALSE(thread.Join());
		thread.AutoJoin(true);

		// 그냥
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
			threads.EmplaceBack([](void* v) { ++g; });
		for (int j = 0; j < 16; ++j) 
			EXPECT_TRUE(threads[j].Join());

		EXPECT_TRUE(g == 16 * (i + 1));
		threads.Clear();
	}
}

#endif // TEST_ThreadTest == ON
