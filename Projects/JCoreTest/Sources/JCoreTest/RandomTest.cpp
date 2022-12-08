#include <JCoreTest/CoreTest.h>
#include <JCore/Random.h>

using namespace std;

#if TEST_RandomTest == ON

static constexpr int Max = 1'000'000;

TEST(RandomTest, RandomTestInt) {
	
	vector vec(Max, 0);

    Random::EngineInitialize();
	for (int i = 0; i < Max; ++i) {
        ++vec[JCore::Random::GenerateInt(0, Max)];
	}

    int equal = 0;
    for (int i = 0; i < Max; ++i) {
        if ((vec[i] - 1) > 0) {
            equal += vec[i] - 1;
        }
    }
    EXPECT_TRUE(double(Max - equal) / Max > 0.6);
}


TEST(RandomTest, RandomTestDouble) {
    union Gr {
        Int64U conv;
        double original;
    } a;
    vector vec(Max, 0);
    for (int i = 0; i < Max; ++i) {
        a.original = JCore::Random::GenerateDouble(0, Max);
        ++vec[a.conv % Max];
    }

    int equal = 0;
    for (int i = 0; i < Max; ++i) {
        if ((vec[i] - 1) > 0) {
            equal += vec[i] - 1;
        }
    }

    EXPECT_TRUE(double(Max - equal) / Max > 0.6);
}

#endif // TEST_RandomTest == ON