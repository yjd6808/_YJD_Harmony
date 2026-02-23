#include <jc_gtest/Core.h>
#include <jc/Random.h>

using namespace std;

#if TEST_RandomTest == ON

static constexpr int Max_v = 1'000'000;

TEST(RandomTest, RandomTestInt)
{
	vector vec(Max_v, 0);

	Random r(true);
	for (int i = 0; i < Max_v; ++i)
	{
		++vec[r.GenerateInt(0, Max_v)];
	}

	int equal = 0;
	for (int i = 0; i < Max_v; ++i)
	{
		if ((vec[i] - 1) > 0)
		{
			equal += vec[i] - 1;
		}
	}
	EXPECT_TRUE(double(Max_v - equal) / Max_v > 0.6);
}

//////////////////////////////////////////////////////////////////////////////////////////
TEST(RandomTest, RandomTestDouble)
{
	union Gr
	{
		_u64 conv;
		double original;
	} a;
	Random r(true);
	vector vec(Max_v, 0);
	for (int i = 0; i < Max_v; ++i)
	{
		a.original = r.GenerateDouble(0, Max_v);
		++vec[a.conv % Max_v];
	}

	int equal = 0;
	for (int i = 0; i < Max_v; ++i)
	{
		if ((vec[i] - 1) > 0)
		{
			equal += vec[i] - 1;
		}
	}

	EXPECT_TRUE(double(Max_v - equal) / Max_v > 0.6);
}

#endif // TEST_RandomTest == ON
