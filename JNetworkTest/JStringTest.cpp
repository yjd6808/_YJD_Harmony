/*
	�ۼ��� : ������
	JString Ŭ���� �׽�Ʈ�Դϴ�.
*/


#include "JPCH.h"

#include <JString.h>

std::vector<int> g_vec;

// JString::Compare �Լ� �׽�Ʈ
TEST(JStringTest, Compare) {
	JString szStr1 = "abcd";
	JString szStr2 = JString::EMPTY;
	JString szStr3 = "a";

	string szStr4 = "abcd";
	string szStr5 = "";
	string szStr6 = "a";

	// JString ���
	EXPECT_TRUE(szStr1 == "abcd");
	EXPECT_TRUE(szStr1 != "abc");
	EXPECT_TRUE(szStr2 == JString::EMPTY);
	EXPECT_TRUE(szStr2 != "a");
	EXPECT_TRUE(szStr3 == "a");
	EXPECT_TRUE(szStr3 != JString::EMPTY);
	EXPECT_TRUE(szStr1.Compare(szStr1) == 0);	// abcd�� abcd �ڱ� �ڽ� ��
	EXPECT_TRUE(szStr1.Compare(szStr2) == 1);	// abcd�� "" �� ���ڿ� �񱳴� abcd�� �� ũ��.
	EXPECT_TRUE(szStr1.Compare(szStr3) == 1);	// abcd�� a�� ���ϸ� abcd�� �� ũ��.

	// std::string�� ���
	EXPECT_TRUE(szStr4 == "abcd");
	EXPECT_TRUE(szStr4 != "abc");
	EXPECT_TRUE(szStr5 == JString::EMPTY);
	EXPECT_TRUE(szStr5 != "a");
	EXPECT_TRUE(szStr6 == "a");
	EXPECT_TRUE(szStr6 != JString::EMPTY);
	EXPECT_TRUE(szStr4.compare(szStr4) == 0);	// abcd�� abcd �ڱ� �ڽ� ��
	EXPECT_TRUE(szStr4.compare(szStr5) == 1);	// abcd�� "" �� ���ڿ� �񱳴� abcd�� �� ũ��.
	EXPECT_TRUE(szStr4.compare(szStr6) == 1);	// abcd�� a�� ���ϸ� abcd�� �� ũ��.
}


// JString::Split �Լ� �׽�Ʈ
TEST(JStringTest, Split) {
	JString szStr1 = "abcd_cd_efg";
	JString szStr2 = "___";
	JString szStr3 = "_bbbbb_";
	JString szStr4 = "_bbbbb";
	JString szStr5 = "_";

	vector<JString> tokens1_1 = szStr1.Split("_", true);
	vector<JString> tokens1_2 = szStr1.Split("_", false);

	vector<JString> tokens2_1 = szStr2.Split("_", true);

	vector<JString> tokens3_1 = szStr3.Split("_", true);
	vector<JString> tokens3_2 = szStr3.Split("_", false);

	vector<JString> tokens4_1 = szStr4.Split("_", true);

	vector<JString> tokens5_1 = szStr5.Split("_", true);
	vector<JString> tokens5_2 = szStr5.Split("_", false);
	

	EXPECT_TRUE(tokens1_1.size() == 3);
	if (tokens1_1.size() == 3) {
		EXPECT_TRUE(tokens1_1[0] == "abcd");
		EXPECT_TRUE(tokens1_1[1] == "cd");
		EXPECT_TRUE(tokens1_1[2] == "efg");
	}
	
	EXPECT_TRUE(tokens1_2.size() == 3);
	if (tokens1_2.size() == 3) {
		EXPECT_TRUE(tokens1_2[0] == "abcd");
		EXPECT_TRUE(tokens1_2[1] == "cd");
		EXPECT_TRUE(tokens1_2[2] == "efg");
	}
	

	EXPECT_TRUE(tokens2_1.size() == 4);
	if (tokens2_1.size() == 4) {
		EXPECT_TRUE(tokens2_1[0] == "");
		EXPECT_TRUE(tokens2_1[1] == "");
		EXPECT_TRUE(tokens2_1[2] == "");
		EXPECT_TRUE(tokens2_1[3] == "");
	}

	EXPECT_TRUE(tokens3_1.size() == 3);
	if (tokens3_1.size() == 3) {
		EXPECT_TRUE(tokens3_1[0] == "");
		EXPECT_TRUE(tokens3_1[1] == "bbbbb");
		EXPECT_TRUE(tokens3_1[2] == "");
	}
	

	EXPECT_TRUE(tokens4_1.size() == 2);
	if (tokens4_1.size() == 2) {
		EXPECT_TRUE(tokens4_1[0] == "");
		EXPECT_TRUE(tokens4_1[1] == "bbbbb");
	}
	

	EXPECT_TRUE(tokens5_1.size() == 2);
	if (tokens5_1.size() == 2) {
		EXPECT_TRUE(tokens5_1[0] == "");
		EXPECT_TRUE(tokens5_1[1] == "");
	}

	EXPECT_TRUE(tokens5_2.size() == 0);
}


// JString::Replace �Լ� �׽�Ʈ
TEST(JStringTest, Replace) {
	JString szStr1 = "abcd_cd_efg";
	JString szStr2 = "___";
	JString szStr3 = "_bbbbb_";
	JString szStr4 = "_";
	JString szStr5 = "";

	szStr1.Replace("_", "@@");
	szStr2.Replace("_", "@@");
	szStr3.Replace("_", "@@");
	szStr4.Replace("_", "@@");
	szStr5.Replace("", "abcdef");

	EXPECT_TRUE(szStr1 == "abcd@@cd@@efg");
	EXPECT_TRUE(szStr2 == "@@@@@@");
	EXPECT_TRUE(szStr3 == "@@bbbbb@@");
	EXPECT_TRUE(szStr4 == "@@");
	EXPECT_TRUE(szStr5 == "abcdef");	
}


// JString::Format �Լ� �׽�Ʈ
TEST(JStringTest, Format) {
	JString szStr1;
	szStr1.Format("%d", 5123);
	EXPECT_TRUE(szStr1 == "5123");

	szStr1.Format("%d %d %d", 2000000000, 2000000000, 2000000000);
	EXPECT_TRUE(szStr1 == "2000000000 2000000000 2000000000");

	szStr1.Format("%.2f", 1.52f);
	EXPECT_TRUE(szStr1 == "1.52");
}


// JString::operator ������ �׽�Ʈ
TEST(JStringTest, AssignmentOperator) {
	JString szStr1 = "abcd";
	JString&& szStr2 = "ccc";

	// operator=(const char*) ȣ��
	szStr1 = "fredsgersgergesrgersfesvdfscdfscdssdfgds";
	EXPECT_TRUE(szStr1 == "fredsgersgergesrgersfesvdfscdfscdssdfgds");

	// operator=(const JString&) ȣ��
	szStr1 = szStr2;
	EXPECT_TRUE(szStr1 == "ccc");

	// operator=(JString&&) ȣ��
	szStr1 = std::move(szStr2);
	EXPECT_TRUE(szStr1 == "ccc");
	EXPECT_TRUE(szStr2.Source()  == nullptr);
}