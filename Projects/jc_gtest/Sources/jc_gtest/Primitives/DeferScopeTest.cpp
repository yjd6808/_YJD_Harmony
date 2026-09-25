/*
 * 작성자: 윤정도
 * 생성일: 9/25/2026
 * =====================
 *
 */

#include "jc/Primitives/DeferScope.h"

#if TEST_DeferScopeTest == ON

TEST(DeferScopeTest, RunsOnScopeExit) {
	int counter = 0;
	{
		JC_DEFER_SCOPE(&counter) {
			counter++;
		};
		EXPECT_EQ(counter, 0);
	}
	EXPECT_EQ(counter, 1);
}

static int earlyReturnCounter = 0;

static void EarlyReturn(bool early) {
	JC_DEFER_SCOPE(&) {
		earlyReturnCounter++;
	};
	if (early)
		return;
	earlyReturnCounter += 10;
}

TEST(DeferScopeTest, RunsOnEarlyReturn) {
	earlyReturnCounter = 0;
	EarlyReturn(true);
	EXPECT_EQ(earlyReturnCounter, 1);
	EarlyReturn(false);
	EXPECT_EQ(earlyReturnCounter, 12);
}

TEST(DeferScopeTest, Captures) {
	int a = 1;
	int b = 2;
	int sum = 0;
	{
		JC_DEFER_SCOPE(=, &sum) {
			sum = a + b;
		};
	}
	EXPECT_EQ(sum, 3);

	{
		JC_DEFER_SCOPE(&a, &b) {
			a = 10;
			b = 20;
		};
	}
	EXPECT_EQ(a, 10);
	EXPECT_EQ(b, 20);

	{
		JC_DEFER_SCOPE(&) {
			a++;
			b++;
		};
	}
	EXPECT_EQ(a, 11);
	EXPECT_EQ(b, 21);
}

TEST(DeferScopeTest, MultipleInSameScope) {
	int order[2] = { 0, 0 };
	int index = 0;
	{
		JC_DEFER_SCOPE(&) {
			order[index++] = 1;
		};
		JC_DEFER_SCOPE(&) {
			order[index++] = 2;
		};
	}
	EXPECT_EQ(index, 2);
	EXPECT_EQ(order[0], 2);
	EXPECT_EQ(order[1], 1);
}

TEST(DeferScopeTest, Dismiss) {
	int counter = 0;
	{
		ScopeExit guard([&]() { counter++; });
		guard.Dismiss();
	}
	EXPECT_EQ(counter, 0);
}

TEST(DeferScopeTest, RunsDuringException) {
	int counter = 0;
	try {
		JC_DEFER_SCOPE(&counter) {
			counter++;
		};
		throw 1;
	} catch (int) {
	}
	EXPECT_EQ(counter, 1);
}

TEST(DeferScopeTest, DirectScopeExit) {
	int counter = 0;
	{
		ScopeExit guard([&]() { counter++; });
		EXPECT_EQ(counter, 0);
	}
	EXPECT_EQ(counter, 1);
}

#endif
