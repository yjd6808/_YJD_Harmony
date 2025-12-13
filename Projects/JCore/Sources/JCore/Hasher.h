/*
	작성자 : 윤정도
	해쉬 생성기
*/

#pragma once

#include <JCore/Type.h>
#include <JCore/Primitives/String.h>
#include <JCore/Primitives/StringUtil.h>
#include <JCore/TypeTraits.h>

#pragma warning (push)
#pragma warning (disable : 4244)  // 'argument': conversion from 'double' to 'float', possible loss of data, double을 강제로 float으로 바꿀라캐서 Hasher<double>  땜에


NS_JC_BEGIN
constexpr Int32U PrimeInt32U_v = 0x087b840FU; // 1억 부근 암거나 - 142,312,463
constexpr Int64U PrimeInt64U_v = 0x0000050B00000002ULL; // 5조 부근 암거나 - 5,544,802,779,138
constexpr Int64U HashXorKey32U_v = 0x3e4dc77d; // Xor 키값 암거나 - 1,045,284,733
constexpr Int64U HashXorKey64U_v = 0x000009866a1bc6b9; // Xor 키값 암거나 - 10'472'910'472'889

template <typename T>
struct Hasher
{
	constexpr Int32U operator()(T _val) const
	{
		if constexpr (JCore::IsFundamentalType_v<T>)
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
		Int32U u_;
	};

	constexpr Int32U operator()(float _val) const
	{
		return Hasher<Int32U>()(Bit{_val}.u_);
	}
};

template <>
struct Hasher<double>
{
	union Bit
	{
		double val_{};
		Int64U u_;
	};

	constexpr Int32U operator()(double _val) const
	{
		return Hasher<Int64U>()(Bit{_val}.u_);
	}
};

constexpr Int32U HashString(const char* _val, int _len)
{
	Int32U conv = PrimeInt32U_v;
	char* pBuffer = const_cast<char*>(_val);
#if defined(_WIN64)
using TStepType = Int64U;
#else
	using TStepType = Int32U;
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
	Int32U operator()(const String& _val) const
	{
		return HashString(_val.Source(), _val.Length());
	}

	constexpr Int32U operator()(const char* _val) const
	{
		return HashString(_val, JCore::StringUtil::Length(_val));
	}

	template <Int32U Size>
	constexpr Int32U operator()(const char (&_val)[Size]) const
	{
		return HashString(_val, Size);
	}
};

template <typename T>
struct Hasher<T*>
{
	constexpr Int32U operator()(T* _val) const
	{
#if defined(_WIN64)
	return Hasher<Int64U>()((Int64U)val);
#else
		return Hasher<Int32U>()((Int32U)_val);
#endif
	}
};


template <>
struct Hasher<String>
{
	Int32U operator()(const String& _val) const
	{
		return HashString(_val.Source(), _val.Length());
	}

	Int32U operator()(const char* _val) const
	{
		return HashString(_val, JCore::StringUtil::Length(_val));
	}

	template <Int32U Size>
	Int32U operator()(const char (&_val)[Size]) const
	{
		return HashString(_val, Size);
	}
};


template <typename T>
struct Hasher64
{
	constexpr Int64U operator()(T _val) const
	{
		if constexpr (JCore::IsFundamentalType_v<T>)
			return ((_val ^ HashXorKey64U_v) % PrimeInt64U_v) * PrimeInt64U_v;
		else
		{
			// 다른 타입이면 강제로 형변환 후 진행
			return ((static_cast<Int64U>(_val) ^ HashXorKey64U_v) % PrimeInt64U_v) * PrimeInt64U_v;
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
		Int64U u_;
	};

	constexpr Int64U operator()(float _val) const
	{
		return Hasher<Int64U>()(Bit{_val}.u_);
	}
};

template <>
struct Hasher64<double>
{
	union Bit
	{
		double val_{};
		Int64U u_;
	};

	constexpr Int64U operator()(double _val) const
	{
		return Hasher<Int64U>()(Bit{_val}.u_);
	}
};


constexpr Int64U HashString64(const char* _val, int _len)
{
	Int64U conv = PrimeInt64U_v;
	char* pBuffer = const_cast<char*>(_val);
#if defined(_WIN64)
using TStepType = Int64U;
#else
	using TStepType = Int32U;
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
	Int64U operator()(const String& _val) const
	{
		return HashString64(_val.Source(), _val.Length());
	}

	constexpr Int64U operator()(const char* _val) const
	{
		return HashString64(_val, JCore::StringUtil::Length(_val));
	}

	template <Int32U Size>
	constexpr Int64U operator()(const char (&_val)[Size]) const
	{
		return HashString64(_val, Size);
	}
};

template <typename T>
struct Hasher64<T*>
{
	constexpr Int64U operator()(T* _val) const
	{
#if defined(_WIN64)
	return Hasher64<Int64U>()((Int64U)val);
#else
		return Hasher64<Int32U>()((Int32U)_val);
#endif
	}
};


template <>
struct Hasher64<String>
{
	Int64U operator()(const String& _val) const
	{
		return HashString64(_val.Source(), _val.Length());
	}

	constexpr Int64U operator()(const char* _val) const
	{
		return HashString64(_val, JCore::StringUtil::Length(_val));
	}

	template <Int32U Size>
	constexpr Int64U operator()(const char (&_val)[Size]) const
	{
		return HashString64(_val, Size);
	}
};


NS_JC_END

#pragma warning (pop)
