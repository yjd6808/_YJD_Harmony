/*
	작성자 : 윤정도
	테스트
*/

#include <JCoreTest/CoreTest.h>
#include <JCore/Math.h>

#if TEST_MathTest == ON


using namespace std;

TEST(MathTest, Pow) {
	EXPECT_TRUE(Math::Pow(1, 0) == (int)powl(1, 0));
	EXPECT_TRUE(Math::Pow(1, 1) == (int)powl(1, 1));
	EXPECT_TRUE(Math::Pow(1, 2) == (int)powl(1, 2));
	EXPECT_TRUE(Math::Pow(1, 3) == (int)powl(1, 3));


	EXPECT_TRUE(Math::Pow(2, 0) == (int)powl(2, 0));
	EXPECT_TRUE(Math::Pow(2, 1) == (int)powl(2, 1));
	EXPECT_TRUE(Math::Pow(2, 2) == (int)powl(2, 2));
	EXPECT_TRUE(Math::Pow(2, 3) == (int)powl(2, 3));

	EXPECT_TRUE(Math::Pow(3, 0) == (int)powl(3, 0));
	EXPECT_TRUE(Math::Pow(3, 1) == (int)powl(3, 1));
	EXPECT_TRUE(Math::Pow(3, 2) == (int)powl(3, 2));
	EXPECT_TRUE(Math::Pow(3, 3) == (int)powl(3, 3));
}

TEST(MathTest, Min) {
	EXPECT_TRUE(Math::Min(1, 2) == 1);
	EXPECT_TRUE(Math::Min(2, 1) == 1);
	EXPECT_TRUE(Math::Min(1, 2, 3) == 1);
	EXPECT_TRUE(Math::Min(1, 3, 2) == 1);
	EXPECT_TRUE(Math::Min(3, 2, 2) == 2);
	EXPECT_TRUE(Math::Min(1, 2, 3, 4) == 1);
	EXPECT_TRUE(Math::Min(1, 2, 4, 3) == 1);
	EXPECT_TRUE(Math::Min(1, 4, 2, 1) == 1);
	EXPECT_TRUE(Math::Min(4, 0, 0, 0) == 0);
	EXPECT_TRUE(Math::Min(5, 0, 0, 0, 0) == 0);
	EXPECT_TRUE(Math::Min(0, 5, 0, 0, 0) == 0);
	EXPECT_TRUE(Math::Min(0, 5, 6, 0, 0) == 0);
	EXPECT_TRUE(Math::Min(0, 5, 6, 7, 0) == 0);
	EXPECT_TRUE(Math::Min(0, 5, 6, 7, 8) == 0);
	EXPECT_TRUE(Math::Min(1, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, -5000) == -5000);
	EXPECT_TRUE(Math::Min(1, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 5000) == 1);
}

TEST(MathTest, Max) {

	EXPECT_TRUE(Math::Max(1, 2) == 2);
	EXPECT_TRUE(Math::Max(2, 1) == 2);
	EXPECT_TRUE(Math::Max(1, 2, 3) == 3);
	EXPECT_TRUE(Math::Max(1, 3, 2) == 3);
	EXPECT_TRUE(Math::Max(3, 2, 2) == 3);
	EXPECT_TRUE(Math::Max(1, 2, 3, 4) == 4);
	EXPECT_TRUE(Math::Max(1, 2, 4, 3) == 4);
	EXPECT_TRUE(Math::Max(1, 4, 2, 1) == 4);
	EXPECT_TRUE(Math::Max(4, 0, 0, 0) == 4);
	EXPECT_TRUE(Math::Max(5, 0, 0, 0, 0) == 5);
	EXPECT_TRUE(Math::Max(0, 5, 0, 0, 0) == 5);
	EXPECT_TRUE(Math::Max(0, 5, 6, 0, 0) == 6);
	EXPECT_TRUE(Math::Max(0, 5, 6, 7, 0) == 7);
	EXPECT_TRUE(Math::Max(0, 5, 6, 7, 8) == 8);
	EXPECT_TRUE(Math::Max(1, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, -5000) == 10);
	EXPECT_TRUE(Math::Max(1, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 5000) == 5000);
}

#endif // TEST_MathTest == ON