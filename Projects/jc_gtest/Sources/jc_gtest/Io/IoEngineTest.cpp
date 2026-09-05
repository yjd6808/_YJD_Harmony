#include "gtest/gtest.h"
#include "Core.h"

#include "jc/IO/Engine/IOEngine.h"
#include "jc/IO/File/FileSource.h"
#include "jc/IO/Memory/MemoryDest.h"
#include "jc/IO/Memory/MemorySource.h"
#include "jc/IO/File/FileDest.h"
#include "jc/IO/Engine/IOTypes.h"
#include "jc/IO/File.h"
#include "jc/IO/Path.h"
#include "jc/Env.h"
#include "jc/Threading/ThreadPool.h"
#include "jc/Threading/Thread.h"

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

	bool VerifyPattern(const _byte* _p, int _len, int _startOffset = 0)
	{
		for (int i = 0; i < _len; ++i)
		{
			if (_p[i] != (_byte)((_startOffset + i) % 251))
				return false;
		}
		return true;
	}
}

TEST(IOEngineTest, FileToMemorySyncLoadWritesCallerBuffer)
{
	const String src = MakeSourceFile("mem_load.bin", 4096);

	IOEngine engine;
	engine.Initialize(IOEngineConfig{});

	// 호출자 그릇에 직접 기록 — 결과 회수 단계 없음
	MemoryStreamPtr spOut = MakeShared<MemoryStream>(0u);
	IOResult result = engine.RunSync(MakeShared<FileSource>(src),
		MakeShared<MemoryDest>(spOut, 1LL << 30), src, TransferPolicy{});

	EXPECT_TRUE(result.IsOk());
	EXPECT_EQ(result.error_, ieNone);
	EXPECT_EQ(result.bytesTransferred_, 4096u);
	EXPECT_EQ(spOut->GetSize(), 4096u);
	EXPECT_TRUE(VerifyPattern(spOut->GetData(), 4096));

	engine.Shutdown();
}

TEST(IOEngineTest, FileToFileDownloadCommitsAtomically)
{
	const String src = MakeSourceFile("dl_src.bin", 8192);
	const String dest = String("test_io/engine/dl_out.bin");
	File::Delete(dest);

	IOEngine engine;
	engine.Initialize(IOEngineConfig{});

	IOResult result = engine.RunSync(MakeShared<FileSource>(src),
		MakeShared<FileDest>(dest), src + " -> " + dest, TransferPolicy{});

	EXPECT_TRUE(result.IsOk());
	EXPECT_TRUE(File::Exist(dest));
	EXPECT_EQ(File::Size(dest), 8192);
	// .part 잔여물 없음
	EXPECT_FALSE(File::Exist(dest + ".part.1"));

	engine.Shutdown();
}

TEST(IOEngineTest, MissingFileFailsWithChannelError)
{
	IOEngine engine;
	engine.Initialize(IOEngineConfig{});

	MemoryStreamPtr spOut = MakeShared<MemoryStream>(0u);
	IOResult result = engine.RunSync(MakeShared<FileSource>(String("test_io/engine/no_such_file.bin")),
		MakeShared<MemoryDest>(spOut, 1LL << 30), "missing", TransferPolicy{});

	EXPECT_FALSE(result.IsOk());
	EXPECT_EQ(result.error_, ieOpenFailed);
	EXPECT_NE(result.channelError_, 0);		// P0-2: OS 에러 기록 — R4 무손실

	engine.Shutdown();
}

TEST(IOEngineTest, MemoryLimitFailsWithDedicatedError)
{
	const String src = MakeSourceFile("limit.bin", 2048);

	IOEngine engine;
	engine.Initialize(IOEngineConfig{});

	MemoryStreamPtr spOut = MakeShared<MemoryStream>(0u);
	IOResult result = engine.RunSync(MakeShared<FileSource>(src),
		MakeShared<MemoryDest>(spOut, 1024), src, TransferPolicy{});	// 1KB 한도 < 2KB 페이로드

	EXPECT_FALSE(result.IsOk());
	EXPECT_EQ(result.error_, ieMemoryLimit);

	engine.Shutdown();
}

TEST(IOEngineTest, FixedBufferOverflowFailsWithBufferTooSmall)
{
	const String src = MakeSourceFile("fixed.bin", 2048);

	IOEngine engine;
	engine.Initialize(IOEngineConfig{});

	// §1 계약 2: 고정 버퍼(raw/non-owner) 용량 부족 — bytesTransferred_가 재시도 힌트
	_byte fixed[1024];
	MemoryStreamPtr spWrap = MakeShared<MemoryStream>(fixed, (_u32)sizeof(fixed), false);
	IOResult result = engine.RunSync(MakeShared<FileSource>(src),
		MakeShared<MemoryDest>(spWrap, 1LL << 30), src, TransferPolicy{});

	EXPECT_FALSE(result.IsOk());
	EXPECT_EQ(result.error_, ieBufferTooSmall);
	EXPECT_EQ(result.bytesTransferred_, 1024u);
	EXPECT_TRUE(VerifyPattern(fixed, 1024));

	engine.Shutdown();
}

TEST(IOEngineTest, AsyncCompletesWithCallbackThenListenerAtPump)
{
	const String src = MakeSourceFile("async.bin", 1024);

	IOEngine engine;
	engine.Initialize(IOEngineConfig{});
	CountingListener listener;
	engine.SetListener(&listener);

	Atomic<bool> callbackDone{ false };
	MemoryStreamPtr spOut = MakeShared<MemoryStream>(0u);
	IOHandle handle = engine.Submit(MakeShared<FileSource>(src), MakeShared<MemoryDest>(spOut, 1LL << 30),
		src, TransferPolicy{}, [&](const IOResult& r)
	{
		callbackDone.Store(true);
		EXPECT_TRUE(r.IsOk());
		EXPECT_EQ(r.bytesTransferred_, 1024u);
	});
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

TEST(IOEngineTest, ProgressNotificationsWiredToListener)
{
	// P0-1: 전송 루프 → PushProgress → OnProgress 배선 검증
	//   (계약: policy interval 0 = 데몬 기본값 — 매 청크 통지는 엔진 기본값 0으로)
	const String src = MakeSourceFile("progress.bin", 1024 * 1024);

	IOEngine engine;
	IOEngineConfig cfg;
	cfg.progressNotifyIntervalMs_ = 0;	// 매 청크 통지
	engine.Initialize(cfg);
	CountingListener listener;
	engine.SetListener(&listener);

	Atomic<bool> callbackDone{ false };
	MemoryStreamPtr spOut = MakeShared<MemoryStream>(0u);
	TransferPolicy policy;
	IOHandle handle = engine.Submit(MakeShared<FileSource>(src), MakeShared<MemoryDest>(spOut, 1LL << 30),
		src, policy, [&](const IOResult& r) { callbackDone.Store(r.IsOk()); });
	EXPECT_NE(handle, InvalidIOHandle);

	int spins = 0;
	while (!callbackDone.Load() && spins++ < 5000)
	{
		engine.Pump();
		Thread::Sleep(1);
	}
	EXPECT_TRUE(callbackDone.Load());
	EXPECT_GT(listener.progress_.Load(), 0);	// P0-1 이전에는 영구 0이었음

	engine.Shutdown();
}

TEST(IOEngineTest, CancelRunningTransferViaCancelRequestedFlag)
{
	const int payloadBytes = 4 * 1024 * 1024;
	const String src = MakeSourceFile("cancel_src.bin", payloadBytes);
	const String dest = String("test_io/engine/cancel_out.bin");

	IOEngine engine;
	engine.Initialize(IOEngineConfig{});
	CountingListener listener;
	engine.SetListener(&listener);

	MemoryStreamPtr spOut = MakeShared<MemoryStream>(0u);
	IOHandle handle = engine.Submit(MakeShared<FileSource>(src), MakeShared<MemoryDest>(spOut, 1LL << 30),
		src, TransferPolicy{}, nullptr);

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
	IOEngine engine;
	engine.Initialize(IOEngineConfig{});

	Atomic<bool> failed{ false };
	IOError seen = ieNone;
	IOHandle handle = engine.FailImmediate(ieOpenFailed, "bad/path", [&](const IOResult& r)
	{
		failed.Store(true);
		seen = r.error_;
	});
	EXPECT_NE(handle, InvalidIOHandle);		// P0-5: 발급 핸들을 그대로 반환
	EXPECT_FALSE(failed.Load());
	engine.Pump();
	EXPECT_TRUE(failed.Load());
	EXPECT_EQ(seen, ieOpenFailed);

	engine.Shutdown();
}

TEST(IOEngineTest, SharedThreadPoolIsNotOwnedByEngine)
{
	ThreadPool sharedPool(2);
	IOEngineConfig cfg;
	cfg.pSharedPool_ = &sharedPool;

	IOEngine engine;
	EXPECT_TRUE(engine.Initialize(cfg));

	const String src = MakeSourceFile("shared.bin", 512);
	MemoryStreamPtr spOut = MakeShared<MemoryStream>(0u);
	IOResult result = engine.RunSync(MakeShared<FileSource>(src),
		MakeShared<MemoryDest>(spOut, 1LL << 30), src, TransferPolicy{});
	EXPECT_TRUE(result.IsOk());

	engine.Shutdown();	// 공유 풀은 Join하지 않음 — 이후에도 사용 가능
	Task<void> t = sharedPool.Run([] {});
	t.Wait();
	// 공유 풀 워커 스레드 정리 (~RunnableThread의 핸들 assert 회피 — 풀 소멸 전 명시 Join)
	sharedPool.Join(ThreadPool::JoinStrategy::WaitOnlyRunningTask);
	SUCCEED();
}

TEST(IOEngineTest, MemorySourceFeedsFileDestWithoutCopy)
{
	// P1-3 경로 ①: 소스 직독 (Memory → File Save 경로의 엔진 수준 검증)
	const String dest = String("test_io/engine/memsrc_out.bin");
	File::Delete(dest);

	IOEngine engine;
	engine.Initialize(IOEngineConfig{});

	MemoryStreamPtr spData = MakeShared<MemoryStream>(0u);
	_byte chunk[512];
	for (int i = 0; i < 512; ++i)
		chunk[i] = (_byte)(i % 251);
	spData->Write(chunk, 512);
	spData->Write(chunk, 512);

	IOResult result = engine.RunSync(MakeShared<MemorySource>(spData),
		MakeShared<FileDest>(dest), "mem->file", TransferPolicy{});

	EXPECT_TRUE(result.IsOk());
	EXPECT_EQ(result.bytesTransferred_, 1024u);
	EXPECT_TRUE(File::Exist(dest));
	EXPECT_EQ(File::Size(dest), 1024);

	engine.Shutdown();
}
