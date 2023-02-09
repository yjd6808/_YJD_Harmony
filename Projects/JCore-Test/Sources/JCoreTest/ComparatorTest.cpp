/*
	작성자 : 윤정도
	Comparator 펑터 테스트
*/


#include <JCoreTest/CoreTest.h>
#include <JCore/Primitives/String.h>
#include <JCore/Comparator.h>


using namespace std;

#if TEST_ComparatorTest == ON

TEST(ComparatorTest, ComparatorTest) {
	EXPECT_TRUE(Comparator<String>()("abcd", "abc") > 0);
	EXPECT_TRUE(Comparator<String>()("abc", "abc") == 0);
	EXPECT_TRUE(Comparator<String>()("abc", "abcd") < 0);

	EXPECT_TRUE(Comparator<const char*>()("abcd", "abc") > 0);
	EXPECT_TRUE(Comparator<const char*>()("abc", "abc") == 0);
	EXPECT_TRUE(Comparator<const char*>()("abc", "abcd") < 0);

	EXPECT_TRUE(Comparator<int>()(1, 2) < 0);
	EXPECT_TRUE(Comparator<int>()(2, 1) > 0);
	EXPECT_TRUE(Comparator<int>()(2, 2) == 0);
}

#endif // TEST_ComparatorTest == ON


