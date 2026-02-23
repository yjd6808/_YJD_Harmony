/*
	작성자 : 윤정도
	해쉬 생성기 테스트
*/


#include <jc_gtest/Core.h>
#include <jc/Random.h>
#include <jc/Hasher.h>


using namespace std;

#if TEST_HasherTest == ON

constexpr int MaxDataCount = 1000000;// 최대로 테스트할 데이터 수
constexpr float SuccessRatio = 50.0;	 // 중복률 통과 율
vector<_u32> gec;
Random g_rand;

template <typename T>
bool CheckDuplicateRatio(vector<_u32>& vec) {
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
	for (_s8 i = MinInt8_v; i < MaxInt8_v; i += 1) {
		gec.push_back(Hasher<_s8>()(i));
	}

	EXPECT_TRUE(CheckDuplicateRatio<_s8>(gec));

	for (_u8 i = 0; i < MaxInt8U_v; i += 1) {
		gec.push_back(Hasher<_u8>()(i));
	}

	EXPECT_TRUE(CheckDuplicateRatio<_u8>(gec));

	for (_s16 i = MinInt16_v; i < MaxInt16_v; i += 1) {
		gec.push_back(Hasher<_s16>()(i));
	}
	
	EXPECT_TRUE(CheckDuplicateRatio<_s16>(gec));

	for (_u16 i = 0; i < MaxInt16U_v; i += 1) {
		gec.push_back(Hasher<_u16>()(i));
	}

	EXPECT_TRUE(CheckDuplicateRatio<_u16>(gec));


	for (_s32 i = 0; i < MaxDataCount; i += 1) {
		gec.push_back(Hasher<_s32>()(i));
	}

	EXPECT_TRUE(CheckDuplicateRatio<_s32>(gec));

	for (_u32 i = 0; i < MaxDataCount; i += 1) {
		gec.push_back(Hasher<_u32>()(i));
	}

	EXPECT_TRUE(CheckDuplicateRatio<_u32>(gec));

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


