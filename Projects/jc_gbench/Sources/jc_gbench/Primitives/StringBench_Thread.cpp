#include "Core.h"
#include "jc_gbench/Primitives/StringBench_Common.h"

#include <vector>
#include <atomic>

using namespace jc_gbench;

namespace jc_gbench {
// S6 명시 pump / 보관소 정리를 타입별로 연결한다. 나머지 타입은 no-op이다.
template <typename TString> inline void StrBenchPumpPool() {}
template <typename TString> inline void StrBenchDrainPools() {}
template <typename TString> inline void StrBenchPurgePools() {}
#if BenchStrS6 == ON
template <> inline void StrBenchPumpPool<BenchS6>() { s6::ProcessThreadPoolDeferred(); }
template <> inline void StrBenchDrainPools<BenchS6>() { benchpool::GlobalXThreadStorageS6().DrainAll(); }
template <> inline void StrBenchPurgePools<BenchS6>() { benchpool::GlobalXThreadStorageS6().PurgeAll(); }
#endif
#if BenchStrS7 == ON
template <> inline void StrBenchPurgePools<BenchS7>() { benchpool::GlobalXThreadStorageS7().PurgeAll(); }
#endif
} // namespace jc_gbench

#if StringThreadBenchTest == ON

// B12/B13 공용 SPSC 채널. producer가 만들고 consumer가 소멸한다.
template <typename TString>
struct BenchSpsc
{
	static constexpr int kCap = 64;
	TString slots_[kCap];
	std::atomic<long> write_{ 0 };
	std::atomic<long> read_{ 0 };

	void Push(TString&& _item)
	{
		while (write_.load(std::memory_order_acquire) - read_.load(std::memory_order_acquire) >= kCap) {}
		slots_[write_.load(std::memory_order_relaxed) % kCap] = std::move(_item);
		write_.fetch_add(1, std::memory_order_release);
	}

	TString Pop()
	{
		while (read_.load(std::memory_order_acquire) >= write_.load(std::memory_order_acquire)) {}
		TString item = std::move(slots_[read_.load(std::memory_order_relaxed) % kCap]);
		read_.fetch_add(1, std::memory_order_release);
		return item;
	}
};

// B11: 스레드별 독립 생성/소멸 (TLS pool 무락 이점의 기준선)
template <typename TString>
static void BM_String_MTIndependent(State& state)
{
	const StrBenchData d = GetStrBenchData(3);
	for (auto _ : state)
	{
		TString s(d.p);
		TString t(s);
		TString u(static_cast<TString&&>(t));
		DoNotOptimize(u.Source());
		ClobberMemory();
	}
}

#if BenchStrB11 == ON
STR_REG_R0(BM_String_MTIndependent, Threads(1)->Threads(4)->Threads(8)->UseRealTime()->Unit(benchmark::TimeUnit::kNanosecond));
STR_REG_S0(BM_String_MTIndependent, Threads(1)->Threads(4)->Threads(8)->UseRealTime()->Unit(benchmark::TimeUnit::kNanosecond));
STR_REG_S1(BM_String_MTIndependent, Threads(1)->Threads(4)->Threads(8)->UseRealTime()->Unit(benchmark::TimeUnit::kNanosecond));
STR_REG_S2(BM_String_MTIndependent, Threads(1)->Threads(4)->Threads(8)->UseRealTime()->Unit(benchmark::TimeUnit::kNanosecond));
STR_REG_S3(BM_String_MTIndependent, Threads(1)->Threads(4)->Threads(8)->UseRealTime()->Unit(benchmark::TimeUnit::kNanosecond));
STR_REG_S4(BM_String_MTIndependent, Threads(1)->Threads(4)->Threads(8)->UseRealTime()->Unit(benchmark::TimeUnit::kNanosecond));
STR_REG_S5(BM_String_MTIndependent, Threads(1)->Threads(4)->Threads(8)->UseRealTime()->Unit(benchmark::TimeUnit::kNanosecond));
STR_REG_S6(BM_String_MTIndependent, Threads(1)->Threads(4)->Threads(8)->UseRealTime()->Unit(benchmark::TimeUnit::kNanosecond));
STR_REG_S7(BM_String_MTIndependent, Threads(1)->Threads(4)->Threads(8)->UseRealTime()->Unit(benchmark::TimeUnit::kNanosecond));
STR_REG_R1(BM_String_MTIndependent, Threads(1)->Threads(4)->Threads(8)->UseRealTime()->Unit(benchmark::TimeUnit::kNanosecond));
#endif

// B12: producer 스레드가 생성 → SPSC 전달 → consumer 스레드가 소멸.
// 워커 스레드는 전체 반복 동안 유지해서 pool이 워밍된 정상 상태를 잰다.
template <typename TString>
static void BM_String_CrossThread(State& state)
{
	const StrBenchData d = GetStrBenchData(3);
	const int n = (int)state.range(0);

	BenchSpsc<TString> ch;
	std::atomic<int> gen{ 0 };
	std::atomic<bool> stop{ false };
	std::atomic<bool> prodDone{ false }, consDone{ false };

	std::thread producer([&] {
		int myGen = 0;
		while (!stop.load(std::memory_order_acquire))
		{
			int g = gen.load(std::memory_order_acquire);
			while (g == myGen && !stop.load(std::memory_order_acquire)) { g = gen.load(std::memory_order_acquire); }
			if (stop.load(std::memory_order_acquire)) break;
			myGen = g;
			for (int i = 0; i < n; ++i)
			{
				TString s(d.p);
				ch.Push(std::move(s));
			}
			StrBenchPumpPool<TString>();
			prodDone.store(true, std::memory_order_release);
		}
	});
	std::thread consumer([&] {
		int myGen = 0;
		while (!stop.load(std::memory_order_acquire))
		{
			int g = gen.load(std::memory_order_acquire);
			while (g == myGen && !stop.load(std::memory_order_acquire)) { g = gen.load(std::memory_order_acquire); }
			if (stop.load(std::memory_order_acquire)) break;
			myGen = g;
			for (int i = 0; i < n; ++i)
			{
				TString s = ch.Pop();
				DoNotOptimize(s.Source());
			}
			consDone.store(true, std::memory_order_release);
		}
	});

	for (auto _ : state)
	{
		prodDone.store(false, std::memory_order_relaxed);
		consDone.store(false, std::memory_order_relaxed);
		gen.fetch_add(1, std::memory_order_release);
		while (!prodDone.load(std::memory_order_acquire) || !consDone.load(std::memory_order_acquire)) {}
		StrBenchDrainPools<TString>();
	}
	state.SetItemsProcessed((int64_t)state.iterations() * n);

	stop.store(true, std::memory_order_release);
	producer.join();
	consumer.join();
}

// B13: owner 스레드가 생성 후 종료 → 메인 스레드가 나중에 소멸.
// 내용물 일치 여부로 cross-thread 생존 정확성도 함께 확인한다.
template <typename TString>
static void BM_String_OwnerExit(State& state)
{
	const StrBenchData d = GetStrBenchData(3);
	const int n = (int)state.range(0);

	for (auto _ : state)
	{
		state.PauseTiming();
		std::vector<TString> v;
		v.reserve(n);
		std::thread owner([&] {
			for (int i = 0; i < n; ++i)
			{
				v.emplace_back(d.p);
			}
		});
		owner.join();
		if (v.size() != (size_t)n || v[0].Compare(d.p) != 0)
		{
			state.SkipWithError("owner-exit 문자열 내용 불일치");
			break;
		}
		state.ResumeTiming();

		v.clear();
		v.shrink_to_fit();
		StrBenchPurgePools<TString>();
	}
	state.SetItemsProcessed((int64_t)state.iterations() * n);
}

#if BenchStrB12 == ON
STR_REG_R0(BM_String_CrossThread, Arg(1000));
STR_REG_S0(BM_String_CrossThread, Arg(1000));
STR_REG_S1(BM_String_CrossThread, Arg(1000));
STR_REG_S2(BM_String_CrossThread, Arg(1000));
STR_REG_S3(BM_String_CrossThread, Arg(1000));
STR_REG_S4(BM_String_CrossThread, Arg(1000));
STR_REG_S5(BM_String_CrossThread, Arg(1000));
STR_REG_S6(BM_String_CrossThread, Arg(1000));
STR_REG_S7(BM_String_CrossThread, Arg(1000));
STR_REG_R1(BM_String_CrossThread, Arg(1000));
#endif

#if BenchStrB13 == ON
STR_REG_R0(BM_String_OwnerExit, Arg(1000));
STR_REG_S0(BM_String_OwnerExit, Arg(1000));
STR_REG_S1(BM_String_OwnerExit, Arg(1000));
STR_REG_S2(BM_String_OwnerExit, Arg(1000));
STR_REG_S3(BM_String_OwnerExit, Arg(1000));
STR_REG_S4(BM_String_OwnerExit, Arg(1000));
STR_REG_S5(BM_String_OwnerExit, Arg(1000));
STR_REG_S6(BM_String_OwnerExit, Arg(1000));
STR_REG_S7(BM_String_OwnerExit, Arg(1000));
STR_REG_R1(BM_String_OwnerExit, Arg(1000));
#endif

#endif
