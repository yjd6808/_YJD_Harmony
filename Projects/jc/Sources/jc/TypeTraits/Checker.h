#pragma once

#include <jc/TypeTraits/IntegralConstant.h>
#include <jc/TypeTraits/Eliminator.h>

NS_JC_BEGIN

class DateTime;
class String;

template <Int32U> struct StaticString;

	NS_DETAIL_BEGIN
    // 포인터 타입인지 확인하는 템플릿
    template <typename T>
    struct IsPointerType : FalseType {}; 

    template <typename T>
    struct IsPointerType<T*> : TrueType {};

    // 참조 타입인지 확인하는 템플릿
    template <typename T>
    struct IsReferenceType : FalseType {};

    template <typename T>
    struct IsReferenceType<T&> : TrueType {};

    template <typename T>
    struct IsReferenceType<T&&> : TrueType {};

	// 한정자를 가진 타입인지 확인
    template <typename T>
    struct IsQulifiedType : FalseType {};

    template <typename T>
    struct IsQulifiedType<const T> : TrueType {};

    template <typename T>
    struct IsQulifiedType<const T&> : TrueType {};

    template <typename T>
    struct IsQulifiedType<const T&&> : TrueType {};

    template <typename T>
    struct IsQulifiedType<const volatile T> : TrueType {};

    template <typename T>
    struct IsQulifiedType<const volatile T&> : TrueType {};

    template <typename T>
    struct IsQulifiedType<const volatile T&&> : TrueType {};

    template <typename T>
    struct IsQulifiedType<volatile T> : TrueType {};

    template <typename T>
    struct IsQulifiedType<volatile T&> : TrueType {};

    template <typename T>
    struct IsQulifiedType<volatile T&&> : TrueType {};

    // 정수 타입인지 확인하는 템플릿
    template <typename T>
    struct IsIntegerType : FalseType {};

    // short
    template <>
    struct IsIntegerType<Int16> : TrueType {};
    template <>
    struct IsIntegerType<Int16U> : TrueType {};

    // int
    template <>
    struct IsIntegerType<Int32> : TrueType {};
    template <>
    struct IsIntegerType<Int32U> : TrueType {};

    // long
    template <>
    struct IsIntegerType<Int32L> : TrueType {};
    template <>
    struct IsIntegerType<Int32UL> : TrueType {};

    // long long
    template <>
    struct IsIntegerType<Int64> : TrueType {};
    template <>
    struct IsIntegerType<Int64U> : TrueType {};

    // 실수 타입인지 확인하는 템플릿
    template <typename T>
    struct IsFloatType : FalseType {};

    template <>
    struct IsFloatType<float> : TrueType {};
    template <>
    struct IsFloatType<double> : TrueType {};
    template <>
    struct IsFloatType<long double> : TrueType {};

    // 문자형 타입인지 확인하는 템플릿
    template <typename T>
    struct IsCharaterType : FalseType {};

    template <>
    struct IsCharaterType<char> : TrueType {};
    template <>
    struct IsCharaterType<wchar_t> : TrueType {};

    // 문자열 타입인지 확인하는 템플릿
    template <typename T>
    struct IsStringType : FalseType {};

    template <>
    struct IsStringType<char*> : TrueType {};
    template <>
    struct IsStringType<const char*> : TrueType {};
    template <>
    struct IsStringType<wchar_t*> : TrueType {};
    template <>
    struct IsStringType<const wchar_t*> : TrueType {};
    template <>
    struct IsStringType<String> : TrueType {};
    template <Int32U Size>
    struct IsStringType<StaticString<Size>> : TrueType {};
    template <Int32U Size>
    struct IsStringType<char[Size]> : TrueType {};
    template <Int32U Size>
    struct IsStringType<wchar_t[Size]> : TrueType {};

    // 배열 타입인지 확인
    template <typename T>
    struct IsArrayType : FalseType {};

    template <typename T>
    struct IsArrayType<T[]> : TrueType {};

    template <typename T, Int32U ArraySize>
    struct IsArrayType<T[ArraySize]> : TrueType {};

    // 정적 배열 타입인지
    template <typename T>
    struct IsStaticArrayType : FalseType {};

    template <typename T, Int32U ArraySize>
    struct IsStaticArrayType<T[ArraySize]> : TrueType {};

    // 동적 배열 타입인지
    template <typename T>
    struct IsDynamicArrayType : FalseType {};

    template <typename T>
    struct IsDynamicArrayType<T[]> : TrueType {};



    // Void 타입인지 확인
    template <typename T>
    struct IsVoid : FalseType {};

    template <>
    struct IsVoid<void> : TrueType {};


    template <typename T>   struct IsInt8 : FalseType {};
    template <>             struct IsInt8<Int8> : TrueType {};
    template <typename T>   struct IsInt8U : FalseType {};
    template <>             struct IsInt8U<Int8U> : TrueType {};
    template <typename T>   struct IsInt16 : FalseType {};
    template <>             struct IsInt16<Int16> : TrueType {};
    template <typename T>   struct IsInt16U : FalseType {};
    template <>             struct IsInt16U<Int16U> : TrueType {};
    template <typename T>   struct IsInt32 : FalseType {};
    template <>             struct IsInt32<Int32> : TrueType {};
    template <typename T>   struct IsInt32U : FalseType {};
    template <>             struct IsInt32U<Int32U> : TrueType {};
    template <typename T>   struct IsInt32L : FalseType {};
    template <>             struct IsInt32L<Int32L> : TrueType {};
    template <typename T>   struct IsInt32UL : FalseType {};
    template <>             struct IsInt32UL<Int32UL> : TrueType {};
    template <typename T>   struct IsInt64 : FalseType {};
    template <>             struct IsInt64<Int64> : TrueType {};
    template <typename T>   struct IsInt64U : FalseType {};
    template <>             struct IsInt64U<Int64U> : TrueType {};
    template <typename T>   struct IsDouble : FalseType {};
    template <>             struct IsDouble<Double> : TrueType {};
    template <typename T>   struct IsLDouble : FalseType {};
    template <>             struct IsLDouble<LDouble> : TrueType {};
    template <typename T>   struct IsString : FalseType {};
    template <>             struct IsString<String> : TrueType {};
    template <typename T>   struct IsDateTime : FalseType {};
    template <>             struct IsDateTime<DateTime> : TrueType {};

	

	NS_END


template <typename T>
constexpr bool IsPointerType_v = detail::IsPointerType<RemoveReference_t<T>>::VALUE;

template <typename T>
constexpr bool IsReferenceType_v = detail::IsReferenceType<T>::VALUE;

template <typename T>
constexpr bool IsNaturalType_v = !detail::IsReferenceType<T>::VALUE && !detail::IsPointerType<T>::VALUE && !detail::IsQulifiedType<T>::VALUE;

template <typename T>
constexpr bool IsIntegerType_v = detail::IsIntegerType<NaturalType_t<T>>::VALUE;

template <typename T>
constexpr bool IsFloatType_v = detail::IsFloatType<NaturalType_t<T>>::VALUE;

template <typename T>
constexpr bool IsNumericType_v = IsIntegerType_v<T> && IsFloatType_v<T>;

template <typename T>
constexpr bool IsCharaterType_v = detail::IsCharaterType<NaturalType_t<T>>::VALUE;

template <typename T>
constexpr bool IsStringType_v = detail::IsStringType<NaturalType_t<T>>::VALUE;

template <typename T>
constexpr bool IsArrayType_v = detail::IsArrayType<NaturalType_t<T>>::VALUE;

template <typename T>
constexpr bool IsStaticArrayType_v = detail::IsStaticArrayType<NaturalType_t<T>>::VALUE;

template <typename T>
constexpr bool IsDyanmicArrayType_v = detail::IsDynamicArrayType<NaturalType_t<T>>::VALUE;

template <typename T>
constexpr bool IsVoidType_v = detail::IsVoid<NaturalType_t<T>>::VALUE;

template <typename T>
constexpr bool IsInt8_v = detail::IsInt8<T>::VALUE;
template <typename T>
constexpr bool IsInt8U_v = detail::IsInt8U<T>::VALUE;
template <typename T>
constexpr bool IsInt16_v = detail::IsInt16<T>::VALUE;
template <typename T>
constexpr bool IsInt16U_v = detail::IsInt16U<T>::VALUE;
template <typename T>
constexpr bool IsInt32_v = detail::IsInt32<T>::VALUE;
template <typename T>
constexpr bool IsInt32U_v = detail::IsInt32U<T>::VALUE;
template <typename T>
constexpr bool IsInt32L_v = detail::IsInt32L<T>::VALUE;
template <typename T>
constexpr bool IsInt32UL_v = detail::IsInt32UL<T>::VALUE;
template <typename T>
constexpr bool IsInt64_v = detail::IsInt64<T>::VALUE;
template <typename T>
constexpr bool IsInt64U_v = detail::IsInt64U<T>::VALUE;
template <typename T>
constexpr bool IsDouble_v = detail::IsDouble<T>::VALUE;
template <typename T>
constexpr bool IsLDouble_v = detail::IsLDouble<T>::VALUE;
template <typename T>
constexpr bool IsString_v = detail::IsString<T>::VALUE;
template <typename T>
constexpr bool IsDateTime_v = detail::IsDateTime<T>::VALUE;


NS_END
