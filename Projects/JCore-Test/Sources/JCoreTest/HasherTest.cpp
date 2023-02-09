/*
	작성자 : 윤정도
	해쉬 생성기 테스트
*/


#include <JCoreTest/CoreTest.h>
#include <JCore/Random.h>
#include <JCore/Hasher.h>


using namespace std;

#if TEST_HasherTest == ON

constexpr int MaxDataCount = 1000000;// 최대로 테스트할 데이터 수
constexpr float SuccessRatio = 50.0;	 // 중복률 통과 율
vector<Int32U> gec;
Random g_rand;

template <typename T>
bool CheckDuplicateRatio(vector<Int32U>& vec) {
	int iOriginalSize = vec.size();
	vec.erase(std::unique(vec.begin(), vec.end()), vec.end());

	double dupRatio = (double)(iOriginalSize - vec.size()) / iOriginalSize * 100.0;
	PrintFormat("(%s)중복 발생률 : %.2f%%(%d개중 %d개가 중복된 데이터로 삭제됨)\n",
		dupRatio, 
		iOriginalSize, 
		iOriginalSize - vec.size());

	vec.clear();

	return dupRatio < SuccessRatio;
}


TEST(HasherTest, Integer) {
	for (Int8 i = MinInt8_v; i < MaxInt8_v; i += 1) {
		gec.push_back(Hasher<Int8>()(i));
	}

	EXPECT_TRUE(CheckDuplicateRatio<Int8>(gec));

	for (Int8U i = 0; i < MaxInt8U_v; i += 1) {
		gec.push_back(Hasher<Int8U>()(i));
	}

	EXPECT_TRUE(CheckDuplicateRatio<Int8U>(gec));

	for (Int16 i = MinInt16_v; i < MaxInt16_v; i += 1) {
		gec.push_back(Hasher<Int16>()(i));
	}
	
	EXPECT_TRUE(CheckDuplicateRatio<Int16>(gec));

	for (Int16U i = 0; i < MaxInt16U_v; i += 1) {
		gec.push_back(Hasher<Int16U>()(i));
	}

	EXPECT_TRUE(CheckDuplicateRatio<Int16U>(gec));


	for (Int32 i = 0; i < MaxDataCount; i += 1) {
		gec.push_back(Hasher<Int32>()(i));
	}

	EXPECT_TRUE(CheckDuplicateRatio<Int32>(gec));

	for (Int32U i = 0; i < MaxDataCount; i += 1) {
		gec.push_back(Hasher<Int32U>()(i));
	}

	EXPECT_TRUE(CheckDuplicateRatio<Int32U>(gec));

}

TEST(HasherTest, SpecializedFloat) {
	for (float i = 0; i < 1.0f; i += 0.00001f) {
		gec.push_back(Hasher<float>()(i));
	}
	EXPECT_TRUE(CheckDuplicateRatio<float>(gec));
}

TEST(HasherTest, Double) {
	for (double i = 0; i < 10.0; i += 0.0001) {
		gec.push_back(Hasher<double>()(i));
	}
	EXPECT_TRUE(CheckDuplicateRatio<double>(gec));
}


TEST(HasherTest, String) {
	for (int i = 0; i < MaxDataCount / 100; i += 1) {
		gec.push_back(Hasher<String>()((stringstream() << i).str().c_str()));
	}
	EXPECT_TRUE(CheckDuplicateRatio<String>(gec));
}

#endif // TEST_HasherTest == ON


