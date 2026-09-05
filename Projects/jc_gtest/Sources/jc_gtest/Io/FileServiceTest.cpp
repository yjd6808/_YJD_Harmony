#include "gtest/gtest.h"
#include "Core.h"

#include "jc/IO/IODaemon.h"
#include "jc/IO/File.h"
#include "jc/IO/Directory.h"
#include "jc/IO/Path.h"
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

	bool VerifyPattern(const _byte* _p, int _len, int _startOffset = 0)
	{
		for (int i = 0; i < _len; ++i)
		{
			if (_p[i] != (_byte)((_startOffset + i) % 251))
				return false;
		}
		return true;
	}

	struct IOTestDaemon
	{
		IODaemon daemon;

		IOTestDaemon()
		{
			EXPECT_TRUE(daemon.Initialize());
			PathResolver& fs = daemon.Resolver();
			fs.Mount("src", "test_io/filesvc/src");
			fs.Mount("save", "test_io/filesvc/save");
			fs.SetDefaultAlias("src");
		}
		~IOTestDaemon() { daemon.Shutdown(); }
	};
}

TEST(FileServiceTest, LoadSyncWritesCallerBuffer)
{
	IOTestDaemon env;
	MakeFile("src/table.bin", 1024);

	MemoryStreamPtr spOut = MakeShared<MemoryStream>(0u);
	IOResult result = env.daemon.Files().Load("src:/table.bin", spOut);

	EXPECT_TRUE(result.IsOk());
	EXPECT_EQ(result.bytesTransferred_, 1024u);
	ASSERT_EQ(spOut->GetSize(), 1024u);
	EXPECT_TRUE(VerifyPattern(spOut->GetData(), 1024));
}

TEST(FileServiceTest, LoadAsyncWithExplicitAlias)
{
	IOTestDaemon env;
	MakeFile("src/table_async.bin", 1024);

	Atomic<bool> done{ false };
	MemoryStreamPtr spOut = MakeShared<MemoryStream>(0u);
	IOHandle h = env.daemon.Files().LoadAsync("src:/table_async.bin", spOut,
		[&](const IOResult& r)
	{
		done.Store(true);
		EXPECT_TRUE(r.IsOk());
		EXPECT_EQ(r.bytesTransferred_, 1024u);
	});
	EXPECT_NE(h, InvalidIOHandle);

	int spins = 0;
	while (!done.Load() && spins++ < 5000)
	{
		env.daemon.Update();
		jc::Thread::Sleep(1);
	}
	EXPECT_TRUE(done.Load());
	EXPECT_TRUE(VerifyPattern(spOut->GetData(), (int)spOut->GetSize()));
}

TEST(FileServiceTest, LoadPartialOffsetAndCount)
{
	// P2-1: 오프셋/카운트 부분 읽기
	IOTestDaemon env;
	MakeFile("src/partial.bin", 1024);

	MemoryStreamPtr spOut = MakeShared<MemoryStream>(0u);
	IOResult result = env.daemon.Files().Load("src:/partial.bin", spOut, 100, 200);

	EXPECT_TRUE(result.IsOk());
	EXPECT_EQ(result.bytesTransferred_, 200u);
	ASSERT_EQ(spOut->GetSize(), 200u);
	EXPECT_TRUE(VerifyPattern(spOut->GetData(), 200, 100));
}

TEST(FileServiceTest, LoadRawBufferOverflowReportsBufferTooSmall)
{
	// §1 계약 2: 고정 버퍼 용량 부족 시 실패 + bytesTransferred_ 힌트
	IOTestDaemon env;
	MakeFile("src/raw.bin", 512);

	_byte fixed[128];
	IOResult result = env.daemon.Files().Load("src:/raw.bin", fixed, sizeof(fixed));

	EXPECT_FALSE(result.IsOk());
	EXPECT_EQ(result.error_, ieBufferTooSmall);
	EXPECT_EQ(result.bytesTransferred_, 128u);
	EXPECT_TRUE(VerifyPattern(fixed, 128));
}

TEST(FileServiceTest, SaveRoundTripsMemoryToFile)
{
	// P1-2: 메모리 → 파일 저장 (.part 원자적 커밋)
	IOTestDaemon env;

	MemoryStreamPtr spData = MakeShared<MemoryStream>(0u);
	_byte chunk[256];
	for (int i = 0; i < 256; ++i)
		chunk[i] = (_byte)(i % 251);
	spData->Write(chunk, 256);

	IOResult saved = env.daemon.Files().Save("save:/roundtrip.bin", spData);
	EXPECT_TRUE(saved.IsOk());
	EXPECT_EQ(saved.bytesTransferred_, 256u);

	MemoryStreamPtr spOut = MakeShared<MemoryStream>(0u);
	IOResult loaded = env.daemon.Files().Load("save:/roundtrip.bin", spOut);
	EXPECT_TRUE(loaded.IsOk());
	ASSERT_EQ(spOut->GetSize(), 256u);
	EXPECT_TRUE(VerifyPattern(spOut->GetData(), 256));
}

TEST(FileServiceTest, SaveAsyncCompletesThroughPump)
{
	IOTestDaemon env;

	MemoryStreamPtr spData = MakeShared<MemoryStream>(0u);
	_byte chunk[128];
	for (int i = 0; i < 128; ++i)
		chunk[i] = (_byte)(i % 251);
	spData->Write(chunk, 128);

	Atomic<bool> done{ false };
	IOHandle h = env.daemon.Files().SaveAsync("save:/async_save.bin", spData,
		[&](const IOResult& r)
	{
		done.Store(true);
		EXPECT_TRUE(r.IsOk());
		EXPECT_EQ(r.bytesTransferred_, 128u);
	});
	EXPECT_NE(h, InvalidIOHandle);

	int spins = 0;
	while (!done.Load() && spins++ < 5000)
	{
		env.daemon.Update();
		jc::Thread::Sleep(1);
	}
	EXPECT_TRUE(done.Load());
}

TEST(FileServiceTest, LoadTextAndSaveTextHelpers)
{
	IOTestDaemon env;

	IOResult saved = env.daemon.Files().SaveText("save:/hello.txt", String("hello io"));
	EXPECT_TRUE(saved.IsOk());

	String text = env.daemon.Files().LoadText("save:/hello.txt");
	EXPECT_TRUE(text == String("hello io"));

	String missing = env.daemon.Files().LoadText("save:/no_such_file.txt");
	EXPECT_TRUE(missing.IsEmpty());
}

TEST(FileServiceTest, DefaultAliasAndRequestOverride)
{
	IOTestDaemon env;
	MakeFile("src/default.txt", 16);
	MakeFile("save/overridden.txt", 32);

	// 디폴트 별칭(src) 기준 — 접두어 없는 상대경로
	MemoryStreamPtr spA = MakeShared<MemoryStream>(0u);
	IOResult a = env.daemon.Files().Load("default.txt", spA);
	EXPECT_TRUE(a.IsOk());
	EXPECT_EQ(spA->GetSize(), 16u);

	// 요청 단위 오버라이드 — 이 요청만 save 별칭 사용
	LoadOptions options;
	options.baseAlias_ = "save";
	MemoryStreamPtr spB = MakeShared<MemoryStream>(0u);
	IOResult b = env.daemon.Files().Load("overridden.txt", spB, 0, 0, options);
	EXPECT_TRUE(b.IsOk());
	EXPECT_EQ(spB->GetSize(), 32u);

	// 존재하지 않는 파일 → Update()에서 ieOpenFailed 통지 대기열 진입
	MemoryStreamPtr spBad = MakeShared<MemoryStream>(0u);
	IOHandle bad = env.daemon.Files().LoadAsync("not_exist_file.bin", spBad);
	EXPECT_NE(bad, InvalidIOHandle);
	env.daemon.Update();	// FailImmediate 큐 소비 — 리스너 없으니 무통지로 정리만
	EXPECT_EQ(env.daemon.GetState(bad), isNone);	// 펌프 완료 후 activeJobs 제거
}

namespace
{
	struct CaptureListener : IIOListener
	{
		IOError lastError_ = ieNone;
		bool failedNotified_ = false;

		void OnFailed(const IOResult& r) override
		{
			failedNotified_ = true;
			lastError_ = r.error_;
		}
	};
}

TEST(FileServiceTest, MissingFileFailsThroughPumpWithListener)
{
	IOTestDaemon env;
	CaptureListener listener;
	env.daemon.SetListener(&listener);

	MemoryStreamPtr spOut = MakeShared<MemoryStream>(0u);
	IOHandle h = env.daemon.Files().LoadAsync("ghost.bin", spOut);
	EXPECT_NE(h, InvalidIOHandle);

	int spins = 0;
	while (!listener.failedNotified_ && spins++ < 1000)
	{
		env.daemon.Update();
		jc::Thread::Sleep(1);
	}
	EXPECT_TRUE(listener.failedNotified_);
	EXPECT_EQ(listener.lastError_, ieOpenFailed);
}

TEST(FileServiceTest, MountRebindingAffectsOnlyNewRequests)
{
	IOTestDaemon env;
	MakeFile("src/rebind.txt", 8);

	// 첫 해석 성공
	MemoryStreamPtr spFirst = MakeShared<MemoryStream>(0u);
	IOResult first = env.daemon.Files().Load("rebind.txt", spFirst);
	EXPECT_TRUE(first.IsOk());

	// 재바인딩: src를 빈 디렉토리로 교체 → 이후 접수분은 미스
	env.daemon.Resolver().Mount("src", "test_io/filesvc/empty_dir");

	MemoryStreamPtr spSecond = MakeShared<MemoryStream>(0u);
	IOResult second = env.daemon.Files().Load("rebind.txt", spSecond);
	EXPECT_FALSE(second.IsOk());
	EXPECT_EQ(second.error_, ieOpenFailed);
}
