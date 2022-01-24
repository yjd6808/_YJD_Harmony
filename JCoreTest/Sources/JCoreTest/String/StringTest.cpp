/*
	작성자 : 윤정도
	String 클래스 테스트입니다.
*/


#include <JCoreTest/CoreTest.h>
#include <JCore/Core.h>
#include <JCore/String.h>

using namespace JCore;
using namespace std;

std::vector<int> g_vec;

#if TEST_StringTest == ON

// String::Compare 함수 테스트
TEST(StringTest, Compare) {
	String szStr1 = "abcd";
	String szStr2 = String::EMPTY;
	String szStr3 = "a";

	string szStr4 = "abcd";
	string szStr5 = "";
	string szStr6 = "a";

	// String 결과
	EXPECT_TRUE(szStr1 == "abcd");
	EXPECT_TRUE(szStr1 != "abc");
	EXPECT_TRUE(szStr2 == String::EMPTY);
	EXPECT_TRUE(szStr2 != "a");
	EXPECT_TRUE(szStr3 == "a");
	EXPECT_TRUE(szStr3 != String::EMPTY);
	EXPECT_TRUE(szStr1.Compare(szStr1) == 0);	// abcd와 abcd 자기 자신 비교
	EXPECT_TRUE(szStr1.Compare(szStr2) == 1);	// abcd와 "" 빈 문자열 비교는 abcd가 더 크다.
	EXPECT_TRUE(szStr1.Compare(szStr3) == 1);	// abcd와 a를 비교하면 abcd가 더 크다.

	// std::string의 결과
	EXPECT_TRUE(szStr4 == "abcd");
	EXPECT_TRUE(szStr4 != "abc");
	EXPECT_TRUE(szStr5 == String::EMPTY);
	EXPECT_TRUE(szStr5 != "a");
	EXPECT_TRUE(szStr6 == "a");
	EXPECT_TRUE(szStr6 != String::EMPTY);
	EXPECT_TRUE(szStr4.compare(szStr4) == 0);	// abcd와 abcd 자기 자신 비교
	EXPECT_TRUE(szStr4.compare(szStr5) == 1);	// abcd와 "" 빈 문자열 비교는 abcd가 더 크다.
	EXPECT_TRUE(szStr4.compare(szStr6) == 1);	// abcd와 a를 비교하면 abcd가 더 크다.
}


// String::Split 함수 테스트
TEST(StringTest, Split) {
	String szStr1 = "abcd_cd_efg";
	String szStr2 = "___";
	String szStr3 = "_bbbbb_";
	String szStr4 = "_bbbbb";
	String szStr5 = "_";

	vector<String> tokens1_1 = szStr1.Split("_", true);
	vector<String> tokens1_2 = szStr1.Split("_", false);

	vector<String> tokens2_1 = szStr2.Split("_", true);

	vector<String> tokens3_1 = szStr3.Split("_", true);
	vector<String> tokens3_2 = szStr3.Split("_", false);

	vector<String> tokens4_1 = szStr4.Split("_", true);

	vector<String> tokens5_1 = szStr5.Split("_", true);
	vector<String> tokens5_2 = szStr5.Split("_", false);
	

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


// String::Replace 함수 테스트
TEST(StringTest, Replace) {
	String szStr1 = "abcd_cd_efg";
	String szStr2 = "___";
	String szStr3 = "_bbbbb_";
	String szStr4 = "_";
	String szStr5 = "";

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


// String::Format 함수 테스트
TEST(StringTest, Format) {
	String szStr1;
	szStr1.Format("%d", 5123);
	EXPECT_TRUE(szStr1 == "5123");

	szStr1.Format("%d %d %d", 2000000000, 2000000000, 2000000000);
	EXPECT_TRUE(szStr1 == "2000000000 2000000000 2000000000");

	szStr1.Format("%.2f", 1.52f);
	EXPECT_TRUE(szStr1 == "1.52");
}


// String::operator 연산자 테스트
TEST(StringTest, AssignmentOperator) {
	String szStr1 = "abcd";
	String&& szStr2 = "ccc";

	// operator=(const char*) 호출
	szStr1 = "fredsgersgergesrgersfesvdfscdfscdssdfgds";
	EXPECT_TRUE(szStr1 == "fredsgersgergesrgersfesvdfscdfscdssdfgds");

	// operator=(const String&) 호출
	szStr1 = szStr2;
	EXPECT_TRUE(szStr1 == "ccc");

	// operator=(String&&) 호출
	szStr1 = std::move(szStr2);
	EXPECT_TRUE(szStr1 == "ccc");
	EXPECT_TRUE(szStr2.Source()  == nullptr);
}


// String::Find 테스트
TEST(StringTest, Find) {
	String szSource = "abcdefg"; // 0 ~ 6

	EXPECT_TRUE(szSource.Find("g") == 6);
	EXPECT_TRUE(szSource.Find("fg") == 5);
	EXPECT_TRUE(szSource.Find("efg") == 4);
	EXPECT_TRUE(szSource.Find("defg") == 3);
	EXPECT_TRUE(szSource.Find("cdefg") == 2);
	EXPECT_TRUE(szSource.Find("bcdefg") == 1);
	EXPECT_TRUE(szSource.Find("abcdefg") == 0);
	EXPECT_TRUE(szSource.Find("-abcdefg") == -1);
	EXPECT_TRUE(szSource.Find("abcdefg-") == -1);

	// 왼쪽에서 검사 테스트
	EXPECT_TRUE(szSource.Find("a") == 0);
	EXPECT_TRUE(szSource.Find("ab") == 0);
	EXPECT_TRUE(szSource.Find("abc") == 0);
	EXPECT_TRUE(szSource.Find("abcd") == 0);
	EXPECT_TRUE(szSource.Find("abcde") == 0);
	EXPECT_TRUE(szSource.Find("abcdef") == 0);
	EXPECT_TRUE(szSource.Find("abcdefg") == 0);
	EXPECT_TRUE(szSource.Find("abcdefg-") == -1);
	EXPECT_TRUE(szSource.Find("-abcdefg") == -1);

	// 범위 검사 테스트
	EXPECT_TRUE(szSource.Find(0, 0, "a") == 0);
	EXPECT_TRUE(szSource.Find(0, 1, "a") == 0);
	EXPECT_TRUE(szSource.Find(1, 1, "a") == -1);

	EXPECT_TRUE(szSource.Find(0, 2, "abc") == 0);
	EXPECT_TRUE(szSource.Find(1, 2, "abc") == -1);
	EXPECT_TRUE(szSource.Find(2, 2, "abc") == -1);

	EXPECT_TRUE(szSource.Find(4, 4, "e") == 4);
	EXPECT_TRUE(szSource.Find(4, 5, "ef") == 4);
	EXPECT_TRUE(szSource.Find(4, 6, "efg") == 4);

	EXPECT_TRUE(szSource.Find(0, 6, "abcdefg") == 0);
	EXPECT_TRUE(szSource.Find(1, 6, "abcdefg") == -1);
	EXPECT_TRUE(szSource.Find(0, 5, "abcdefg") == -1);
}



// String::FindReverse 테스트
TEST(StringTest, FindReverse) {
	String szSource = "abcdefg"; // 0 ~ 6

	EXPECT_TRUE(szSource.FindReverse("g") == 6);
	EXPECT_TRUE(szSource.FindReverse("fg") == 5);
	EXPECT_TRUE(szSource.FindReverse("efg") == 4);
	EXPECT_TRUE(szSource.FindReverse("defg") == 3);
	EXPECT_TRUE(szSource.FindReverse("cdefg") == 2);
	EXPECT_TRUE(szSource.FindReverse("bcdefg") == 1);
	EXPECT_TRUE(szSource.FindReverse("abcdefg") == 0);
	EXPECT_TRUE(szSource.FindReverse("-abcdefg") == -1);
	EXPECT_TRUE(szSource.FindReverse("abcdefg-") == -1);

	// 왼쪽에서 검사 테스트
	EXPECT_TRUE(szSource.FindReverse("a") == 0);
	EXPECT_TRUE(szSource.FindReverse("ab") == 0);
	EXPECT_TRUE(szSource.FindReverse("abc") == 0);
	EXPECT_TRUE(szSource.FindReverse("abcd") == 0);
	EXPECT_TRUE(szSource.FindReverse("abcde") == 0);
	EXPECT_TRUE(szSource.FindReverse("abcdef") == 0);
	EXPECT_TRUE(szSource.FindReverse("abcdefg") == 0);
	EXPECT_TRUE(szSource.FindReverse("abcdefg-") == -1);
	EXPECT_TRUE(szSource.FindReverse("-abcdefg") == -1);

	// 범위 검사 테스트
	EXPECT_TRUE(szSource.FindReverse(0, 0, "a") == 0);
	EXPECT_TRUE(szSource.FindReverse(0, 1, "a") == 0);
	EXPECT_TRUE(szSource.FindReverse(1, 1, "a") == -1);

	EXPECT_TRUE(szSource.FindReverse(0, 2, "abc") == 0);
	EXPECT_TRUE(szSource.FindReverse(1, 2, "abc") == -1);
	EXPECT_TRUE(szSource.FindReverse(2, 2, "abc") == -1);

	EXPECT_TRUE(szSource.FindReverse(4, 4, "e") == 4);
	EXPECT_TRUE(szSource.FindReverse(4, 5, "ef") == 4);
	EXPECT_TRUE(szSource.FindReverse(4, 6, "efg") == 4);

	EXPECT_TRUE(szSource.FindReverse(0, 6, "abcdefg") == 0);
	EXPECT_TRUE(szSource.FindReverse(1, 6, "abcdefg") == -1);
	EXPECT_TRUE(szSource.FindReverse(0, 5, "abcdefg") == -1);

}


#endif // TEST_StringTest == ON


