#include <jc_gtest/Core.h>
#include <jc/Random.h>

using namespace std;

#if TEST_RandomTest == ON

static constexpr int Max_v = 1'000'000;

TEST(RandomTest, RandomTestInt)
{
	vector vec(Max_v, 0);

	Random::EngineInitialize();
	for (int i = 0; i < Max_v; ++i)
	{
		++vec[jc::Random::GenerateInt(0, Max_v)];
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
		Int64U conv;
		double original;
	} a;
	vector vec(Max_v, 0);
	for (int i = 0; i < Max_v; ++i)
	{
		a.original = jc::Random::GenerateDouble(0, Max_v);
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
