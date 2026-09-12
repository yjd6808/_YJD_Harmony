/*
 * 작성자: 윤정도
 * 생성일: 9/6/2026
 * =====================
 * ObjectPoolTLSIndexed<T> 단독 테스트 (Dummy64, V1~V9)
 *
 * 스레드 상한: 논리 프로세서 수 / 2 (8논리 머신 → 4개).
 * 대기는 전부 블로킹 동기화(mutex + condition_variable, promise/future)만 쓴다.
 * 스핀 대기는 금지한다. 수백 개 스핀 스레드가 전 CPU를 점유해 머신이 멈춘 전례가 있다.
 */

#include "jc/Pool/ObjectPoolTLSIndexed.h"

#include <thread>
#include <vector>
#include <atomic>
#include <mutex>
#include <condition_variable>
#include <future>

using namespace std;

#if TEST_ObjectPoolTLSIndexedTest == ON

struct Dummy64 : TlsPoolNode
{
	char pad_[JC_CACHE_ALIGN_SIZE - sizeof(TlsPoolNode)];
};
using TDummyPool = ObjectPoolTLSIndexed<Dummy64>;

// 1회용 블로킹 배리어. 카운트에 도달하기 전에는 CV에서 잔다. 스핀 없음.
class TestGate
{
public:
	explicit TestGate(int _count) : count_(_count) {}
	void Arrive()
	{
		unique_lock<mutex> lock(mutex_);
		++arrived_;
		if (arrived_ >= count_)
		{
			cv_.notify_all();
			return;
		}
		cv_.wait(lock, [&] { return arrived_ >= count_; });
	}
private:
	mutex mutex_;
	condition_variable cv_;
	int count_;
	int arrived_ = 0;
};

inline int WorkerThreadCount()
{
	const unsigned n = thread::hardware_concurrency();
	return static_cast<int>(max(2u, n / 2));
}

// Pool·고아 락은 의도적 영구 객체(D8)라 CRT 델타 검사(LeakCheck)에 걸린다.
// 모든 테스트 시작 전에 워밍업으로 미리 만들어 두면, 이후 LeakCheck 구간에는 객체(해제 대상)만 오간다.
// 워밍업 스레드 수도 상한(논리 프로세서 / 2)을 지킨다. 전부 게이트에서 블로킹 대기한다.
inline void WarmupOnce()
{
	static const bool warmed = [] {
		// 워밍업은 최대 동시 수요(워커 상한 + 메인 보유 1)보다 1개 많게 만든다.
		// 이후 테스트는 전부 고아 재사용이라 CreatePool(영구 객체)이 LeakCheck 구간에 안 잡힌다.
		const int threadCount = WorkerThreadCount() + 2;
		vector<Dummy64*> holders(threadCount, nullptr);
		TestGate gate(threadCount + 1);
		vector<thread> workers;
		workers.reserve(threadCount);
		for (int i = 0; i < threadCount; ++i)
		{
			workers.emplace_back([&, i] {
				holders[i] = TDummyPool::Pop();
				gate.Arrive();				// 전원 생존 상태에서 만나서 동시 풀 확보
				TDummyPool::Push(holders[i]);
			});
		}
		gate.Arrive();
		for (thread& w : workers)
		{
			w.join();
		}
		return true;
	}();
	(void)warmed;
}

TEST(ObjectPoolTLSIndexedTest, LIFO)
{
	WarmupOnce();
	LeakCheck;

	Dummy64* pA = TDummyPool::Pop();
	Dummy64* pB = TDummyPool::Pop();
	TDummyPool::Push(pB);
	TDummyPool::Push(pA);

	EXPECT_EQ(TDummyPool::Pop(), pA);		// LIFO: 마지막에 넣은 a가 먼저 나온다.
	TDummyPool::Push(pA);

	TDummyPool::Stats stats = TDummyPool::GetStats();
	EXPECT_EQ(stats.totalCount, 2u);
	TDummyPool::TrimThisThread();			// 이 테스트가 만든 잔여물을 힙으로 (뒤 테스트와 격리)
}

TEST(ObjectPoolTLSIndexedTest, PoolIdxInvariant)
{
	WarmupOnce();
	LeakCheck;

	Dummy64* pObj = TDummyPool::Pop();
	const _u32 idx = pObj->poolIdx_;
	TDummyPool::Push(pObj);
	Dummy64* pReused = TDummyPool::Pop();
	EXPECT_EQ(pReused->poolIdx_, idx);		// I1: 도장은 바뀌지 않는다.
	EXPECT_EQ(pReused, pObj);
	TDummyPool::Push(pReused);
	TDummyPool::TrimThisThread();
}

TEST(ObjectPoolTLSIndexedTest, SpscCrossThread)
{
	WarmupOnce();
	LeakCheck;

	constexpr int kCount = 10000;
	vector<Dummy64*> objs;
	objs.reserve(kCount);
	for (int i = 0; i < kCount; ++i)
	{
		objs.push_back(TDummyPool::Pop());
	}
	const _u64 totalBefore = TDummyPool::GetStats().totalCount;

	thread worker([&objs] {
		for (Dummy64* pObj : objs)
		{
			TDummyPool::Push(pObj);		// foreign 반납 → owner inbox
		}
	});
	worker.join();

	for (int i = 0; i < kCount; ++i)	// inbox drain으로 회수. 힙 추가 할당 없어야 한다.
	{
		objs[i] = TDummyPool::Pop();
	}
	EXPECT_EQ(TDummyPool::GetStats().totalCount, totalBefore);

	for (Dummy64* pObj : objs)
	{
		TDummyPool::Push(pObj);
	}
	TDummyPool::TrimThisThread();
}

TEST(ObjectPoolTLSIndexedTest, OwnerExitForeignPush)
{
	WarmupOnce();
	LeakCheck;

	const _u32 orphanBefore = TDummyPool::GetStats().orphanPoolCount;

	promise<Dummy64*> made;
	thread maker([&made] {
		made.set_value(TDummyPool::Pop());
	});									// maker 종료 → 풀 고아화. 객체는 리스트 어디에도 없다.
	Dummy64* pObj = made.get_future().get();
	maker.join();

	TDummyPool::Push(pObj);				// owner 죽음 → 즉시 힙 반납. 크래시·누수 없어야 한다.
	// maker는 고아 번호를 빌렸다가 반납하므로(net 0) 고아 수는 그대로다.
	EXPECT_EQ(TDummyPool::GetStats().orphanPoolCount, orphanBefore);
	TDummyPool::SweepOrphans();
}

TEST(ObjectPoolTLSIndexedTest, PoolIdxReuse)
{
	WarmupOnce();
	LeakCheck;

	atomic<_u32> idxA{ TDummyPool::kNone };
	thread first([&idxA] {
		Dummy64* pObj = TDummyPool::Pop();
		idxA.store(pObj->poolIdx_);
		TDummyPool::Push(pObj);
	});									// 종료 시 번호 반납
	first.join();

	atomic<_u32> idxC{ TDummyPool::kNone };
	thread second([&idxC] {
		Dummy64* pObj = TDummyPool::Pop();
		idxC.store(pObj->poolIdx_);
		TDummyPool::Push(pObj);
	});
	second.join();

	EXPECT_EQ(idxC.load(), idxA.load());	// 고아 pop: 같은 번호 물려받는다.
	TDummyPool::SweepOrphans();
}

TEST(ObjectPoolTLSIndexedTest, ReassignedPoolReuse)
{
	WarmupOnce();
	LeakCheck;

	// A가 만든 객체를 B가 들고 있는 사이 A 종료 → C가 번호 인수 → B 반납 → C가 재사용 (R4)
	promise<Dummy64*> made;
	thread threadA([&made] {
		made.set_value(TDummyPool::Pop());
	});
	Dummy64* pObj = made.get_future().get();
	threadA.join();						// A 종료. pObj의 poolIdx_ 풀은 고아.

	promise<void> attached;
	promise<void> go;
	promise<bool> reused;
	thread threadC([&attached, &go, &reused, pObj] {
		Dummy64* pWarm = TDummyPool::Pop();	// 번호 인수 (alive_ = 1)
		TDummyPool::Push(pWarm);
		attached.set_value();
		go.get_future().wait();
		Dummy64* pFirst = TDummyPool::Pop();	// pFree_에 있던 pWarm
		Dummy64* pSecond = TDummyPool::Pop();	// inbox drain으로 들어온 pObj
		reused.set_value(pFirst == pObj || pSecond == pObj);
		TDummyPool::Push(pFirst);
		TDummyPool::Push(pSecond);
	});
	attached.get_future().wait();
	TDummyPool::Push(pObj);				// B 역할 (메인 스레드). 새 owner inbox로 간다.
	go.set_value();
	EXPECT_TRUE(reused.get_future().get());	// C가 pObj를 회수했다 (R4, 정상 재사용)
	threadC.join();

	TDummyPool::SweepOrphans();
}

TEST(ObjectPoolTLSIndexedTest, ThreadCreateDestroyLoop)
{
	WarmupOnce();
	LeakCheck;

	const _u32 poolBefore = TDummyPool::GetStats().poolCount;
	for (int i = 0; i < 200; ++i)
	{
		thread worker([] {
			Dummy64* pObj = TDummyPool::Pop();
			TDummyPool::Push(pObj);
		});
		worker.join();
	}

	EXPECT_EQ(TDummyPool::GetStats().poolCount, poolBefore);	// 번호 재사용: 새로 만들지 않는다.
	TDummyPool::SweepOrphans();
}

TEST(ObjectPoolTLSIndexedTest, CrossPushStress)
{
	WarmupOnce();
	LeakCheck;

	// 상한(논리 프로세서 / 2) 스레드로 교차 반납 스트레스. 세그먼트 구간(idx 256+)은
	// 동시 257풀이 필요해서 상한 안에서는 도달 불가. 해당 분기는 코드 리뷰로 커버한다(§16).
	const int threadCount = WorkerThreadCount();
	constexpr int kObjects = 500;
	vector<vector<Dummy64*>> bags(threadCount);
	TestGate gate(threadCount + 1);

	vector<thread> workers;
	workers.reserve(threadCount);
	for (int i = 0; i < threadCount; ++i)
	{
		workers.emplace_back([&, i] {
			bags[i].reserve(kObjects);
			for (int j = 0; j < kObjects; ++j)
			{
				bags[i].push_back(TDummyPool::Pop());
			}
			gate.Arrive();				// 전원 확보 후 교차 반납 시작
			vector<Dummy64*>& others = bags[(i + 1) % threadCount];
			for (int j = 0; j < kObjects; ++j)
			{
				TDummyPool::Push(others[j]);	// 남의 풀 inbox로 CAS push
			}
		});
	}
	gate.Arrive();
	for (thread& w : workers)
	{
		w.join();
	}

	TDummyPool::SweepOrphans();			// 고아 inbox 잔류 회수. 누수 없어야 한다.
}

TEST(ObjectPoolTLSIndexedTest, TrimAndSweep)
{
	WarmupOnce();
	LeakCheck;

	Dummy64* pA = TDummyPool::Pop();
	Dummy64* pB = TDummyPool::Pop();
	TDummyPool::Push(pA);
	TDummyPool::Push(pB);

	TDummyPool::TrimThisThread();		// 현 스레드 pFree_ 힙 반납 + 고아 sweep
	TDummyPool::Stats stats = TDummyPool::GetStats();
	EXPECT_EQ(stats.freeCount, 0u);
}

#endif
