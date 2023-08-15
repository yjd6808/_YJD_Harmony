/*
 * 작성자: 윤정도
 * 생성일: 8/15/2023 3:46:10 PM
 * =====================
 *
 */

#include <JCoreTest/CoreTest.h>
#include <JCore/Primitives/BitFlag.h>

#if TEST_BitFlagTest == ON

enum Flag {
	_1 = 0b00000000000000000000000000000001,
	_2 = 0b00000000000000000000000000000010,
	_3 = 0b00000000000000000000000000000100,
	_4 = 0b00000000000000000000000000001000,

	_12 = _1 | _2,
	_123 = _1 | _2 | _3,
	_1234 = _1 | _2 | _3 | _4
};

TEST(BitFlagTest, Regular) {
	BitFlag<Flag> s;
	s.Add(_1);
	EXPECT_TRUE(s.Check(_1));
	EXPECT_TRUE(s.Check(_2) == false);
	EXPECT_TRUE(s.Check(_2, _3) == false);

	s.Add(_1, _3);
	EXPECT_TRUE(s.Check(_1));
	EXPECT_TRUE(s.Check(_2) == false);
	EXPECT_TRUE(s.Check(_2, _3) == false);
	EXPECT_TRUE(s.Check(_1, _3));

	s.Set(_1, _2);
	EXPECT_TRUE(s.Check(_1, _3) == false);
	EXPECT_TRUE(s.Check(_1));
	EXPECT_TRUE(s.Check(_1, _2));
	EXPECT_TRUE(s.Check(_12));
	EXPECT_TRUE(s.Check(_3) == false);
}


#endif


