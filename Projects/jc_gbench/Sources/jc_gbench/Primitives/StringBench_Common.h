/*
 * StringBench_Common.h: 시나리오 공통 템플릿용 데이터셋과 std::string 어댑터이다.
 * 벤치 템플릿은 jc::String의 공개 API(Source/Length/Append/비교)로 작성한다.
 */

#pragma once

#include <string>
#include <cstring>
#include <cstdio>
#include <cstdarg>

#include "jc/Primitives/String.h"
#include "jc_gbench/Primitives/StrForBench.h"

namespace jc_gbench {

// 벤치 대상 별칭: R0(원본), S0(복사본), S1(inline SSO), S2(Impl 포인터),
// S3(Impl + TLS pool), S4(pool + Impl SSO), R1(std::string 참조군)
using BenchR0 = jc::String;
#if BenchStrS0 == ON
using BenchS0 = s0::StrForBench;
#endif
#if BenchStrS1 == ON
using BenchS1 = s1::StrForBench;
#endif
#if BenchStrS2 == ON
using BenchS2 = s2::StrForBench;
#endif
#if BenchStrS3 == ON
using BenchS3 = s3::StrForBench;
#endif
#if BenchStrS4 == ON
using BenchS4 = s4::StrForBench;
#endif
#if BenchStrS5 == ON
using BenchS5 = s5::StrForBench;
#endif
#if BenchStrS6 == ON
using BenchS6 = s6::StrForBench;
#endif
#if BenchStrS7 == ON
using BenchS7 = s7::StrForBench;
#endif

// 후보 ON/OFF에 따라 BENCHMARK_TEMPLATE 등록을 켜고 끈다.
// 용법: STR_REG_S1(BM_String_Construct, DenseRange(0, 6)->Unit(TimeUnit::kNanosecond));
#if BenchStrR0 == ON
#define STR_REG_R0(BM, CHAIN) BENCHMARK_TEMPLATE(BM, BenchR0)->CHAIN
#else
#define STR_REG_R0(BM, CHAIN)
#endif
#if BenchStrS0 == ON
#define STR_REG_S0(BM, CHAIN) BENCHMARK_TEMPLATE(BM, BenchS0)->CHAIN
#else
#define STR_REG_S0(BM, CHAIN)
#endif
#if BenchStrS1 == ON
#define STR_REG_S1(BM, CHAIN) BENCHMARK_TEMPLATE(BM, BenchS1)->CHAIN
#else
#define STR_REG_S1(BM, CHAIN)
#endif
#if BenchStrS2 == ON
#define STR_REG_S2(BM, CHAIN) BENCHMARK_TEMPLATE(BM, BenchS2)->CHAIN
#else
#define STR_REG_S2(BM, CHAIN)
#endif
#if BenchStrS3 == ON
#define STR_REG_S3(BM, CHAIN) BENCHMARK_TEMPLATE(BM, BenchS3)->CHAIN
#else
#define STR_REG_S3(BM, CHAIN)
#endif
#if BenchStrS4 == ON
#define STR_REG_S4(BM, CHAIN) BENCHMARK_TEMPLATE(BM, BenchS4)->CHAIN
#else
#define STR_REG_S4(BM, CHAIN)
#endif
#if BenchStrS5 == ON
#define STR_REG_S5(BM, CHAIN) BENCHMARK_TEMPLATE(BM, BenchS5)->CHAIN
#else
#define STR_REG_S5(BM, CHAIN)
#endif
#if BenchStrS6 == ON
#define STR_REG_S6(BM, CHAIN) BENCHMARK_TEMPLATE(BM, BenchS6)->CHAIN
#else
#define STR_REG_S6(BM, CHAIN)
#endif
#if BenchStrS7 == ON
#define STR_REG_S7(BM, CHAIN) BENCHMARK_TEMPLATE(BM, BenchS7)->CHAIN
#else
#define STR_REG_S7(BM, CHAIN)
#endif
#if BenchStrR1 == ON
#define STR_REG_R1(BM, CHAIN) BENCHMARK_TEMPLATE(BM, BenchR1)->CHAIN
#else
#define STR_REG_R1(BM, CHAIN)
#endif

// std::string을 jc::String 공개 API에 맞춘 얇은 어댑터
struct StdStr
{
	std::string s_;

	StdStr() = default;
	StdStr(const char* _p) : s_(_p != nullptr ? _p : "") {}
	StdStr(const StdStr& _o) = default;
	StdStr(StdStr&& _o) noexcept = default;
	~StdStr() = default;

	char* Source() { return s_.data(); }
	const char* Source() const { return s_.c_str(); }
	const char* SafeSource() const { return s_.c_str(); }
	int Length() const { return (int)s_.size(); }
	int Capacity() const { return (int)s_.capacity(); }

	void Append(char _ch) { s_.push_back(_ch); }
	void Append(const char* _p) { if (_p) s_.append(_p); }
	void Append(const char* _p, int _len) { if (_p && _len > 0) s_.append(_p, _len); }
	void Append(const StdStr& _o) { s_.append(_o.s_); }

	int Compare(const StdStr& _o) const { return s_.compare(_o.s_); }
	int Compare(const char* _p) const { return s_.compare(_p != nullptr ? _p : ""); }
	int Find(const char* _p) const
	{
		if (!_p || !*_p)
			return -1;
		const size_t pos = s_.find(_p);
		return pos == std::string::npos ? -1 : (int)pos;
	}

	void Format(const char* _format, ...)
	{
		va_list args;
		va_start(args, _format);
		va_list copied;
		va_copy(copied, args);
		const int len = vsnprintf(nullptr, 0, _format, copied);
		va_end(copied);
		if (len > 0)
		{
			s_.resize(len);
			vsnprintf(s_.data(), len + 1, _format, args);
		}
		va_end(args);
	}

	StdStr operator+(const StdStr& _o) const { StdStr t(*this); t.s_.append(_o.s_); return t; }
	StdStr operator+(const char* _p) const { StdStr t(*this); t.Append(_p); return t; }
	StdStr& operator+=(const StdStr& _o) { s_.append(_o.s_); return *this; }
	StdStr& operator+=(const char* _p) { Append(_p); return *this; }
	StdStr& operator=(const StdStr& _o) = default;
	StdStr& operator=(StdStr&& _o) noexcept = default;
	StdStr& operator=(const char* _p) { s_.assign(_p != nullptr ? _p : ""); return *this; }

	bool operator==(const StdStr& _o) const { return s_ == _o.s_; }
	bool operator==(const char* _p) const { return s_ == (_p != nullptr ? _p : ""); }
	bool operator<(const StdStr& _o) const { return s_ < _o.s_; }
	bool operator<(const char* _p) const { return s_ < (_p != nullptr ? _p : ""); }
};

using BenchR1 = StdStr;

// 데이터셋: 0 Empty, 1 Tiny(7B), 2 Short(22B), 3 Mid(40B), 4 Long(128B), 5 Huge(4096B), 6 Utf8Mixed
struct StrBenchData
{
	const char* p;
	int len;
};

inline StrBenchData GetStrBenchData(int _idx)
{
	static const std::string kLong(128, 'x');
	static const std::string kHuge(4096, 'x');
	static const std::string kUtf8(reinterpret_cast<const char*>(u8"나마스떼 नमस्ते 🙏_0123456789_ABCDEF"));

	switch (_idx)
	{
	case 0:  return { "", 0 };
	case 1:  return { "abcdefg", 7 };
	case 2:  return { "PlayerCharacter_01_abc", 22 };
	case 3:  return { "0123456789012345678901234567890123456789", 40 };
	case 4:  return { kLong.c_str(), (int)kLong.size() };
	case 5:  return { kHuge.c_str(), (int)kHuge.size() };
	case 6:  return { kUtf8.c_str(), (int)kUtf8.size() };
	default: return { "abcdefg", 7 };
	}
}

} // namespace jc_gbench
