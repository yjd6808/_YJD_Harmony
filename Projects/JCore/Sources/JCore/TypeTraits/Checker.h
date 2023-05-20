#pragma once

#include <JCore/TypeTraits/IntegralConstant.h>
#include <JCore/TypeTraits/Eliminator.h>

NS_JC_BEGIN

class DateTime;
class String;

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

	

	NS_DETAIL_END


template <typename T>
constexpr bool IsPointerType_v = Detail::IsPointerType<RemoveReference_t<T>>::Value;

template <typename T>
constexpr bool IsReferenceType_v = Detail::IsReferenceType<T>::Value;

template <typename T>
constexpr bool IsIntegerType_v = Detail::IsIntegerType<NaturalType_t<T>>::Value;

template <typename T>
constexpr bool IsFloatType_v = Detail::IsFloatType<NaturalType_t<T>>::Value;

template <typename T>
constexpr bool IsCharaterType_v = Detail::IsCharaterType<NaturalType_t<T>>::Value;

template <typename T>
constexpr bool IsStringType_v = Detail::IsStringType<NaturalType_t<T>>::Value;

template <typename T>
constexpr bool IsArrayType_v = Detail::IsArrayType<NaturalType_t<T>>::Value;

template <typename T>
constexpr bool IsStaticArrayType_v = Detail::IsStaticArrayType<NaturalType_t<T>>::Value;

template <typename T>
constexpr bool IsDyanmicArrayType_v = Detail::IsDynamicArrayType<NaturalType_t<T>>::Value;

template <typename T>
constexpr bool IsVoidType_v = Detail::IsVoid<NaturalType_t<T>>::Value;

template <typename T>
constexpr bool IsInt8_v = Detail::IsInt8<T>::Value;
template <typename T>
constexpr bool IsInt8U_v = Detail::IsInt8U<T>::Value;
template <typename T>
constexpr bool IsInt16_v = Detail::IsInt16<T>::Value;
template <typename T>
constexpr bool IsInt16U_v = Detail::IsInt16U<T>::Value;
template <typename T>
constexpr bool IsInt32_v = Detail::IsInt32<T>::Value;
template <typename T>
constexpr bool IsInt32U_v = Detail::IsInt32U<T>::Value;
template <typename T>
constexpr bool IsInt32L_v = Detail::IsInt32L<T>::Value;
template <typename T>
constexpr bool IsInt32UL_v = Detail::IsInt32UL<T>::Value;
template <typename T>
constexpr bool IsInt64_v = Detail::IsInt64<T>::Value;
template <typename T>
constexpr bool IsInt64U_v = Detail::IsInt64U<T>::Value;
template <typename T>
constexpr bool IsDouble_v = Detail::IsDouble<T>::Value;
template <typename T>
constexpr bool IsLDouble_v = Detail::IsLDouble<T>::Value;
template <typename T>
constexpr bool IsString_v = Detail::IsString<T>::Value;
template <typename T>
constexpr bool IsDateTime_v = Detail::IsDateTime<T>::Value;


NS_JC_END