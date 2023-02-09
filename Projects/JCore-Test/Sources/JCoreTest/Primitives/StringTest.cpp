/*
	작성자 : 윤정도
	String 클래스 테스트입니다.
*/


#include <JCoreTest/CoreTest.h>
#include <JCore/Core.h>
#include <JCore/Primitives/String.h>
#include <JCore/Container/Vector.h>


using namespace std;

static Vector<int> gec;

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


// String::operator> < >= <= 함수 테스트
TEST(StringTest, ComparisonOperator) {
	const String str1 = "abcd";
	const String str2 = "abc";
	const String str3 = "ab";

	EXPECT_FALSE(str2 > str2);
	EXPECT_TRUE(str2 >= str2);
	EXPECT_TRUE(str1 >= str2);
	EXPECT_TRUE(str1 > str2);
	EXPECT_TRUE(str3 < str2);
	EXPECT_TRUE(str3 <= str2);
}


// String::Split 함수 테스트
TEST(StringTest, Split) {
	String szStr1 = "abcd_cd_efg";
	String szStr2 = "___";
	String szStr3 = "_bbbbb_";
	String szStr4 = "_bbbbb";
	String szStr5 = "_";

	

	{
		LeakCheck;
		Vector<String> tokens1_1 = szStr1.Split("_", true);
		EXPECT_TRUE(tokens1_1.Size() == 3);
		if (tokens1_1.Size() == 3) {
			EXPECT_TRUE(tokens1_1[0] == "abcd");
			EXPECT_TRUE(tokens1_1[1] == "cd");
			EXPECT_TRUE(tokens1_1[2] == "efg");
		}
	}

	{
		LeakCheck;
		Vector<String> tokens1_2 = szStr1.Split("_", false);
		EXPECT_TRUE(tokens1_2.Size() == 3);
		if (tokens1_2.Size() == 3) {
			EXPECT_TRUE(tokens1_2[0] == "abcd");
			EXPECT_TRUE(tokens1_2[1] == "cd");
			EXPECT_TRUE(tokens1_2[2] == "efg");
		}
	}
	

	{
		LeakCheck;
		Vector<String> tokens2_1 = szStr2.Split("_", true);
		EXPECT_TRUE(tokens2_1.Size() == 4);
		if (tokens2_1.Size() == 4) {
			EXPECT_TRUE(tokens2_1[0] == "");
			EXPECT_TRUE(tokens2_1[1] == "");
			EXPECT_TRUE(tokens2_1[2] == "");
			EXPECT_TRUE(tokens2_1[3] == "");
		}
	}
	

	{
		LeakCheck;
		Vector<String> tokens3_1 = szStr3.Split("_", true);
		EXPECT_TRUE(tokens3_1.Size() == 3);
		if (tokens3_1.Size() == 3) {
			EXPECT_TRUE(tokens3_1[0] == "");
			EXPECT_TRUE(tokens3_1[1] == "bbbbb");
			EXPECT_TRUE(tokens3_1[2] == "");
		}
	}

	{
		LeakCheck;
		Vector<String> tokens3_2 = szStr3.Split("_", false);
	}

	{
		LeakCheck;
		Vector<String> tokens4_1 = szStr4.Split("_", true);

		EXPECT_TRUE(tokens4_1.Size() == 2);
		if (tokens4_1.Size() == 2) {
			EXPECT_TRUE(tokens4_1[0] == "");
			EXPECT_TRUE(tokens4_1[1] == "bbbbb");
		}
	}

	{
		LeakCheck;
		Vector<String> tokens5_1 = szStr5.Split("_", true);
		EXPECT_TRUE(tokens5_1.Size() == 2);
		if (tokens5_1.Size() == 2) {
			EXPECT_TRUE(tokens5_1[0] == "");
			EXPECT_TRUE(tokens5_1[1] == "");
		}

	}

	{
		LeakCheck;
		Vector<String> tokens5_2 = szStr5.Split("_", false);
		EXPECT_TRUE(tokens5_2.Size() == 0);
	}
	
	

	
	
	

	

	
	

	

	
}


// String::ReplaceAll 함수 테스트
TEST(StringTest, ReplaceAll) {

	// ReplaceAllDifferentLen 테스트
	String szStr1 = "abcd_cd_efg";
	String szStr2 = "___";
	String szStr3 = "_bbbbb_";
	String szStr4 = "_";
	String szStr5 = "";

	szStr1.ReplaceAll("_", "@@");
	szStr2.ReplaceAll("_", "@@");
	szStr3.ReplaceAll("_", "@@");
	szStr4.ReplaceAll("_", "@@");
	szStr5.ReplaceAll("", "abcdef");

	EXPECT_TRUE(szStr1 == "abcd@@cd@@efg");
	EXPECT_TRUE(szStr2 == "@@@@@@");
	EXPECT_TRUE(szStr3 == "@@bbbbb@@");
	EXPECT_TRUE(szStr4 == "@@");
	EXPECT_TRUE(szStr5 == "abcdef");	


	// ReplaceAllEqualLen 테스트
	String szStr6 = "한글__사랑__한글__동물";
	String szStr7 = "a_a_b_c";
	String szStr8 = "aa_aaa";

	szStr6.ReplaceAll("한글", "국가");
	szStr7.ReplaceAll("a", "z");
	szStr8.ReplaceAll("aa", "kk");


	EXPECT_TRUE(szStr6 == "국가__사랑__국가__동물");
	EXPECT_TRUE(szStr7 == "z_z_b_c");
	EXPECT_TRUE(szStr8 == "kk_kka");
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

TEST(StringTest, Count) {
	String szSource = " ";
	EXPECT_TRUE(szSource.Count(" ") == 1);
	EXPECT_TRUE(szSource.Count("a") == 0);

	// 은근슬쩍 operator=T 테스트
	szSource = 123454321;
	EXPECT_TRUE(szSource.Count("1") == 2);
	EXPECT_TRUE(szSource.Count("2") == 2);
	EXPECT_TRUE(szSource.Count("3") == 2);
	EXPECT_TRUE(szSource.Count("4") == 2);
	EXPECT_TRUE(szSource.Count("5") == 1);

	szSource = "aabbcc";
	EXPECT_TRUE(szSource.Count("a") == 2);
	EXPECT_TRUE(szSource.Count("b") == 2);
	EXPECT_TRUE(szSource.Count("c") == 2);

	szSource = "a";
	EXPECT_TRUE(szSource.Count("a") == 1);
}

TEST(StringTest, Insert) {
	String szSource("", 1);
	szSource.Insert(0, "a");
	EXPECT_TRUE(szSource == "a");

	szSource.Insert(0, "b");
	EXPECT_TRUE(szSource == "ba");

	szSource.Insert(0, "c");
	EXPECT_TRUE(szSource == "cba");

	szSource.Insert(3, "b");
	EXPECT_TRUE(szSource == "cbab");
	szSource.Insert(4, "c");
	EXPECT_TRUE(szSource == "cbabc");

	szSource.Insert(1, "zz");
	EXPECT_TRUE(szSource == "czzbabc");
}


TEST(StringTest, Replace) {
	LeakCheck;
	String szSource = "a";
	// 빈 문자열로 바꾸는 경우
	int ret = szSource.Replace("a", "");
	EXPECT_TRUE(szSource == "");
	EXPECT_TRUE(ret == -1);	// 마지막 위치에 도달해서 -1을 반환


	// 맨 앞에서 긴 문자열로 바꾸는 경우
	szSource = String("aabcdd", 7);
	ret = szSource.Replace("aa", "zzzz");
	EXPECT_TRUE(szSource == "zzzzbcdd");
	EXPECT_TRUE(ret == 4);

	// 맨뒤에서 긴 문자열로 바꾸는 경우
	ret = szSource.Replace("dd", "zzzz");
	EXPECT_TRUE(szSource == "zzzzbczzzz");
	EXPECT_TRUE(ret == -1);	// 마지막 위치에 도달해서 -1 반환

	// 중간에서 긴 문자열로 바꾸는 경우
	ret = szSource.Replace("bc", "zzzz");
	EXPECT_TRUE(szSource == "zzzzzzzzzzzz");
	EXPECT_TRUE(ret == 8);

	// 확장 잘 되는지 확인
	szSource = String("a", 2);
	szSource.Replace("a", "zzzzzz");
	EXPECT_TRUE(szSource == "zzzzzz");

	// 맨 앞에서 같은 길이로 치환
	szSource = "aabbcc";
	szSource.Replace("aa", "gg");
	EXPECT_TRUE(szSource == "ggbbcc");

	// 중간에서 같은 길이로 치환
	szSource.Replace("bb", "gg");
	EXPECT_TRUE(szSource == "ggggcc");

	// 뒤에서 같은 길이로 치환
	szSource.Replace("cc", "gg");
	EXPECT_TRUE(szSource == "gggggg");

	// 맨 앞에서 짧은 길이로 치환
	szSource = "aabbcc";
	szSource.Replace("aa", "a");
	EXPECT_TRUE(szSource == "abbcc");

	// 중간에서 짧은 길이로 치환
	szSource.Replace("bb", "b");
	EXPECT_TRUE(szSource == "abcc");

	// 맨뒤에서 짧은 길이로 치환
	szSource.Replace("cc", "c");
	EXPECT_TRUE(szSource == "abc");


	// 반복 Replace 테스트
	int iReplaceOffset = 0;
	int iReplaceCount = 0;
	szSource = "aaaaaaaa";

	while ((iReplaceOffset = szSource.Replace(iReplaceOffset, "a", "b")) != -1) {
		iReplaceCount++;
	}
	EXPECT_TRUE(szSource == "bbbbbbbb");
	EXPECT_TRUE(iReplaceCount == 7);	// 한번은 카운트 안되서 7로 뜸


	szSource = "aaaaaaaa";
	iReplaceOffset = 0;
	iReplaceCount = 0;

	// 길게 반복 Replace
	while ((iReplaceOffset = szSource.Replace(iReplaceOffset, "a", "cc")) != -1) {
		iReplaceCount++;
	}

	EXPECT_TRUE(szSource == "cccccccccccccccc");
	EXPECT_TRUE(iReplaceCount == 7);	// 한번은 카운트 안되서 7로 뜸

	iReplaceOffset = 0;
	iReplaceCount = 0;

	// 짧게 반복 Replace
	while ((iReplaceOffset = szSource.Replace(iReplaceOffset, "cc", "a")) != -1) {
		iReplaceCount++;
	}

	EXPECT_TRUE(szSource == "aaaaaaaa");
	EXPECT_TRUE(iReplaceCount == 7);	// 한번은 카운트 안되서 7로 뜸
}

TEST(StringTest, Clear) {
	String szSource = "abcd";
	szSource.Clear();
	EXPECT_TRUE(szSource == "");

	szSource = "abcd";
	szSource.Clear(1, 2);
	EXPECT_TRUE(szSource == "ad");

	szSource = "abcd";
	szSource.Clear(1, 3);
	EXPECT_TRUE(szSource == "a");

	szSource = "abcd";
	szSource.Clear(1, 4);
	EXPECT_TRUE(szSource == "a");
}

#endif // TEST_StringTest == ON


