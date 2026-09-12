/*
 * 작성자: 윤정도
 * 생성일: 9/6/2026
 * =====================
 * StringImpl_SSO<CharT> Storage 단독 테스트 (24B inline SSO, S1 계열)
 */

#include "jc/Primitives/StringImpl_SSO.h"

#include <thread>
#include <vector>
#include <string>

using namespace std;

#if TEST_StringImpl_SSOTest == ON

using TSsoStorage = StringImpl_SSO<char>;
using TSsoStorageW = StringImpl_SSO<wchar_t>;

TEST(StringImpl_SSOTest, Capacity) {
	TSsoStorage s;
	EXPECT_EQ(s.Len(), 0);
	EXPECT_EQ(s.Cap(), 22);
	EXPECT_EQ(s.Data()[0], '\0');

	TSsoStorageW w;
	EXPECT_EQ(w.Len(), 0);
	EXPECT_EQ(w.Cap(), 10);
	EXPECT_EQ(w.Data()[0], L'\0');
}

TEST(StringImpl_SSOTest, SsoRoundtrip) {
	LeakCheck;

	TSsoStorage s;
	s.Assign("hello", 5);
	EXPECT_EQ(s.Len(), 5);
	EXPECT_EQ(s.Cap(), 22);
	EXPECT_STREQ(s.Data(), "hello");

	TSsoStorage copied(s);
	EXPECT_STREQ(copied.Data(), "hello");

	TSsoStorage moved(std::move(s));
	EXPECT_STREQ(moved.Data(), "hello");
	EXPECT_EQ(s.Len(), 0);

	TSsoStorage assigned;
	assigned = copied;
	EXPECT_STREQ(assigned.Data(), "hello");

	TSsoStorage swapped;
	swapped.Assign("world!", 6);
	swapped = std::move(assigned);			// 이동 대입 (steal. 원본은 비운다)
	EXPECT_STREQ(swapped.Data(), "hello");
	EXPECT_EQ(assigned.Len(), 0);

	moved.Data()[3] = '\0';					// NUL 종결은 호출자 책임. SetLen은 길이만 둔다.
	moved.SetLen(3);
	EXPECT_STREQ(moved.Data(), "hel");

	moved = std::move(moved);				// self-move: no-op (swap과 동일)
	EXPECT_STREQ(moved.Data(), "hel");

	moved.Clear();
	EXPECT_EQ(moved.Len(), 0);
	EXPECT_EQ(moved.Cap(), 22);
}

TEST(StringImpl_SSOTest, HeapRoundtrip) {
	LeakCheck;

	const string big(100, 'x');
	TSsoStorage s;
	s.Assign(big.c_str(), 100);				// SSO 22 초과 → 힙
	EXPECT_EQ(s.Len(), 100);
	EXPECT_GE(s.Cap(), 100);
	EXPECT_STREQ(s.Data(), big.c_str());

	s.Reserve(200);
	EXPECT_STREQ(s.Data(), big.c_str());
	EXPECT_GE(s.Cap(), 200);

	TSsoStorage copied(s);
	EXPECT_STREQ(copied.Data(), big.c_str());
	EXPECT_NE(copied.Data(), s.Data());

	TSsoStorage moved(std::move(s));
	EXPECT_STREQ(moved.Data(), big.c_str());
	EXPECT_EQ(s.Len(), 0);
}

TEST(StringImpl_SSOTest, MarkTransition) {
	LeakCheck;

	TSsoStorage s;
	const string s22(22, 'a');
	s.Assign(s22.c_str(), 22);				// 경계 안 → SSO 유지
	EXPECT_EQ(s.Cap(), 22);
	EXPECT_STREQ(s.Data(), s22.c_str());

	const string s23(23, 'b');
	s.Assign(s23.c_str(), 23);				// 경계 초과 → 힙 승급
	EXPECT_GE(s.Cap(), 23);
	EXPECT_STREQ(s.Data(), s23.c_str());

	s.Clear();								// 힙 해제 → SSO 복귀
	EXPECT_EQ(s.Len(), 0);
	EXPECT_EQ(s.Cap(), 22);

	s.Assign("z", 1);
	EXPECT_STREQ(s.Data(), "z");
}

TEST(StringImpl_SSOTest, OverlapAssign) {
	LeakCheck;

	TSsoStorage s;
	s.Assign("abcdefghij", 10);
	s.Assign(s.Data(), s.Len());
	EXPECT_STREQ(s.Data(), "abcdefghij");

	s.Assign(s.Data() + 3, 7);
	EXPECT_STREQ(s.Data(), "defghij");

	const string big(100, 'y');
	s.Assign(big.c_str(), 100);
	s.Assign(s.Data() + 90, 10);			// 힙 자기 substring (임시 경유)
	EXPECT_EQ(s.Len(), 10);
	EXPECT_EQ(s.Data()[0], 'y');
}

TEST(StringImpl_SSOTest, Adopt) {
	LeakCheck;

	constexpr int kLen = 60;
	char* pBuf = Memory::Allocate<char*>(kLen + 1);
	memcpy(pBuf, string(kLen, 'z').c_str(), kLen + 1);

	TSsoStorage s;
	s.Adopt(pBuf, kLen, kLen);
	EXPECT_EQ(s.Len(), kLen);
	EXPECT_EQ(s.Data()[0], 'z');

	char* pSmall = Memory::Allocate<char*>(6);
	memcpy(pSmall, "hello", 6);
	s.Adopt(pSmall, 5, 5);					// SSO 가능 → 복사 후 버퍼 해제
	EXPECT_STREQ(s.Data(), "hello");
	EXPECT_EQ(s.Cap(), 22);
}

TEST(StringImpl_SSOTest, WideBoundary) {
	LeakCheck;

	TSsoStorageW s;
	const wstring w10(10, L'k');
	s.Assign(w10.c_str(), 10);
	EXPECT_EQ(s.Len(), 10);
	EXPECT_EQ(s.Cap(), 10);
	EXPECT_STREQ(s.Data(), w10.c_str());

	const wstring w11(11, L'k');
	s.Assign(w11.c_str(), 11);				// SSO 초과 → 힙
	EXPECT_EQ(s.Len(), 11);
	EXPECT_GE(s.Cap(), 11);
	EXPECT_STREQ(s.Data(), w11.c_str());

	TSsoStorageW copied(s);
	EXPECT_STREQ(copied.Data(), w11.c_str());
}

TEST(StringImpl_SSOTest, IndependentThreads) {
	LeakCheck;

	// 공유 상태가 없으므로 스레드마다 독립 Storage로 동작한다. 4 스레드.
	vector<thread> workers;
	for (int i = 0; i < 4; ++i) {
		workers.emplace_back([i] {
			TSsoStorage s;
			const string body(50 + i, static_cast<char>('a' + i));
			s.Assign(body.c_str(), static_cast<int>(body.size()));
			TSsoStorage moved(std::move(s));
			if (moved.Len() != static_cast<int>(body.size())) {
				FAIL() << "len mismatch";
			}
		});
	}
	for (thread& w : workers) {
		w.join();
	}
}

#endif
