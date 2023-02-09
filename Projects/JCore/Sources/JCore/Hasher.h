/*
	작성자 : 윤정도
	해쉬 생성기
*/

#pragma once

#include <JCore/Type.h>
#include <JCore/Primitives/String.h>
#include <JCore/TypeTraits.h>

#pragma warning (push)
#pragma warning (disable : 4244)  // 'argument': conversion from 'double' to 'float', possible loss of data, double을 강제로 float으로 바꿀라캐서 Hasher<double>  땜에

NS_JC_BEGIN
	
constexpr Int32U PrimeInt32U_v = 0x087b840FU;			// 1억 부근 암거나 - 142,312,463
constexpr Int64U PrimeInt64U_v = 0x0000050B00000002ULL;	// 5조 부근 암거나 - 5,544,802,779,138
constexpr Int64U HashXorKey_v = 0x3e4dc77d;				// Xor 키값 암거나 - 1,045,284,733

template <typename T>
struct Hasher
{
	constexpr Int32U operator()(T val) const {
		if constexpr (JCore::IsFundamentalType_v<T>)
			return ((val ^ HashXorKey_v) % PrimeInt32U_v) * PrimeInt32U_v;
		else {	// 다른 타입이면 강제로 형변환 후 진행
			return ((static_cast<int>(val) ^ HashXorKey_v) % PrimeInt32U_v) * PrimeInt32U_v;
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

template <typename T>
struct Hasher<T*>
{
	constexpr Int32U operator()(T* val) const {
		return Hasher<Int32U>()((Int32U)val);
	}
};


template <>
struct Hasher<String> {
	Int32U operator()(const String& val) const {
		Int32U uiConv = PrimeInt32U_v;
		char* pBuffer = val.Source();
#if defined(_WIN64)
		using TStepType = Int64U;
#else
		using TStepType = Int32U;
#endif
		constexpr int iStep = sizeof(TStepType);	// 플랫폼에 따라.. 다르게
		const int iStepCount = val.Length() / iStep;

		int i = 0;
		for (; i < iStepCount; i += iStep) {
			uiConv ^= Hasher<TStepType>()(*reinterpret_cast<TStepType*>(pBuffer + i));
			uiConv *= PrimeInt32U_v;
		}

		for (; i < val.Length(); ++i) {
			uiConv ^= pBuffer[i] ^ HashXorKey_v;
			uiConv *= PrimeInt32U_v;
		}

		return uiConv;
	}
};

NS_JC_END

#pragma warning (pop)