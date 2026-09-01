#include "gtest/gtest.h"
#include "Core.h"

#include "jc/Io/IoDaemon.h"
#include "jc/FileSystem/File.h"
#include "jc/FileSystem/Directory.h"
#include "jc/FileSystem/Path.h"
#include "jc/Env.h"
#include "jc/Threading/Thread.h"

using namespace jc;

namespace
{
	String MakeFile(const char* _rel, int _bytes)
	{
		const String base = Env::CurrentDirectory();
		const String path = Path::Combine(base, String("test_io/filesvc/") + String(_rel));
		{ int slash = path.FindReverse("/"); int bslash = path.FindReverse("\\"); int pos = slash > bslash ? slash : bslash; if (pos > 0) { String parent = path.SubStr(0, pos); Vector<String> segs; String cur; for (int i = 0; i < parent.Length(); ++i) { cur.Append(parent[i]); if (parent[i] == '/' || parent[i] == '\\' || i == parent.Length()-1) { String dir = cur; while (dir.Length()>0 && (dir.Last()=='/' || dir.Last()=='\\')) dir = dir.SubStr(0, dir.Length()-1); if (!dir.IsEmpty() && !Directory::Exist(dir)) Directory::Create(dir); } } } }
		File::Delete(path);
		std::vector<char> body(_bytes > 0 ? _bytes : 1);
		for (int i = 0; i < (int)body.size(); ++i)
			body[(size_t)i] = (char)(i % 251);
		File::WriteAllBytes((_byte*)body.data(), (int)body.size(), path.Source());
		return path;
	}

	struct IoTestDaemon
	{
		IoDaemon daemon;

		IoTestDaemon()
		{
			EXPECT_TRUE(daemon.Initialize());
			PathResolver& fs = daemon.Resolver();
			fs.Mount("src", "test_io/filesvc/src");
			fs.Mount("save", "test_io/filesvc/save");
			fs.SetDefaultAlias("src");
		}
		~IoTestDaemon() { daemon.Shutdown(); }
	};
}

TEST(FileServiceTest, LoadAsyncWithExplicitAlias)
{
	IoTestDaemon env;
	MakeFile("src/table.bin", 1024);
	MakeFile("src/table2.bin", 1024);

	env.daemon.Files().LoadAsync("src:/table.bin", [](const IoResult& r)
	{
	}, LoadOptions());

	// 동기 API로 확정 검증 (다른 파일로 동시 접근 회피)
	IoResultPtr result = env.daemon.Files().Load("src:/table2.bin");
	ASSERT_NE(result, nullptr);
	EXPECT_TRUE(result->IsSuccess());
	ASSERT_NE(result->data_, nullptr);
	EXPECT_EQ(result->data_->GetSize(), 1024u);
}

TEST(FileServiceTest, DefaultAliasAndRequestOverride)
{
	IoTestDaemon env;
	MakeFile("src/default.txt", 16);
	MakeFile("save/overridden.txt", 32);

	// 디폴트 별칭(src) 기준 — 접두어 없는 상대경로
	IoResultPtr a = env.daemon.Files().Load("default.txt");
	ASSERT_NE(a, nullptr);
	EXPECT_TRUE(a->IsSuccess());

	// 요청 단위 오버라이드 — 이 요청만 save 별칭 사용
	LoadOptions options;
	options.baseAlias_ = "save";
	IoResultPtr b = env.daemon.Files().Load("overridden.txt", options);
	ASSERT_NE(b, nullptr);
	EXPECT_TRUE(b->IsSuccess());

	// 존재하지 않는 파일 → Update()에서 ieOpenFailed 통지 대기열 진입
	IoHandle bad = env.daemon.Files().LoadAsync("not_exist_file.bin");
	EXPECT_NE(bad, InvalidIoHandle);
	env.daemon.Update();	// FailImmediate 큐 소비 — 리스너 없으니 무통지로 정리만
	EXPECT_EQ(env.daemon.GetState(bad), isNone);	// 펌프 완료 후 activeJobs 제거
}

namespace
{
	struct CaptureListener : IIoListener
	{
		IoError lastError_ = ieNone;
		bool failedNotified_ = false;

		void OnFailed(const IoResult& r) override
		{
			failedNotified_ = true;
			lastError_ = r.error_;
		}
	};
}

TEST(FileServiceTest, MissingFileFailsThroughPumpWithListener)
{
	IoTestDaemon env;
	CaptureListener listener;
	env.daemon.SetListener(&listener);

	IoHandle h = env.daemon.Files().LoadAsync("ghost.bin");
	EXPECT_NE(h, InvalidIoHandle);

	int spins = 0;
	while (!listener.failedNotified_ && spins++ < 1000)
	{
		env.daemon.Update();
		jc::Thread::Sleep(1);
	}
	EXPECT_TRUE(listener.failedNotified_);
	EXPECT_EQ(listener.lastError_, ieOpenFailed);
}

TEST(FileServiceTest, DownloadUsesResolveWritableAndCommits)
{
	IoTestDaemon env;
	MakeFile("src/payload.dat", 2048);
	{
		auto r = env.daemon.Resolver().ResolveWritable("save:/out.dat");
		File::Delete(r.fullPath_);
	}
	IoResultPtr result = env.daemon.Files().Download("src:/payload.dat", "save:/out.dat");
	ASSERT_NE(result, nullptr);
	EXPECT_TRUE(result->IsSuccess());
	auto resolved = env.daemon.Resolver().ResolveWritable("save:/out.dat");
	EXPECT_TRUE(File::Exist(resolved.fullPath_));
	EXPECT_EQ(File::Size(resolved.fullPath_), 2048);
}

TEST(FileServiceTest, MountRebindingAffectsOnlyNewRequests)
{
	IoTestDaemon env;
	MakeFile("src/rebind.txt", 8);

	// 첫 해석 성공
	IoResultPtr first = env.daemon.Files().Load("rebind.txt");
	ASSERT_NE(first, nullptr);
	EXPECT_TRUE(first->IsSuccess());

	// 재바인딩: src를 빈 디렉토리로 교체 → 이후 접수분은 미스
	env.daemon.Resolver().Mount("src", "test_io/filesvc/empty_dir");

	IoResultPtr second = env.daemon.Files().Load("rebind.txt");
	ASSERT_NE(second, nullptr);
	EXPECT_FALSE(second->IsSuccess());
	EXPECT_EQ(second->error_, ieOpenFailed);
}
