#pragma once

#include <JCore/Namespace.h>

NS_JC_BEGIN

// 값을 타입으로 쓸수 있도록 하는 템플릿
template <typename T, T Val>
struct IntegralConstant { static constexpr T Value = Val; };

struct TrueType : IntegralConstant<bool, true> {};
struct FalseType : IntegralConstant<bool, false> {};

NS_JC_END