/*
 * 작성자: 윤정도
 * 생성일: 1/7/2023 8:21:08 AM
 * =====================
 *
 */



#include "jc/IO/File.h"

#if TEST_FileUtilTest == ON

TEST(FileUtilsTest, BasicTest)
{
	CRuntime::FileDelete(_T("a.txt"));
	CRuntime::FileDelete(_T("b.txt"));

	File::WriteAllText(_T("안녕하세요"), "a.txt");
	EXPECT_TRUE(File::ReadAllText("a.txt") == _T("안녕하세요"));

	EXPECT_TRUE(File::Copy(_T("a.txt"), _T("b.txt")));
	EXPECT_TRUE(File::Exist(_T("a.txt")));
	EXPECT_TRUE(File::Exist(_T("b.txt")));

	CRuntime::FileDelete(_T("a.txt"));
	CRuntime::FileDelete(_T("b.txt"));
	
}

#endif
