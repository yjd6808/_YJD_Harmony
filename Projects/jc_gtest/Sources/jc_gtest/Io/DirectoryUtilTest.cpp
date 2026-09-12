/*
 * 작성자: 윤정도
 * 생성일: 1/8/2023 6:58:48 AM
 * =====================
 *
 */



#include "jc/Container/Vector.h"

#include "jc/IO/Directory.h"
#include "jc/IO/File.h"
#include "jc/Env.h"

#if TEST_DirectoryTest == ON

TEST(DirectoryTest, BasicTest) {

	EXPECT_TRUE(Directory::Create(_T("abcd1")));
	EXPECT_TRUE(Directory::Create(_T("abcd1/a")));
	EXPECT_TRUE(Directory::Create(_T("abcd2")));

	EXPECT_TRUE(Directory::Exist(_T("abcd1")));
	EXPECT_TRUE(Directory::Exist(_T("abcd1/a")));
	EXPECT_TRUE(Directory::Exist(_T("abcd2")));

	for (int i = 0; i < 36; i++) {
		File::WriteAllText(_T("fsefes"), StringUtilA::Format("abcd1/a/%d.txt", i).Source());
		File::WriteAllText(_T("fesfesf"), StringUtilA::Format("abcd1/%d.txt", i).Source());
		File::WriteAllText(_T("fesfesf"), StringUtilA::Format("abcd2/%d.txt", i).Source());
	}

	String szPath = Env::CurrentDirectory();


	EXPECT_TRUE(Directory::Delete(_T("abcd1")));
	EXPECT_TRUE(Directory::Delete(_T("abcd2")));

	EXPECT_TRUE(Directory::Exist(_T("abcd1")) == false);
	EXPECT_TRUE(Directory::Exist(_T("abcd1/a")) == false);
	EXPECT_TRUE(Directory::Exist(_T("abcd2")) == false);

#if Print
	for (int i = 0; i < files.Size(); ++i) {
		PrintFormat(_T("%s\n"), files[i].Source());
	}
#endif
}

#endif
