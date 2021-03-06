/*
	작성자 : 윤정도
	해쉬 생성기
*/

#pragma once

#include <JCore/Type.h>
#include <JCore/String.h>
#include <JCore/TypeTraits.h>

#pragma warning (push)
#pragma warning (disable : 4244)  // 'argument': conversion from 'double' to 'float', possible loss of data, double을 강제로 float으로 바꿀라캐서 Hasher<double>  땜에

namespace JCore {
	
constexpr Int32U PrimeInt32U_v = 0x087b840FU;			// 1억 부근 암거나 - 142,312,463
constexpr Int64U PrimeInt64U_v = 0x0000050B00000002ULL;	// 5조 부근 암거나 - 5,544,802,779,138
constexpr Int64U HashXorKey_v = 0x3e4dc77d;				// Xor 키값 암거나 - 1,045,284,733

template <typename T>
struct Hasher
{
	constexpr Int32U operator()(T val) const {
		if constexpr (JCore::IsPrimitiveType_v<T>)
			return ((val ^ HashXorKey_v) % PrimeInt32U_v) * PrimeInt32U_v;
		else {	// 다른 타입이면 강제로 형변환 후 진행
			return ((static_cast<int>(val) ^ HashXorKey_v) % PrimeInt32U_v) * PrimeInt32U_v;
		}
	}
};


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
		return Hasher<float>()(val);
	}
};


template <>
struct Hasher<String>
{
	Int32U operator()(const String& val) const {
		Int32U uiConv = PrimeInt32U_v;
		const char* pBuffer = val.Source();
 
		for (int i = 0; i < val.Length(); i++) {
			uiConv ^= pBuffer[i] ^ HashXorKey_v;
			uiConv *= PrimeInt32U_v;
		}

		return uiConv;
	}
};

} // namespace JCore

#pragma warning (pop)