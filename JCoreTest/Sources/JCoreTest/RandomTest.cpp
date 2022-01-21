#include <JCoreTest/CoreTest.h>
#include <JCore/Random.h>

using namespace JCore;
using namespace std;

#if TEST_RandomTest == ON

TEST(RandomTest, RandomTestInt) {
	constexpr int MAX = 100;

	Random rand;
	vector<int> vec[MAX];
	for (int k = 0; k < MAX; k++) {
		for (int i = 0; i < MAX; i++) {
			vec[k].push_back(rand.GenerateInt(0, 1000000));
		}
	}

	auto comp = [MAX](auto& lhs, auto& rhs)->int {
		int diffCount = MAX;
		for (int i = 0; i < lhs.size(); i++) {
			for (int j = 0; j < rhs.size(); j++) {
				if (lhs[i] == rhs[j]) {
					diffCount--;
					break;
				}
			}
		}
		return diffCount;
	};

	for (int i = 0; i < MAX; i++) {
		for (int j = i + 1; j < MAX; j++) {
			int diff = comp(vec[i], vec[j]);
			if (diff <= 97) {
				EXPECT_TRUE(false);
			}
		}
	}
}


TEST(RandomTest, RandomTestDouble) {
	constexpr int MAX = 100;

	Random rand;
	vector<double> vec[MAX];
	for (int k = 0; k < MAX; k++) {
		for (int i = 0; i < MAX; i++) {
			vec[k].push_back(rand.GenerateDouble(0, 1000000));
		}
	}

	auto comp = [MAX](auto& lhs, auto& rhs)->int {
		int diffCount = MAX;
		for (int i = 0; i < lhs.size(); i++) {
			for (int j = 0; j < rhs.size(); j++) {
				if (std::abs(lhs[i] - rhs[j]) <= 0.01) {
					diffCount--;
					break;
				}
			}
		}
		return diffCount;
	};

	for (int i = 0; i < MAX; i++) {
		for (int j = i + 1; j < MAX; j++) {
			int diff = comp(vec[i], vec[j]);
			if (diff <= 98) {
				EXPECT_TRUE(false);
			}
		}
	}
}

#endif // TEST_RandomTest == ON