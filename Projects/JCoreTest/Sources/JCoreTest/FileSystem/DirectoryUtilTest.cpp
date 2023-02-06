/*
 * 작성자: 윤정도
 * 생성일: 1/8/2023 6:58:48 AM
 * =====================
 *
 */



#include <JCoreTest/CoreTest.h>
#include <JCore/Container/Vector.h>

#include <JCore/FileSystem/Directory.h>
#include <JCore/FileSystem/File.h>
#include <JCore/Env.h>

#if TEST_DirectoryTest == ON

TEST(DirectoryTest, BasicTest) {

	EXPECT_TRUE(Directory::Create("abcd1"));
	EXPECT_TRUE(Directory::Create("abcd1/a"));
	EXPECT_TRUE(Directory::Create("abcd2"));

	EXPECT_TRUE(Directory::Exist("abcd1"));
	EXPECT_TRUE(Directory::Exist("abcd1/a"));
	EXPECT_TRUE(Directory::Exist("abcd2"));

	for (int i = 0; i < 36; i++) {
		File::WriteAllText("fsefes", StringUtil::Format("abcd1/a/%d.txt", i).Source());
		File::WriteAllText("fesfesf", StringUtil::Format("abcd1/%d.txt", i).Source());
		File::WriteAllText("fesfesf", StringUtil::Format("abcd2/%d.txt", i).Source());
	}

	String szPath = Env::CurrentDirectory();


	Vector<String> files = Directory::Files(szPath.Source(), true);
	EXPECT_TRUE(files.Size() > (36 * 3));

	EXPECT_TRUE(Directory::Delete("abcd1"));
	EXPECT_TRUE(Directory::Delete("abcd2"));

	EXPECT_TRUE(Directory::Exist("abcd1") == false);
	EXPECT_TRUE(Directory::Exist("abcd1/a") == false);
	EXPECT_TRUE(Directory::Exist("abcd2") == false);

#if Print
	for (int i = 0; i < files.Size(); ++i) {
		PrintFormat("%s\n", files[i].Source());
	}
#endif
}

#endif