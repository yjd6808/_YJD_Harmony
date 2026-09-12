/*
 * 작성자: 윤정도
 * 생성일: 9/6/2026
 * =====================
 * StringImpl_SSOCtxTlsPool<CharT> Storage 단독 테스트 (BasicString 없이 Storage 직접 검증)
 */

#include "jc/Primitives/StringImpl_SSOCtxTlsPool.h"

#include <thread>
#include <vector>
#include <string>
#include <future>

using namespace std;

#if TEST_StringImpl_SSOCtxTlsPoolTest == ON

using TPooledStorage = StringImpl_SSOCtxTlsPool<char>;
using TPooledStorageW = StringImpl_SSOCtxTlsPool<wchar_t>;
using TSlotPool = ObjectPoolTLSIndexed<StringSlot<char>>;
using TSlotPoolW = ObjectPoolTLSIndexed<StringSlot<wchar_t>>;

// 슬롯 풀도 영구 객체라 워밍업한다. char/wchar_t 풀은 별도 인스턴스라 둘 다 예열한다.
// 워커 스레드용 여유 고아 풀도 미리 만든다. 없으면 워커가 CreatePool(영구 128B)해서 LeakCheck에 걸린다.
inline void WarmupStorageOnce()
{
	static const bool warmed = [] {
		for (int i = 0; i < 4; ++i)
		{
			StringSlot<char>* pA = TSlotPool::Pop();
			TSlotPool::Push(pA);
			StringSlot<wchar_t>* pW = TSlotPoolW::Pop();
			TSlotPoolW::Push(pW);
		}
		for (int i = 0; i < 2; ++i)
		{
			thread spare([] {
				StringSlot<char>* pSpare = TSlotPool::Pop();
				TSlotPool::Push(pSpare);
			});
			spare.join();
		}
		// 워밍업이 만든 슬롯 잔여물을 여기서 비운다. 안 그러면 첫 테스트의
		// LeakCheck 델타가 음수로 잡혀(체크포인트 이전 할당을 테스트 안에서 해제) 실패한다.
		TSlotPool::TrimThisThread();
		TSlotPoolW::TrimThisThread();
		return true;
	}();
	(void)warmed;
}

// Pop해온 슬롯은 스레드 풀 pFree_에 영구 보관된다. 테스트 끝마다 비워야 LeakCheck 델타가 0이다.
// 파괴 순서가 핵심이다. Storage 객체들보다 늦게, LeakCheck보다 먼저 파괴되어야 해서 RAII 가드로 둔다.
struct TrimGuard
{
	~TrimGuard()
	{
		TSlotPool::TrimThisThread();
		TSlotPoolW::TrimThisThread();
	}
};

TEST(StringImpl_SSOCtxTlsPoolTest, EmptyAcquiresNothing)
{
	WarmupStorageOnce();
	LeakCheck;
	TrimGuard _trim;

	const _u64 totalBefore = TSlotPool::GetStats().totalCount;
	{
		vector<TPooledStorage> empties(10000);
		for (const TPooledStorage& s : empties)
		{
			EXPECT_EQ(s.Len(), 0);
			EXPECT_EQ(s.Cap(), 0);
			EXPECT_EQ(s.Data()[0], '\0');
		}
	}
	EXPECT_EQ(TSlotPool::GetStats().totalCount, totalBefore);	// 빈 문자열은 풀을 안 건듦
}

TEST(StringImpl_SSOCtxTlsPoolTest, SsoRoundtrip)
{
	WarmupStorageOnce();
	LeakCheck;
	TrimGuard _trim;

	TPooledStorage s;
	s.Assign("hello", 5);
	EXPECT_EQ(s.Len(), 5);
	EXPECT_EQ(s.Cap(), 43);
	EXPECT_STREQ(s.Data(), "hello");

	TPooledStorage copied(s);				// 복사 생성
	EXPECT_STREQ(copied.Data(), "hello");

	TPooledStorage moved(std::move(s));		// 이동 생성
	EXPECT_STREQ(moved.Data(), "hello");
	EXPECT_EQ(s.Len(), 0);					// 원본은 비게 된다

	TPooledStorage assigned;
	assigned = copied;						// 복사 대입
	EXPECT_STREQ(assigned.Data(), "hello");

	TPooledStorage swapped;
	swapped.Assign("world!", 6);
	swapped = std::move(assigned);			// 이동 대입 (swap. 풀 트래픽 0. 원본이 이전 값을 물려받음)
	EXPECT_STREQ(swapped.Data(), "hello");
	EXPECT_STREQ(assigned.Data(), "world!");

	moved.Data()[3] = '\0';					// NUL 종결은 호출자 책임. SetLen은 길이만 둔다.
	moved.SetLen(3);
	EXPECT_STREQ(moved.Data(), "hel");

	moved.Clear();							// 슬롯 유지
	EXPECT_EQ(moved.Len(), 0);
	EXPECT_EQ(moved.Cap(), 43);
}

TEST(StringImpl_SSOCtxTlsPoolTest, HeapRoundtrip)
{
	WarmupStorageOnce();
	LeakCheck;
	TrimGuard _trim;

	const string big(100, 'x');
	TPooledStorage s;
	s.Assign(big.c_str(), 100);				// SSO 43 초과 → 힙
	EXPECT_EQ(s.Len(), 100);
	EXPECT_GE(s.Cap(), 100);
	EXPECT_STREQ(s.Data(), big.c_str());

	s.Reserve(200);							// 확장해도 내용 유지
	EXPECT_STREQ(s.Data(), big.c_str());
	EXPECT_GE(s.Cap(), 200);

	TPooledStorage copied(s);				// 힙 깊은 복사
	EXPECT_STREQ(copied.Data(), big.c_str());
	EXPECT_NE(copied.Data(), s.Data());		// 버퍼는 다르다

	TPooledStorage moved(std::move(s));
	EXPECT_STREQ(moved.Data(), big.c_str());
	EXPECT_EQ(s.Len(), 0);
}

TEST(StringImpl_SSOCtxTlsPoolTest, OverlapAssign)
{
	WarmupStorageOnce();
	LeakCheck;
	TrimGuard _trim;

	TPooledStorage s;
	s.Assign("abcdefghij", 10);
	s.Assign(s.Data(), s.Len());			// 전체 자기 대입
	EXPECT_STREQ(s.Data(), "abcdefghij");

	s.Assign(s.Data() + 3, 7);				// 자기 substring (overlap 경로)
	EXPECT_STREQ(s.Data(), "defghij");
	EXPECT_EQ(s.Len(), 7);

	const string big(100, 'y');
	s.Assign(big.c_str(), 100);				// 힙 상태에서 자기 substring
	s.Assign(s.Data() + 90, 10);
	EXPECT_EQ(s.Len(), 10);
	EXPECT_EQ(s.Data()[0], 'y');
}

TEST(StringImpl_SSOCtxTlsPoolTest, Adopt)
{
	WarmupStorageOnce();
	LeakCheck;
	TrimGuard _trim;

	constexpr int kLen = 60;
	char* pBuf = Memory::Allocate<char*>(kLen + 1);
	memcpy(pBuf, string(kLen, 'z').c_str(), kLen + 1);

	TPooledStorage s;
	s.Adopt(pBuf, kLen, kLen);				// 소유권 인수. 복사 없음.
	EXPECT_EQ(s.Len(), kLen);
	EXPECT_EQ(s.Data()[0], 'z');
	// 소멸 시 힙 버퍼 + 슬롯 정리. LeakCheck가 릭을 잡는다.
}

TEST(StringImpl_SSOCtxTlsPoolTest, CrossThreadDestroy)
{
	WarmupStorageOnce();
	LeakCheck;
	TrimGuard _trim;

	promise<TPooledStorage*> made;
	thread maker([&made] {
		TPooledStorage* pStorage = new TPooledStorage();
		const string big(4096, 'q');
		pStorage->Assign(big.c_str(), 4096);
		made.set_value(pStorage);
	});
	TPooledStorage* pStorage = made.get_future().get();
	maker.join();							// owner 종료. 슬롯 풀은 고아.

	TPooledStorage owned(std::move(*pStorage));
	delete pStorage;
	EXPECT_EQ(owned.Len(), 4096);
	EXPECT_EQ(owned.Data()[0], 'q');
	// owned 소멸: 힙 버퍼 해제 → 고아 슬롯 힙 반납 (§9.4 2단계). 크래시·누수 없어야 한다.
}

TEST(StringImpl_SSOCtxTlsPoolTest, WideBoundary)
{
	WarmupStorageOnce();
	LeakCheck;
	TrimGuard _trim;

	TPooledStorageW s;
	EXPECT_EQ(s.Cap(), 0);

	const wstring w21(21, L'k');
	s.Assign(w21.c_str(), 21);				// SSO 경계 안
	EXPECT_EQ(s.Len(), 21);
	EXPECT_EQ(s.Cap(), 21);
	EXPECT_STREQ(s.Data(), w21.c_str());

	const wstring w22(22, L'k');
	s.Assign(w22.c_str(), 22);				// SSO 초과 → 힙
	EXPECT_EQ(s.Len(), 22);
	EXPECT_GE(s.Cap(), 22);
	EXPECT_STREQ(s.Data(), w22.c_str());

	TPooledStorageW copied(s);
	EXPECT_STREQ(copied.Data(), w22.c_str());
}

#endif
