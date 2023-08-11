#include <JCoreTest/CoreTest.h>
#include <JCore/Event.h>



#if TEST_EventTest == ON

TEST(EventTest, General) {
	Event<> e;

	int test = 0;

	auto lmb1 = [&test]() { test += 1; };
	auto lmb2 = [&test]() { test += 2; };

	struct functor1 { int& ref; void operator()() const { ref += 3; } };
	struct functor2 { int& ref; void operator()() const { ref += 4; } };

	e.Register(0, lmb1);							// named lambda
	e.Register(1, lmb2);							// named lambda

	e.Register(2, functor1{ test });				// unnamed functor
	e.Register(3, functor2{ test });				// unnamed functor

	e();	// (+10) lmb1, lmb2, functor1, functor2;
	EXPECT_TRUE(test == 10);

	e.Unregister(1);
	e();	// (+8) lmb2, functor1, functor2
	EXPECT_TRUE(test == 18);	

	e.Unregister(0);
	e();	// (+7) functor1, functor2
	EXPECT_TRUE(test == 25);

	e.Unregister(2);
	e();	
	EXPECT_TRUE(test == 29);

	e.Unregister(3);
	e();
	EXPECT_TRUE(test == 29);

	functor1 ex_1{ test };
	functor2 ex_2{ test };

	e.Register(4, ex_1);
	e.Register(5, ex_2);
	e();	// (+7) ex_1, ex_2
	EXPECT_TRUE(test == 36);

	e.Unregister(4);
	e();	// (+4) ex_2
	EXPECT_TRUE(test == 40);
}


int g_event_testal;
TEST(EventTest, ClassMethod) {
	Event<int, int> e;

	struct Legend
	{
		String name[20];

		static void test(int a, int b) { g_event_testal++; }
		void member_test(int a, int b) { g_event_testal++; }
		void member_test2(int a, int b) { g_event_testal++; }
	};

	Legend t1{ "t1" };
	Legend t2{ "t2" };

	e.Register(0, Legend::test);

	// 방법1
	e.Register(1, std::bind(&Legend::member_test, &t1, std::placeholders::_1, std::placeholders::_2));
	e.Register(2, std::bind(&Legend::member_test, &t1, std::placeholders::_1, std::placeholders::_2));

	// 방법2
	e.Register(3, [&t1](int a, int b) { t1.member_test(a, b); });
	e.Register(4, [&t2](int a, int b) { t2.member_test(a, b); });

	// 호출
	e(1, 5);
	EXPECT_TRUE(g_event_testal == 5);

	e.Unregister(0);
	e(1, 5);
	EXPECT_TRUE(g_event_testal == 9);
}


#endif