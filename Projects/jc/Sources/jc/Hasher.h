/*
	작성자 : 윤정도
	해쉬 생성기 — 알고리즘 탑재형
*/

#pragma once

#include "jc/Type.h"
#include "jc/Primitives/String.h"
#include "jc/Primitives/StringUtil.h"
#include "jc/TypeTraits.h"

#pragma warning (push)
#pragma warning (disable : 4244)

NS_JC_BEGIN

constexpr _u32 PrimeInt32U_v = 0x087b840FU;
constexpr _u64 PrimeInt64U_v = 0x0000050B00000002ULL;
constexpr _u64 HashXorKey32U_v = 0x3e4dc77d;
constexpr _u64 HashXorKey64U_v = 0x000009866a1bc6b9;

namespace HashAlgorithm
{
	struct SimplePrime32
	{
		using Result = _u32;
		static constexpr Result kPrime = PrimeInt32U_v;
		static constexpr Result kXor = static_cast<Result>(HashXorKey32U_v);
		static constexpr Result kOffset = PrimeInt32U_v;
		static constexpr Result kBasis = PrimeInt32U_v;

		static constexpr Result Combine(Result _cur, _u8 _byte) noexcept
		{
			return ( _cur ^ static_cast<Result>(_byte ^ static_cast<_u8>(kXor & 0xFF)) ) * kPrime;
		}

		static constexpr Result HashIntegral(_u64 _val) noexcept
		{
			return static_cast<Result>((( _val ^ HashXorKey32U_v) % kPrime) * kPrime);
		}

		static constexpr Result HashBytes(const char* _pStr, size_t _size) noexcept
		{
			Result h = kOffset;
			for (size_t i = 0; i < _size; ++i)
				h = Combine(h, static_cast<_u8>(_pStr[i]));
			return h;
		}

		static constexpr Result HashBytes(const void* _pData, size_t _size) noexcept
		{
			return HashBytes(static_cast<const char*>(_pData), _size);
		}
	};

	struct SimplePrime64
	{
		using Result = _u64;
		static constexpr Result kPrime = PrimeInt64U_v;
		static constexpr Result kXor = HashXorKey64U_v;
		static constexpr Result kOffset = PrimeInt64U_v;
		static constexpr Result kBasis = PrimeInt64U_v;

		static constexpr Result Combine(Result _cur, _u8 _byte) noexcept
		{
			return ( _cur ^ static_cast<Result>(_byte ^ static_cast<_u8>(kXor & 0xFF)) ) * kPrime;
		}

		static constexpr Result HashIntegral(_u64 _val) noexcept
		{
			return ((_val ^ kXor) % kPrime) * kPrime;
		}

		static constexpr Result HashBytes(const char* _pStr, size_t _size) noexcept
		{
			Result h = kOffset;
			for (size_t i = 0; i < _size; ++i)
				h = Combine(h, static_cast<_u8>(_pStr[i]));
			return h;
		}

		static constexpr Result HashBytes(const void* _pData, size_t _size) noexcept
		{
			return HashBytes(static_cast<const char*>(_pData), _size);
		}
	};

	struct Fnv1a32
	{
		using Result = _u32;
		static constexpr Result kOffset = 2166136261u;
		static constexpr Result kBasis = 2166136261u;
		static constexpr Result kPrime = 16777619u;

		static constexpr Result Combine(Result _cur, _u8 _byte) noexcept
		{
			return (_cur ^ _byte) * kPrime;
		}

		static constexpr Result HashBytes(const char* _pStr, size_t _size) noexcept
		{
			Result h = kOffset;
			for (size_t i = 0; i < _size; ++i)
			{
				h ^= static_cast<_u8>(_pStr[i]);
				h *= kPrime;
			}
			return h;
		}

		static constexpr Result HashBytes(const void* _pData, size_t _size) noexcept
		{
			return HashBytes(static_cast<const char*>(_pData), _size);
		}

		static constexpr Result HashIntegral(_u64 _val) noexcept
		{
			return HashBytes(static_cast<const char*>(static_cast<const void*>(&_val)), sizeof(_val));
		}

		static constexpr Result HashString(const char* _str, size_t _len) noexcept
		{
			return HashBytes(_str, _len);
		}
	};

	struct Fnv1a64
	{
		using Result = _u64;
		static constexpr Result kOffset = 14695981039346656037ull;
		static constexpr Result kBasis = 14695981039346656037ull;
		static constexpr Result kPrime = 1099511628211ull;

		static constexpr Result Combine(Result _cur, _u8 _byte) noexcept
		{
			return (_cur ^ _byte) * kPrime;
		}

		static constexpr Result HashBytes(const char* _pStr, size_t _size) noexcept
		{
			Result h = kOffset;
			for (size_t i = 0; i < _size; ++i)
			{
				h ^= static_cast<_u8>(_pStr[i]);
				h *= kPrime;
			}
			return h;
		}

		static constexpr Result HashBytes(const void* _pData, size_t _size) noexcept
		{
			return HashBytes(static_cast<const char*>(_pData), _size);
		}

		static constexpr Result HashIntegral(_u64 _val) noexcept
		{
			return HashBytes(static_cast<const char*>(static_cast<const void*>(&_val)), sizeof(_val));
		}

		static constexpr Result HashString(const char* _str, size_t _len) noexcept
		{
			return HashBytes(_str, _len);
		}
	};
} // namespace HashAlgorithm

// 스트리밍 누적 빌더 — 선언/시그니처 해시 등 다중 필드 누적에 사용
template <typename TAlgo>
class HashBuilder
{
public:
	using Result = typename TAlgo::Result;

	constexpr HashBuilder() noexcept : cur_(TAlgo::kOffset) {}
	explicit constexpr HashBuilder(Result _seed) noexcept : cur_(_seed) {}

	void Append(const void* _pData, size_t _size) noexcept
	{
		const _u8* pBytes = static_cast<const _u8*>(_pData);
		for (size_t i = 0; i < _size; ++i)
		{
			cur_ = TAlgo::Combine(cur_, pBytes[i]);
		}
	}

	template <typename T>
	void AppendValue(const T& _val) noexcept
	{
		Append(&_val, sizeof(T));
	}

	// 결정론적 리틀엔디안 8바이트 분해 — 플랫폼 엔디안 무관, 패딩 배제
	void AppendIntegral(_u64 _val) noexcept
	{
		for (_s32 i = 0; i < 8; ++i)
		{
			cur_ = TAlgo::Combine(cur_, static_cast<_u8>((_val >> (i * 8)) & 0xFF));
		}
	}

	void AppendString(const char* _str, size_t _len) noexcept
	{
		if (_str != nullptr && _len > 0)
			Append(_str, _len);
	}

	void AppendString(const char* _str) noexcept
	{
		if (_str != nullptr)
			Append(_str, jc::StringUtil::Length(_str));
	}

	void AppendString(const String& _str) noexcept
	{
		Append(_str.Source(), _str.Length());
	}

	// 이미 계산된 해시값을 다시 누적 (예: nameHash ^= ... 패턴 대체)
	void CombineHash(Result _hash) noexcept
	{
		// 해시값을 바이트 단위로 재주입 — FNV/Prime 모두에 대해 결정론적 확산
		for (_s32 i = 0; i < static_cast<_s32>(sizeof(Result)); ++i)
		{
			cur_ = TAlgo::Combine(cur_, static_cast<_u8>((_hash >> (i * 8)) & 0xFF));
		}
	}

	Result Digest() const noexcept { return cur_; }
	void Reset() noexcept { cur_ = TAlgo::kOffset; }
	void Reset(Result _seed) noexcept { cur_ = _seed; }

private:
	Result cur_;
};

// ============================================================
// Hasher<T, Algo> — 32비트 기본 (SimplePrime32)
// ============================================================

template <typename T, typename TAlgo = HashAlgorithm::SimplePrime32>
struct Hasher
{
	using Result = typename TAlgo::Result;
	constexpr Result operator()(T _val) const noexcept
	{
		if constexpr (jc::IsFundamentalType_v<T> || jc::IsEnumType_v<T>)
			return TAlgo::HashIntegral(static_cast<_u64>(_val));
		else
			return TAlgo::HashBytes(&_val, sizeof(T));
	}
};

// float / double — 비트 캐스트 후 정수 해시
template <typename TAlgo>
struct Hasher<float, TAlgo>
{
	using Result = typename TAlgo::Result;
	union Bit { float val_{}; _u32 u_; };
	constexpr Result operator()(float _val) const noexcept
	{
		Bit b{}; b.val_ = _val;
		return TAlgo::HashIntegral(b.u_);
	}
};

template <typename TAlgo>
struct Hasher<double, TAlgo>
{
	using Result = typename TAlgo::Result;
	union Bit { double val_{}; _u64 u_; };
	constexpr Result operator()(double _val) const noexcept
	{
		Bit b{}; b.val_ = _val;
		return TAlgo::HashIntegral(b.u_);
	}
};

// 문자열 — 내용 기반
template <typename TAlgo>
struct Hasher<const char*, TAlgo>
{
	using Result = typename TAlgo::Result;

	Result operator()(const String& _val) const noexcept
	{
		return TAlgo::HashBytes(_val.Source(), static_cast<size_t>(_val.Length()));
	}

	constexpr Result operator()(const char* _val) const noexcept
	{
		return _val ? TAlgo::HashBytes(_val, jc::StringUtil::Length(_val)) : TAlgo::kOffset;
	}

	template <_u32 Size>
	constexpr Result operator()(const char (&_val)[Size]) const noexcept
	{
		// 리터럴의 널 종료자 제외 — "abc" (Size=4) → 3바이트만 해시
		size_t len = Size;
		if (len > 0 && _val[len - 1] == '\0')
			--len;
		return TAlgo::HashBytes(_val, len);
	}
};

template <typename T, typename TAlgo>
struct Hasher<T*, TAlgo>
{
	using Result = typename TAlgo::Result;
	constexpr Result operator()(T* _val) const noexcept
	{
		return TAlgo::HashIntegral(reinterpret_cast<_u64>(_val));
	}
};

template <typename T, typename TAlgo>
struct Hasher<T* const, TAlgo>
{
	using Result = typename TAlgo::Result;
	constexpr Result operator()(T* const _val) const noexcept
	{
		return TAlgo::HashIntegral(reinterpret_cast<_u64>(_val));
	}
};

template <typename TAlgo>
struct Hasher<String, TAlgo>
{
	using Result = typename TAlgo::Result;

	Result operator()(const String& _val) const noexcept
	{
		return TAlgo::HashBytes(_val.Source(), static_cast<size_t>(_val.Length()));
	}

	Result operator()(const char* _val) const noexcept
	{
		return _val ? TAlgo::HashBytes(_val, jc::StringUtil::Length(_val)) : TAlgo::kOffset;
	}

	template <_u32 Size>
	constexpr Result operator()(const char (&_val)[Size]) const noexcept
	{
		size_t len = Size;
		if (len > 0 && _val[len - 1] == '\0')
			--len;
		return TAlgo::HashBytes(_val, len);
	}
};

// ============================================================
// Hasher64<T, Algo> — 64비트 기본 (SimplePrime64)
// 하위호환을 위해 Hasher와 별도 템플릿 유지
// ============================================================

template <typename T, typename TAlgo = HashAlgorithm::SimplePrime64>
struct Hasher64
{
	using Result = typename TAlgo::Result;
	constexpr Result operator()(T _val) const noexcept
	{
		if constexpr (jc::IsFundamentalType_v<T> || jc::IsEnumType_v<T>)
			return TAlgo::HashIntegral(static_cast<_u64>(_val));
		else
			return TAlgo::HashBytes(&_val, sizeof(T));
	}
};

template <typename TAlgo>
struct Hasher64<float, TAlgo>
{
	using Result = typename TAlgo::Result;
	union Bit { float val_{}; _u32 u_; };
	constexpr Result operator()(float _val) const noexcept
	{
		Bit b{}; b.val_ = _val;
		return TAlgo::HashIntegral(b.u_);
	}
};

template <typename TAlgo>
struct Hasher64<double, TAlgo>
{
	using Result = typename TAlgo::Result;
	union Bit { double val_{}; _u64 u_; };
	constexpr Result operator()(double _val) const noexcept
	{
		Bit b{}; b.val_ = _val;
		return TAlgo::HashIntegral(b.u_);
	}
};

template <typename TAlgo>
struct Hasher64<const char*, TAlgo>
{
	using Result = typename TAlgo::Result;

	Result operator()(const String& _val) const noexcept
	{
		return TAlgo::HashBytes(_val.Source(), static_cast<size_t>(_val.Length()));
	}

	constexpr Result operator()(const char* _val) const noexcept
	{
		return _val ? TAlgo::HashBytes(_val, jc::StringUtil::Length(_val)) : TAlgo::kOffset;
	}

	template <_u32 Size>
	constexpr Result operator()(const char (&_val)[Size]) const noexcept
	{
		size_t len = Size;
		if (len > 0 && _val[len - 1] == '\0')
			--len;
		return TAlgo::HashBytes(_val, len);
	}
};

template <typename T, typename TAlgo>
struct Hasher64<T*, TAlgo>
{
	using Result = typename TAlgo::Result;
	constexpr Result operator()(T* _val) const noexcept
	{
		return TAlgo::HashIntegral(reinterpret_cast<_u64>(_val));
	}
};

template <typename T, typename TAlgo>
struct Hasher64<T* const, TAlgo>
{
	using Result = typename TAlgo::Result;
	constexpr Result operator()(T* const _val) const noexcept
	{
		return TAlgo::HashIntegral(reinterpret_cast<_u64>(_val));
	}
};

template <typename TAlgo>
struct Hasher64<String, TAlgo>
{
	using Result = typename TAlgo::Result;

	Result operator()(const String& _val) const noexcept
	{
		return TAlgo::HashBytes(_val.Source(), static_cast<size_t>(_val.Length()));
	}

	Result operator()(const char* _val) const noexcept
	{
		return _val ? TAlgo::HashBytes(_val, jc::StringUtil::Length(_val)) : TAlgo::kOffset;
	}

	template <_u32 Size>
	constexpr Result operator()(const char (&_val)[Size]) const noexcept
	{
		size_t len = Size;
		if (len > 0 && _val[len - 1] == '\0')
			--len;
		return TAlgo::HashBytes(_val, len);
	}
};

// ============================================================
// 하위호환 free 함수 (deprecated) — 기존 호출부 유지
// ============================================================

constexpr _u32 HashString(const char* _val, int _len) noexcept
{
	return HashAlgorithm::SimplePrime32::HashBytes(_val, static_cast<size_t>(_len));
}

constexpr _u64 HashString64(const char* _val, int _len) noexcept
{
	return HashAlgorithm::SimplePrime64::HashBytes(_val, static_cast<size_t>(_len));
}

NS_END

#pragma warning (pop)
