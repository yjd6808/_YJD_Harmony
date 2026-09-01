/*
 * 작성자: 윤정도
 * 생성일: 8/23/2026
 * =====================
 * jc::MemoryStream 단위 테스트 — 로더 메모리 모드의 수신 버퍼 지지율 검증
 */

#include "jc/Container/MemoryStream.h"

namespace {
inline _byte Pattern(_s64 _i) { return (_byte)(_i % 251); }
inline bool VerifyPatternBytes(const _byte* _p, _s64 _len, _s64 _startOffset = 0) {
 for (_s64 i = 0; i < _len; ++i) if (_p[i] != Pattern(_startOffset + i)) return false; return true;
}
}




#if TEST_MemoryStreamTest == ON

TEST(MemoryStreamTest, WriteThenReadSequential) {
	LeakCheck;

	MemoryStream stream(1024);

	// 패턴 10,000바이트를 3회에 나눠 기록
	_byte src[10000];
	for (int i = 0; i < 10000; ++i) src[i] = Pattern(i);
	stream.Write(src, 3000);
	stream.Write(src + 3000, 3000);
	stream.Write(src + 6000, 4000);
	EXPECT_EQ(stream.GetSize(), 10000);

	// GetData 직접 검증 (메모리 로드 결과 소비자가 쓰는 경로)
	EXPECT_TRUE(VerifyPatternBytes(stream.GetData(), stream.GetSize()));

	// 순차 Read 경로 검증
	_byte dst[4096];
	_s64 total = 0;
	for (;;)
	{
		const int n = stream.Read(dst, (int)sizeof(dst));
		if (n <= 0) break;
		EXPECT_TRUE(VerifyPatternBytes(dst, n, total));
		total += n;
	}
	EXPECT_EQ(total, 10000);
}

TEST(MemoryStreamTest, GrowthPreservesData) {
	LeakCheck;

	MemoryStream stream(16);                             // 일부러 작은 초기 용량 — 재할당 다수 유발
	const int kTotal = 100 * 1024;
	_byte chunk[257];                                    // 용량 경계와 어긋나는 홀수 크기
	int written = 0;
	while (written < kTotal)
	{
		const int n = Math::Min((int)sizeof(chunk), kTotal - written);
		for (int i = 0; i < n; ++i) chunk[i] = Pattern(written + i);
		stream.Write(chunk, n);
		written += n;
	}

	EXPECT_EQ(stream.GetSize(), kTotal);
	EXPECT_TRUE(VerifyPatternBytes(stream.GetData(), kTotal));   // 재할당을 거쳐도 선두 데이터 보존
}

TEST(MemoryStreamTest, ReadPastEnd) {
	LeakCheck;

	MemoryStream stream(64);
	_byte src[100];
	for (int i = 0; i < 100; ++i) src[i] = Pattern(i);
	stream.Write(src, 100);

	_byte dst[200];
	EXPECT_EQ(stream.Read(dst, 200), 100);               // 남은 만큼만
	EXPECT_EQ(stream.Read(dst, 200), 0);                 // EOF
}

TEST(MemoryStreamTest, NonOwnerWrap) {
	LeakCheck;

	_byte external[256]{};
	{
		MemoryStream stream(external, sizeof(external), false);   // owner_ = false
		_byte src[100];
		for (int i = 0; i < 100; ++i) src[i] = Pattern(i);
		stream.Write(src, 100);
		EXPECT_EQ(stream.GetData(), external);           // 외부 버퍼 그대로 사용
	}
	// 스트림 소멸 후에도 외부 버퍼 유효 + 내용 유지 (owner=false는 해제 금지)
	EXPECT_TRUE(VerifyPatternBytes(external, 100));
}

TEST(MemoryStreamTest, SharedPtrLifetime) {
	LeakCheck;

	MemoryStreamPtr spKeep;
	{
		MemoryStreamPtr spData = MakeShared<MemoryStream>(1024);
		_byte src[10];
		for (int i = 0; i < 10; ++i) src[i] = Pattern(i);
		spData->Write(src, 10);
		spKeep = spData;                                 // 소유권 공유 — TakeData 이후 사용자 보관 시나리오와 동일
	}
	EXPECT_EQ(spKeep->GetSize(), 10);
	EXPECT_TRUE(VerifyPatternBytes(spKeep->GetData(), 10));
	// 스코프 종료 시 단일 해제 — LeakCheck가 이중 해제/누수 모두 검출
}

TEST(MemoryStreamTest, LargePayload) {
	LeakCheck;

	const int kTotal = 4 * 1024 * 1024;
	MemoryStream stream(1024);
	_byte chunk[65536];
	int written = 0;
	while (written < kTotal)
	{
		const int n = Math::Min((int)sizeof(chunk), kTotal - written);
		for (int i = 0; i < n; ++i) chunk[i] = Pattern(written + i);
		stream.Write(chunk, n);
		written += n;
	}
	EXPECT_EQ(stream.GetSize(), kTotal);
	EXPECT_TRUE(VerifyPatternBytes(stream.GetData(), kTotal));
}

#endif // TEST_MemoryStreamTest == ON
