/*
 * 작성자: 윤정도
 * 생성일: 12/20/2022 3:23:36 PM
 * =====================
 *
 */


#include <JCoreTest/CoreTest.h>
#include <JCore/Threading/ThreadLocal.h>


#if TEST_ThreadLocalTest == ON

 // 전역으로 둘 경우 thread_local을 그냥 쓰면 되지만 내가 만든 기능이 잘동작하는 확인하기 위해 둠
static ThreadLocal<int> global;
static constexpr int maxThread = 20;
static int section[maxThread];

// 기존 thread_local처럼 동작하는지 테스트
TEST(ThreadLocalTest, ThreadSafeTest) 
{
	vector<thread> ths;
	for (int i = 0; i < maxThread; ++i) {
		ths.emplace_back([i]() {
			for (int j = 0; j < 20'000; j++)
				section[i] = ++global.Ref();
		});
	}
	for (int i = 0; i < maxThread; ++i) {
		ths[i].join();
	}
	for (int i = 0; i < maxThread; ++i) {
		EXPECT_TRUE(section[i] == 20'000);
	}
}


TEST(ThreadLocalTest, ClassMemberTest) {
	Arrays::Fill(section, 0);
	
	struct Test
	{
		ThreadLocal<int> val{2000};
	};

	Test test;
	int result[maxThread];

	
	vector<thread> ths;

	EXPECT_TRUE(test.val.Ref() == 2000);
	for (int i = 0; i < maxThread; ++i) {
		ths.emplace_back([i, &test, &result]() {
			for (int j = 0; j < 100; j++)
				++test.val.Ref();
			result[i] = test.val.Ref();
		});
	}
	for (int i = 0; i < maxThread; ++i) {
		ths[i].join();
	}

	for (int i = 0; i < maxThread; i++)
		EXPECT_TRUE(result[i] == 2100);
}

struct Legion
{
	Legion() {}
	Legion(int k): a(k) {}
	Legion(const Legion& k) { std::cout << "레기온 복사\n"; a = k.a; }
	int a;
};

TEST(ThreadLocalTest, SameTypeTest) {

	{
		ThreadLocal<Legion> a{ 1 };
		ThreadLocal<Legion> b{ 2 };
		ThreadLocal<Legion> c{ 3 };
		ThreadLocal<Legion> d{ 4 };
		ThreadLocal<Legion> e{ 5 };

		EXPECT_TRUE(a.Ref().a == 1);
		EXPECT_TRUE(b.Ref().a == 2);
		EXPECT_TRUE(c.Ref().a == 3);
		EXPECT_TRUE(d.Ref().a == 4);
		EXPECT_TRUE(e.Ref().a == 5);
	}


	
}


#endif