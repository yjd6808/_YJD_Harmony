﻿/*
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
	
constexpr Int32U PrimeInt32U_v = 0x087b840FU;				// 1억 부근 암거나 - 142,312,463
constexpr Int64U PrimeInt64U_v = 0x0000050B00000002ULL;		// 5조 부근 암거나 - 5,544,802,779,138
constexpr Int64U HashXorKey32U_v = 0x3e4dc77d;				// Xor 키값 암거나 - 1,045,284,733
constexpr Int64U HashXorKey64U_v = 0x000009866a1bc6b9;		// Xor 키값 암거나 - 10'472'910'472'889

template <typename T>
struct Hasher
{
	constexpr Int32U operator()(T val) const {
		if constexpr (JCore::IsFundamentalType_v<T>)
			return ((val ^ HashXorKey32U_v) % PrimeInt32U_v) * PrimeInt32U_v;
		else {	// 다른 타입이면 강제로 형변환 후 진행
			return ((static_cast<int>(val) ^ HashXorKey32U_v) % PrimeInt32U_v) * PrimeInt32U_v;
		}
	}
};



// float, double은 꼼수로...
template <>
struct Hasher<float>
{
	union Bit
	{
		float val{};
		Int32U u;
	};

	constexpr Int32U operator()(float val) const {
		return Hasher<Int32U>()(Bit{ val }.u);
	}
};

template <>
struct Hasher<double>
{
	union Bit
	{
		double val{};
		Int64U u;
	};

	constexpr Int32U operator()(double val) const {
		return Hasher<Int64U>()(Bit{ val }.u);
	}
};

constexpr Int32U HashString(const char* val, int len) {
	Int32U uiConv = PrimeInt32U_v;
	char* pBuffer = const_cast<char*>(val);
#if defined(_WIN64)
	using TStepType = Int64U;
#else
	using TStepType = Int32U;
#endif
	constexpr int iStep = sizeof(TStepType);	// 플랫폼에 따라.. 다르게
	const int iStepCount = len / iStep;

	int i = 0;
	for (; i < iStepCount; i += iStep) {
		uiConv ^= Hasher<TStepType>()(*reinterpret_cast<TStepType*>(pBuffer + i));
		uiConv *= PrimeInt32U_v;
	}

	for (; i < len; ++i) {
		uiConv ^= pBuffer[i] ^ HashXorKey32U_v;
		uiConv *= PrimeInt32U_v;
	}

	return uiConv;
}

template <>
struct Hasher<const char*>
{
	Int32U operator()(const String& val) const {
		return HashString(val.Source(), val.Length());
	}

	constexpr Int32U operator()(const char* val) const {
		return HashString(val, JCore::StringUtil::Length(val));
	}

	template <Int32U Size>
	constexpr Int32U operator()(const char(&val)[Size]) const {
		return HashString(val, Size);
	}

};

template <typename T>
struct Hasher<T*>
{
	constexpr Int32U operator()(T* val) const {
#if defined(_WIN64)
		return Hasher<Int64U>()((Int64U)val);
#else
		return Hasher<Int32U>()((Int32U)val);
#endif
	}
};




template <>
struct Hasher<String> {
	Int32U operator()(const String& val) const {
		return HashString(val.Source(), val.Length());
	}

	Int32U operator()(const char* val) const {
		return HashString(val, JCore::StringUtil::Length(val));
	}

	template <Int32U Size>
	Int32U operator()(const char(&val)[Size]) const {
		return HashString(val, Size);
	}
};




template <typename T>
struct Hasher64
{
	constexpr Int64U operator()(T val) const {
		if constexpr (JCore::IsFundamentalType_v<T>)
			return ((val ^ HashXorKey64U_v) % PrimeInt64U_v) * PrimeInt64U_v;
		else {	// 다른 타입이면 강제로 형변환 후 진행
			return ((static_cast<Int64U>(val) ^ HashXorKey64U_v) % PrimeInt64U_v) * PrimeInt64U_v;
		}
	}
};



// float, double은 꼼수로...
template <>
struct Hasher64<float>
{
	union Bit
	{
		float val{};
		Int64U u;
	};

	constexpr Int64U operator()(float val) const {
		return Hasher<Int64U>()(Bit{ val }.u);
	}
};

template <>
struct Hasher64<double>
{
	union Bit
	{
		double val{};
		Int64U u;
	};

	constexpr Int64U operator()(double val) const {
		return Hasher<Int64U>()(Bit{ val }.u);
	}
};


constexpr Int64U HashString64(const char* val, int len) {
	Int64U uiConv = PrimeInt64U_v;
	char* pBuffer = const_cast<char*>(val);
#if defined(_WIN64)
	using TStepType = Int64U;
#else
	using TStepType = Int32U;
#endif
	constexpr int iStep = sizeof(TStepType);	// 플랫폼에 따라.. 다르게
	const int iStepCount = len / iStep;

	int i = 0;
	for (; i < iStepCount; i += iStep) {
		uiConv ^= Hasher64<TStepType>()(*reinterpret_cast<TStepType*>(pBuffer + i));
		uiConv *= PrimeInt64U_v;
	}

	for (; i < len; ++i) {
		uiConv ^= pBuffer[i] ^ HashXorKey32U_v;
		uiConv *= PrimeInt64U_v;
	}

	return uiConv;
}



template <>
struct Hasher64<const char*>
{
	Int64U operator()(const String& val) const {
		return HashString64(val.Source(), val.Length());
	}

	constexpr Int64U operator()(const char* val) const {
		return HashString64(val, JCore::StringUtil::Length(val));
	}

	template <Int32U Size>
	constexpr Int64U operator()(const char(&val)[Size]) const {
		return HashString64(val, Size);
	}

};

template <typename T>
struct Hasher64<T*>
{
	constexpr Int64U operator()(T* val) const {
#if defined(_WIN64)
		return Hasher64<Int64U>()((Int64U)val);
#else
		return Hasher64<Int32U>()((Int32U)val);
#endif
	}
};




template <>
struct Hasher64<String> {
	Int64U operator()(const String& val) const {
		return HashString64(val.Source(), val.Length());
	}

	constexpr Int64U operator()(const char* val) const {
		return HashString64(val, JCore::StringUtil::Length(val));
	}

	template <Int32U Size>
	constexpr Int64U operator()(const char(&val)[Size]) const {
		return HashString64(val, Size);
	}
};


NS_JC_END

#pragma warning (pop)