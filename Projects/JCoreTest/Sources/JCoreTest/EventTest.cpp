#include <JCoreTest/CoreTest.h>
#include <JCore/Event.h>

using namespace JCore;

#if TEST_EventTest == ON

TEST(EventTest, General) {
	Event<> e;

	int test = 0;

	auto lmb1 = [&test]() { test += 1; };
	auto lmb2 = [&test]() { test += 2; };

	struct functor1 { int& ref; void operator()() const { ref += 3; } };
	struct functor2 { int& ref; void operator()() const { ref += 4; } };

	e += lmb1;				// named lambda
	e += lmb2;				// named lambda
	e += functor1{ test };	// unnamed functor
	e += functor2{ test };	// unnamed functor

	e();	// (+10) lmb1, lmb2, functor1, functor2;
	EXPECT_TRUE(test == 10);

	e -= lmb2;
	e();	// (+8) lmb2, functor1, functor2
	EXPECT_TRUE(test == 18);	

	e -= lmb1;
	e();	// (+7) functor1, functor2
	EXPECT_TRUE(test == 25);

	e.UnregisterByType(typeid(functor1)); // (+4) functor2
	e();	
	EXPECT_TRUE(test == 29);

	e.UnregisterByType(typeid(functor2)); // (+0) ����
	e();
	EXPECT_TRUE(test == 29);

	functor1 ex_1{ test };
	functor2 ex_2{ test };

	e += ex_1;
	e += ex_2;
	e();	// (+7) ex_1, ex_2
	EXPECT_TRUE(test == 36);

	e -= ex_1;
	e();	// (+4) ex_2
	EXPECT_TRUE(test == 40);
}


int g_event_test_val;
TEST(EventTest, ClassMethod) {
	Event<int, int> e;

	struct Legend
	{
		String name[20];

		static void test(int a, int b) { g_event_test_val++; }
		void member_test(int a, int b) { g_event_test_val++; }
		void member_test2(int a, int b) { g_event_test_val++; }
	};

	Legend t1{ "t1" };
	Legend t2{ "t2" };

	e += Legend::test;		// ���� �Լ��� ����
	// e += t1.member_test;	// �̷��� ��� �Լ��� �߰��� ���� ����

	// ���1
	e += std::bind(&Legend::member_test, &t1, std::placeholders::_1, std::placeholders::_2);
	e += std::bind(&Legend::member_test, &t2, std::placeholders::_1, std::placeholders::_2);

	// ���2
	e += [&t1](int a, int b) {
		t1.member_test(a, b);
	};
	e += [&t2](int a, int b) {
		t2.member_test(a, b);
	};
	
	


	// ȣ��
	e(1, 5);
	EXPECT_TRUE(g_event_test_val == 5);

	e -= Legend::test;
	e(1, 5);
	EXPECT_TRUE(g_event_test_val == 9);
}


#endif