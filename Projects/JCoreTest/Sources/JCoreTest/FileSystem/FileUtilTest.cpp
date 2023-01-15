/*
 * 작성자: 윤정도
 * 생성일: 1/7/2023 8:21:08 AM
 * =====================
 *
 */


#include <JCoreTest/CoreTest.h>

#include <JCore/FileSystem/File.h>

#if TEST_FileUtilTest == ON

TEST(FileUtilsTest, BasicTest) {
	CRuntime::FileDelete("a.txt");
	CRuntime::FileDelete("b.txt");

	File::WriteAllText("안녕하세요", "a.txt");
	EXPECT_TRUE(File::ReadAllText("a.txt") == "안녕하세요");

	EXPECT_TRUE(File::Copy("a.txt", "b.txt"));
	EXPECT_TRUE(File::Exist("a.txt"));
	EXPECT_TRUE(File::Exist("b.txt"));

	CRuntime::FileDelete("a.txt");
	CRuntime::FileDelete("b.txt");
	
}

#endif