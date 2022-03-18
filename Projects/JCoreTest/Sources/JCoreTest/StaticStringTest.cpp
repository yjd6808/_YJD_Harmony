/*
	�ۼ��� : ������
	StaticStringTest �Դϴ�.
*/


#include <JCoreTest/CoreTest.h>
#include <JCore/Core.h>
#include <JCore/String.h>
#include <JCore/StaticString.h>

using namespace JCore;
using namespace std;

#if TEST_StaticStringTest == ON

TEST(StaticStringTest, Operator) {
	constexpr StaticString<6> szStr1{ "abcd" };
	constexpr StaticString<6> szStr2{ "" };
	constexpr StaticString<6> szStr3{ "a" };

	// String ���
	EXPECT_TRUE(szStr1 == "abcd");
	EXPECT_TRUE(szStr1 != "abc");
	EXPECT_TRUE(szStr2 == String::EMPTY);
	EXPECT_TRUE(szStr2 != "a");
	EXPECT_TRUE(szStr3 == "a");
	EXPECT_TRUE(szStr3 != String::EMPTY);
	EXPECT_TRUE(szStr1.Compare(szStr1) == 0);	// abcd�� abcd �ڱ� �ڽ� ��
	EXPECT_TRUE(szStr1.Compare(szStr2) == 1);	// abcd�� "" �� ���ڿ� �񱳴� abcd�� �� ũ��.
	EXPECT_TRUE(szStr1.Compare(szStr3) == 1);	// abcd�� a�� ���ϸ� abcd�� �� ũ��.

	szStr1[0] = 'b';
	EXPECT_TRUE(szStr1[0] == 'b');
	EXPECT_TRUE(szStr1[1] == 'b');
	EXPECT_TRUE(szStr1[2] == 'c');
	EXPECT_TRUE(szStr1[3] == 'd');
	EXPECT_TRUE(szStr1[4] == '\0');

}

TEST(StaticStringTest, CopyFrom) {
	constexpr StaticString<7> szStr1{ "abcdef" };

	szStr1.CopyFrom("");
	EXPECT_TRUE(szStr1 == "");

	szStr1.CopyFrom("k");
	EXPECT_TRUE(szStr1 == "k");

	szStr1.CopyFrom("bb");
	EXPECT_TRUE(szStr1 == "bb");

	szStr1.CopyFrom("ccc");
	EXPECT_TRUE(szStr1 == "ccc");

	szStr1.CopyFrom("dddd");
	EXPECT_TRUE(szStr1 == "dddd");

	szStr1.CopyFrom("eeeee");
	EXPECT_TRUE(szStr1 == "eeeee");

	szStr1.CopyFrom("ffffff");
	EXPECT_TRUE(szStr1 == "ffffff");

	szStr1.CopyFrom("qqqqqqq");
	EXPECT_TRUE(szStr1 == "qqqqqq");	// ũ�Ⱑ 7�̱� ������ 6���� �����

	szStr1.CopyFrom("rrrrrrrr");
	EXPECT_TRUE(szStr1 == "rrrrrr");	// ũ�Ⱑ 7�̱� ������ 6���� �����

	// ============================

	StaticString<7> szStr2{ "abcdef" };

	szStr2.CopyFrom("");
	EXPECT_TRUE(szStr2 == "");

	szStr2.CopyFrom("k");
	EXPECT_TRUE(szStr2 == "k");

	szStr2.CopyFrom("bb");
	EXPECT_TRUE(szStr2 == "bb");

	szStr2.CopyFrom("ccc");
	EXPECT_TRUE(szStr2 == "ccc");

	szStr2.CopyFrom("dddd");
	EXPECT_TRUE(szStr2 == "dddd");

	szStr2.CopyFrom("eeeee");
	EXPECT_TRUE(szStr2 == "eeeee");

	szStr2.CopyFrom("ffffff");
	EXPECT_TRUE(szStr2 == "ffffff");

	szStr2.CopyFrom("qqqqqqq");
	EXPECT_TRUE(szStr2 == "qqqqqq");	// ũ�Ⱑ 7�̱� ������ 6���� �����

	szStr2.CopyFrom("rrrrrrrr");
	EXPECT_TRUE(szStr2 == "rrrrrr");	// ũ�Ⱑ 7�̱� ������ 6���� �����
}

TEST(StaticStringTest, StartWith) {
	constexpr StaticString<7> szStr1{ "abcdef" };

	EXPECT_TRUE(szStr1.StartWith(""));
	EXPECT_TRUE(szStr1.StartWith("a"));
	EXPECT_TRUE(szStr1.StartWith("ab"));
	EXPECT_TRUE(szStr1.StartWith("abc"));
	EXPECT_TRUE(szStr1.StartWith("abcd"));
	EXPECT_TRUE(szStr1.StartWith("abcde"));
	EXPECT_TRUE(szStr1.StartWith("abcdef"));
	
	// ��ã�� ���
	EXPECT_FALSE(szStr1.StartWith("c"));
	EXPECT_FALSE(szStr1.StartWith("cab"));
	EXPECT_FALSE(szStr1.StartWith("abcdefg"));
}

TEST(StaticStringTest, Contain) {
	constexpr StaticString<7> szStr1{ "abcd" };
	EXPECT_TRUE(szStr1.Contain("a"));
	EXPECT_TRUE(szStr1.Contain("b"));
	EXPECT_TRUE(szStr1.Contain("c"));
	EXPECT_TRUE(szStr1.Contain("d"));
	EXPECT_TRUE(szStr1.Contain(""));

	EXPECT_TRUE(szStr1.Contain("ab"));
	EXPECT_TRUE(szStr1.Contain("bc"));
	EXPECT_TRUE(szStr1.Contain("cd"));

	EXPECT_TRUE(szStr1.Contain("abc"));
	EXPECT_TRUE(szStr1.Contain("bcd"));

	EXPECT_TRUE(szStr1.Contain("abcd"));

	// ��ã�� ���
	EXPECT_FALSE(szStr1.Contain(" abcd"));
	EXPECT_FALSE(szStr1.Contain("abcd "));
	EXPECT_FALSE(szStr1.Contain("ab "));
	EXPECT_FALSE(szStr1.Contain(" ab"));
}

TEST(StaticStringTest, EndWith) {
	constexpr StaticString<7> szStr1{ "abcdef" };

	EXPECT_TRUE(szStr1.EndWith(""));
	EXPECT_TRUE(szStr1.EndWith("f"));
	EXPECT_TRUE(szStr1.EndWith("ef"));
	EXPECT_TRUE(szStr1.EndWith("def"));
	EXPECT_TRUE(szStr1.EndWith("cdef"));
	EXPECT_TRUE(szStr1.EndWith("bcdef"));
	EXPECT_TRUE(szStr1.EndWith("abcdef"));

	// ��ã�� ���
	EXPECT_FALSE(szStr1.EndWith("g"));
	EXPECT_FALSE(szStr1.EndWith("eef"));
	EXPECT_FALSE(szStr1.EndWith("0abcdef"));
}

TEST(StaticStringTest, Remove) {
	constexpr StaticString<7> szStr1{ "abcd" };

	EXPECT_TRUE(szStr1.Remove("") == "abcd");

	EXPECT_TRUE(szStr1.Remove("a") == "bcd");
	EXPECT_TRUE(szStr1.Remove("b") == "acd");
	EXPECT_TRUE(szStr1.Remove("c") == "abd");
	EXPECT_TRUE(szStr1.Remove("d") == "abc");

	EXPECT_TRUE(szStr1.Remove("ab") == "cd");
	EXPECT_TRUE(szStr1.Remove("bc") == "ad");
	EXPECT_TRUE(szStr1.Remove("cd") == "ab");

	EXPECT_TRUE(szStr1.Remove("abc") == "d");
	EXPECT_TRUE(szStr1.Remove("bcd") == "a");

	EXPECT_TRUE(szStr1.Remove("abcd") == "");
}

#endif
