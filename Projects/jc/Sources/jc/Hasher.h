/*
	작성자 : 윤정도
	해쉬 생성기
*/

#pragma once

#include "jc/Type.h"
#include "jc/Primitives/String.h"
#include "jc/Primitives/StringUtil.h"
#include "jc/TypeTraits.h"

#pragma warning (push)
#pragma warning (disable : 4244)  // 'argument': conversion from 'double' to 'float', possible loss of data, double을 강제로 float으로 바꿀라캐서 Hasher<double>  땜에


NS_JC_BEGIN
constexpr _u32 PrimeInt32U_v = 0x087b840FU; // 1억 부근 암거나 - 142,312,463
constexpr _u64 PrimeInt64U_v = 0x0000050B00000002ULL; // 5조 부근 암거나 - 5,544,802,779,138
constexpr _u64 HashXorKey32U_v = 0x3e4dc77d; // Xor 키값 암거나 - 1,045,284,733
constexpr _u64 HashXorKey64U_v = 0x000009866a1bc6b9; // Xor 키값 암거나 - 10'472'910'472'889

template <typename T>
struct Hasher
{
	constexpr _u32 operator()(T _val) const
	{
		if constexpr (jc::IsFundamentalType_v<T>)
			return ((_val ^ HashXorKey32U_v) % PrimeInt32U_v) * PrimeInt32U_v;
		else
		{
			// 다른 타입이면 강제로 형변환 후 진행
			return ((static_cast<int>(_val) ^ HashXorKey32U_v) % PrimeInt32U_v) * PrimeInt32U_v;
		}
	}
};


// float, double은 꼼수로...
template <>
struct Hasher<float>
{
	union Bit
	{
		float val_{};
		_u32 u_;
	};

	constexpr _u32 operator()(float _val) const
	{
		return Hasher<_u32>()(Bit{_val}.u_);
	}
};

template <>
struct Hasher<double>
{
	union Bit
	{
		double val_{};
		_u64 u_;
	};

	constexpr _u32 operator()(double _val) const
	{
		return Hasher<_u64>()(Bit{_val}.u_);
	}
};

constexpr _u32 HashString(const char* _val, int _len)
{
	_u32 conv = PrimeInt32U_v;
	char* pBuffer = const_cast<char*>(_val);
#if defined(_WIN64)
using TStepType = _u64;
#else
	using TStepType = _u32;
#endif
	constexpr int step = sizeof(TStepType); // 플랫폼에 따라.. 다르게
	const int stepCount = _len / step;

	int i = 0;
	for (; i < stepCount; i += step)
	{
		conv ^= Hasher<TStepType>()(*reinterpret_cast<TStepType*>(pBuffer + i));
		conv *= PrimeInt32U_v;
	}

	for (; i < _len; ++i)
	{
		conv ^= pBuffer[i] ^ HashXorKey32U_v;
		conv *= PrimeInt32U_v;
	}

	return conv;
}

template <>
struct Hasher<const char*>
{
	_u32 operator()(const String& _val) const
	{
		return HashString(_val.Source(), _val.Length());
	}

	constexpr _u32 operator()(const char* _val) const
	{
		return HashString(_val, jc::StringUtil::Length(_val));
	}

	template <_u32 Size>
	constexpr _u32 operator()(const char (&_val)[Size]) const
	{
		return HashString(_val, Size);
	}
};

template <typename T>
struct Hasher<T*>
{
	constexpr _u32 operator()(T* _val) const
	{
#if defined(_WIN64)
	return Hasher<_u64>()((_u64)_val);
#else
		return Hasher<_u32>()((_u32)_val);
#endif
	}
};


template <>
struct Hasher<String>
{
	_u32 operator()(const String& _val) const
	{
		return HashString(_val.Source(), _val.Length());
	}

	_u32 operator()(const char* _val) const
	{
		return HashString(_val, jc::StringUtil::Length(_val));
	}

	template <_u32 Size>
	_u32 operator()(const char (&_val)[Size]) const
	{
		return HashString(_val, Size);
	}
};


template <typename T>
struct Hasher64
{
	constexpr _u64 operator()(T _val) const
	{
		if constexpr (jc::IsFundamentalType_v<T>)
			return ((_val ^ HashXorKey64U_v) % PrimeInt64U_v) * PrimeInt64U_v;
		else
		{
			// 다른 타입이면 강제로 형변환 후 진행
			return ((static_cast<_u64>(_val) ^ HashXorKey64U_v) % PrimeInt64U_v) * PrimeInt64U_v;
		}
	}
};


// float, double은 꼼수로...
template <>
struct Hasher64<float>
{
	union Bit
	{
		float val_{};
		_u64 u_;
	};

	constexpr _u64 operator()(float _val) const
	{
		return Hasher<_u64>()(Bit{_val}.u_);
	}
};

template <>
struct Hasher64<double>
{
	union Bit
	{
		double val_{};
		_u64 u_;
	};

	constexpr _u64 operator()(double _val) const
	{
		return Hasher<_u64>()(Bit{_val}.u_);
	}
};


constexpr _u64 HashString64(const char* _val, int _len)
{
	_u64 conv = PrimeInt64U_v;
	char* pBuffer = const_cast<char*>(_val);
#if defined(_WIN64)
using TStepType = _u64;
#else
	using TStepType = _u32;
#endif
	constexpr int step = sizeof(TStepType); // 플랫폼에 따라.. 다르게
	const int stepCount = _len / step;

	int i = 0;
	for (; i < stepCount; i += step)
	{
		conv ^= Hasher64<TStepType>()(*reinterpret_cast<TStepType*>(pBuffer + i));
		conv *= PrimeInt64U_v;
	}

	for (; i < _len; ++i)
	{
		conv ^= pBuffer[i] ^ HashXorKey32U_v;
		conv *= PrimeInt64U_v;
	}

	return conv;
}


template <>
struct Hasher64<const char*>
{
	_u64 operator()(const String& _val) const
	{
		return HashString64(_val.Source(), _val.Length());
	}

	constexpr _u64 operator()(const char* _val) const
	{
		return HashString64(_val, jc::StringUtil::Length(_val));
	}

	template <_u32 Size>
	constexpr _u64 operator()(const char (&_val)[Size]) const
	{
		return HashString64(_val, Size);
	}
};

template <typename T>
struct Hasher64<T*>
{
	constexpr _u64 operator()(T* _val) const
	{
#if defined(_WIN64)
	return Hasher64<_u64>()((_u64)_val);
#else
		return Hasher64<_u32>()((_u32)_val);
#endif
	}
};


template <>
struct Hasher64<String>
{
	_u64 operator()(const String& _val) const
	{
		return HashString64(_val.Source(), _val.Length());
	}

	constexpr _u64 operator()(const char* _val) const
	{
		return HashString64(_val, jc::StringUtil::Length(_val));
	}

	template <_u32 Size>
	constexpr _u64 operator()(const char (&_val)[Size]) const
	{
		return HashString64(_val, Size);
	}
};


NS_END

#pragma warning (pop)
