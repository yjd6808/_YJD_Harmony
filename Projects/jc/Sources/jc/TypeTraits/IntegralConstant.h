#pragma once

#include <jc/Namespace.h>

NS_JC_BEGIN

// 값을 타입으로 쓸수 있도록 하는 템플릿
template <typename T, T _val>
struct IntegralConstant
{
	static constexpr T VALUE = _val;
};

template <bool _val>
struct ValidConstant
{
	static constexpr bool VALID = _val;
};

struct TrueType : IntegralConstant<bool, true> {};
struct FalseType : IntegralConstant<bool, false> {};
struct ValidType : ValidConstant<true> {};
struct InvalidType : ValidConstant<false> {};


NS_JC_END
