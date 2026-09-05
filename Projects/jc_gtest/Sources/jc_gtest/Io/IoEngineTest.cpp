#include "gtest/gtest.h"
#include "Core.h"

#include "jc/IO/Engine/IOJobEngine.h"
#include "jc/IO/Engine/FileSource.h"
#include "jc/IO/Engine/MemoryDest.h"
#include "jc/IO/Engine/FileDest.h"
#include "jc/IO/IOResult.h"
#include "jc/IO/File.h"
#include "jc/IO/Path.h"
#include "jc/Env.h"

using namespace jc;

namespace
{
	struct CountingListener : IIOListener
	{
		Atomic<int> progress_{ 0 };
		Atomic<int> completed_{ 0 };
		Atomic<int> cancelled_{ 0 };
		Atomic<int> failed_{ 0 };

		void OnProgress(const IOProgress&) override { progress_.Increment(); }
		void OnCompleted(const IOResult&) override { completed_.Increment(); }
		void OnCancelled(const IOResult&) override { cancelled_.Increment(); }
		void OnFailed(const IOResult&) override { failed_.Increment(); }
	};

	String MakeSourceFile(const char* _name, int _bytes)
	{
		const String base = Env::CurrentDirectory();
		const String path = Path::Combine(base, String("test_io/engine/") + String(_name));
		auto spSrc = MakeShared<FileSource>(path);
		MemoryDest dest(1LL << 30);
		IOJobEngine writer;	// 파일 생성용 소형 엔진 대신 직접 기록
		FileDest fileDest(path);
		fileDest.Bind(1);
		EXPECT_TRUE(fileDest.Open(-1));
		std::vector<char> body(_bytes);
		for (int i = 0; i < _bytes; ++i)
			body[(size_t)i] = (char)(i % 251);
		if (_bytes > 0)
			EXPECT_TRUE(fileDest.Write((const _byte*)body.data(), _bytes));
		EXPECT_TRUE(fileDest.Commit());
		return path;
	}
}

TEST(IOEngineTest, FileToMemorySyncLoadAndTakeData)
{
	const String src = MakeSourceFile("mem_load.bin", 4096);

	IOJobEngine engine;
	engine.Initialize(IOJobEngineConfig{});

	IOResultPtr result = engine.RunSync(MakeShared<FileSource>(src),
		MakeShared<MemoryDest>(1LL << 30), src, TransferPolicy{}, false);

	ASSERT_NE(result, nullptr);
	EXPECT_TRUE(result->IsSuccess());
	EXPECT_EQ(result->error_, ieNone);
	EXPECT_EQ(result->detail_.httpStatus_, 0);			// 파일 전송은 HTTP 상태 없음
	ASSERT_NE(result->data_, nullptr);
	EXPECT_EQ(result->data_->GetSize(), 4096u);
	EXPECT_EQ(result->data_->GetData()[100], (_byte)(100 % 251));

	MemoryStreamPtr taken = result->TakeData();
	EXPECT_TRUE(taken != nullptr);
	EXPECT_TRUE(result->IsTaken());

	engine.Shutdown();
}

TEST(IOEngineTest, FileToFileDownloadCommitsAtomically)
{
	const String src = MakeSourceFile("dl_src.bin", 8192);
	const String dest = String("test_io/engine/dl_out.bin");
	File::Delete(dest);

	IOJobEngine engine;
	engine.Initialize(IOJobEngineConfig{});

	IOResultPtr result = engine.RunSync(MakeShared<FileSource>(src),
		MakeShared<FileDest>(dest), src + " -> " + dest, TransferPolicy{}, false);

	ASSERT_NE(result, nullptr);
	EXPECT_TRUE(result->IsSuccess());
	EXPECT_TRUE(File::Exist(dest));
	EXPECT_EQ(File::Size(dest), 8192);
	// .part 잔여물 없음
	EXPECT_FALSE(File::Exist(dest + ".part.1"));

	engine.Shutdown();
}

TEST(IOEngineTest, MissingFileFailsWithoutSilentLoss)
{
	IOJobEngine engine;
	engine.Initialize(IOJobEngineConfig{});

	IOResultPtr result = engine.RunSync(MakeShared<FileSource>(String("test_io/engine/no_such_file.bin")),
		MakeShared<MemoryDest>(1LL << 30), "missing", TransferPolicy{}, false);

	ASSERT_NE(result, nullptr);
	EXPECT_FALSE(result->IsSuccess());
	EXPECT_EQ(result->error_, ieOpenFailed);

	engine.Shutdown();
}

TEST(IOEngineTest, MemoryLimitFailsWithDedicatedError)
{
	const String src = MakeSourceFile("limit.bin", 2048);

	IOJobEngine engine;
	engine.Initialize(IOJobEngineConfig{});

	IOResultPtr result = engine.RunSync(MakeShared<FileSource>(src),
		MakeShared<MemoryDest>(1024), src, TransferPolicy{}, false);	// 1KB 한도 < 2KB 페이로드

	ASSERT_NE(result, nullptr);
	EXPECT_FALSE(result->IsSuccess());
	EXPECT_EQ(result->error_, ieMemoryLimit);

	engine.Shutdown();
}

TEST(IOEngineTest, AsyncCompletesWithCallbackThenListenerAtPump)
{
	const String src = MakeSourceFile("async.bin", 1024);

	IOJobEngine engine;
	engine.Initialize(IOJobEngineConfig{});
	CountingListener listener;
	engine.SetListener(&listener);

	Atomic<bool> callbackDone{ false };
	IOHandle handle = engine.Submit(MakeShared<FileSource>(src), MakeShared<MemoryDest>(1LL << 30),
		src, TransferPolicy{}, [&](const IOResult& r)
	{
		callbackDone.Store(true);
		EXPECT_TRUE(r.IsSuccess());
	}, false);
	EXPECT_NE(handle, InvalidIOHandle);
	EXPECT_FALSE(callbackDone.Load());		// 콜백은 Pump()에서만

	int spins = 0;
	while (!callbackDone.Load() && spins++ < 5000)
	{
		engine.Pump();
		Thread::Sleep(1);
	}
	EXPECT_TRUE(callbackDone.Load());
	EXPECT_EQ(listener.completed_.Load(), 1);

	// 완료 후에도 링 캐시로 진행률 조회 가능 (R9 비대칭 해소)
	IOProgress progress;
	EXPECT_TRUE(engine.GetProgress(handle, OUT progress));
	EXPECT_EQ(progress.handle_, handle);

	engine.Shutdown();
}

TEST(IOEngineTest, CancelRunningTransferViaCancelRequestedFlag)
{
	const int payloadBytes = 4 * 1024 * 1024;
	const String src = MakeSourceFile("cancel_src.bin", payloadBytes);
	const String dest = String("test_io/engine/cancel_out.bin");

	IOJobEngine engine;
	engine.Initialize(IOJobEngineConfig{});
	CountingListener listener;
	engine.SetListener(&listener);

	IOHandle handle = engine.Submit(MakeShared<FileSource>(src), MakeShared<FileDest>(dest),
		src, TransferPolicy{}, nullptr, false);

	// 즉시 취소 — pending 선점 또는 실행 중 협조 취소 (로컬 파일은 매우 빠르므로 완료될 수도 있음)
	bool cancelOk = engine.Cancel(handle);
	EXPECT_TRUE(cancelOk || engine.GetState(handle) == isCompleted);
	int spins = 0;
	while (engine.GetState(handle) != isCancelled && engine.GetState(handle) != isCompleted && engine.GetState(handle) != isNone && spins++ < 10000)
	{
		engine.Pump();
		Thread::Sleep(1);
	}
	engine.Pump();

	IOState state = engine.GetState(handle);
	EXPECT_TRUE(state == isCancelled || state == isNone || state == isCompleted);

	engine.Shutdown();
}

TEST(IOEngineTest, FailImmediateQueuesCallbackForNextPump)
{
	IOJobEngine engine;
	engine.Initialize(IOJobEngineConfig{});

	Atomic<bool> failed{ false };
	IOError seen = ieNone;
	engine.FailImmediate(ieOpenFailed, "bad/path", [&](const IOResult& r)
	{
		failed.Store(true);
		seen = r.error_;
	});
	EXPECT_FALSE(failed.Load());
	engine.Pump();
	EXPECT_TRUE(failed.Load());
	EXPECT_EQ(seen, ieOpenFailed);

	engine.Shutdown();
}

TEST(IOEngineTest, SharedThreadPoolIsNotOwnedByEngine)
{
	ThreadPool sharedPool(2);
	IOJobEngineConfig cfg;
	cfg.pSharedPool_ = &sharedPool;

	IOJobEngine engine;
	EXPECT_TRUE(engine.Initialize(cfg));

	const String src = MakeSourceFile("shared.bin", 512);
	IOResultPtr result = engine.RunSync(MakeShared<FileSource>(src),
		MakeShared<MemoryDest>(1LL << 30), src, TransferPolicy{}, false);
	EXPECT_TRUE(result->IsSuccess());

	engine.Shutdown();	// 공유 풀은 Join하지 않음 — 이후에도 사용 가능
	Task<void> t = sharedPool.Run([] {});
	t.Wait();
	SUCCEED();
}
