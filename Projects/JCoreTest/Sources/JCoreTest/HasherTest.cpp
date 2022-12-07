/*
	작성자 : 윤정도
	해쉬 생성기 테스트
*/


#include <JCoreTest/CoreTest.h>
#include <JCore/Random.h>
#include <JCore/Hasher.h>


using namespace std;

#if TEST_HasherTest == ON

constexpr int MaxDataCount_v = 1000000;// 최대로 테스트할 데이터 수
constexpr float SuccessRatio_v = 50.0;	 // 중복률 통과 율
vector<Int32U> g_vec;
Random g_rand;

template <typename T>
bool CheckDuplicateRatio(vector<Int32U>& vec) {
	int iOriginalSize = vec.size();
	vec.erase(std::unique(vec.begin(), vec.end()), vec.end());

	double dupRatio = (double)(iOriginalSize - vec.size()) / iOriginalSize * 100.0;
	PrintFormat("(%s)중복 발생률 : %.2f%%(%d개중 %d개가 중복된 데이터로 삭제됨)\n",
		Type<T>().Source(),
		dupRatio, 
		iOriginalSize, 
		iOriginalSize - vec.size());

	vec.clear();

	return dupRatio < SuccessRatio_v;
}


TEST(HasherTest, Integer) {
	for (Int8 i = MinInt8_v; i < MaxInt8_v; i += 1) {
		g_vec.push_back(Hasher<Int8>()(i));
	}

	EXPECT_TRUE(CheckDuplicateRatio<Int8>(g_vec));

	for (Int8U i = 0; i < MaxInt8U_v; i += 1) {
		g_vec.push_back(Hasher<Int8U>()(i));
	}

	EXPECT_TRUE(CheckDuplicateRatio<Int8U>(g_vec));

	for (Int16 i = MinInt16_v; i < MaxInt16_v; i += 1) {
		g_vec.push_back(Hasher<Int16>()(i));
	}
	
	EXPECT_TRUE(CheckDuplicateRatio<Int16>(g_vec));

	for (Int16U i = 0; i < MaxInt16U_v; i += 1) {
		g_vec.push_back(Hasher<Int16U>()(i));
	}

	EXPECT_TRUE(CheckDuplicateRatio<Int16U>(g_vec));


	for (Int32 i = 0; i < MaxDataCount_v; i += 1) {
		g_vec.push_back(Hasher<Int32>()(i));
	}

	EXPECT_TRUE(CheckDuplicateRatio<Int32>(g_vec));

	for (Int32U i = 0; i < MaxDataCount_v; i += 1) {
		g_vec.push_back(Hasher<Int32U>()(i));
	}

	EXPECT_TRUE(CheckDuplicateRatio<Int32U>(g_vec));

}

TEST(HasherTest, SpecializedFloat) {
	for (float i = 0; i < 1.0f; i += 0.00001f) {
		g_vec.push_back(Hasher<float>()(i));
	}
	EXPECT_TRUE(CheckDuplicateRatio<float>(g_vec));
}

TEST(HasherTest, Double) {
	for (double i = 0; i < 10.0; i += 0.0001) {
		g_vec.push_back(Hasher<double>()(i));
	}
	EXPECT_TRUE(CheckDuplicateRatio<double>(g_vec));
}


TEST(HasherTest, String) {
	for (int i = 0; i < MaxDataCount_v / 100; i += 1) {
		g_vec.push_back(Hasher<String>()((stringstream() << i).str().c_str()));
	}
	EXPECT_TRUE(CheckDuplicateRatio<String>(g_vec));
}

#endif // TEST_HasherTest == ON


