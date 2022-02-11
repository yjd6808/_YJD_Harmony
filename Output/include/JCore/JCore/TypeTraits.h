#pragma once

#include <JCore/String.h>
#include <JCore/StringUtil.h>
#include <JCore/StaticString.h>

#include <type_traits>

namespace JCore {

/* =============================================================== */
// 값을 타입으로 쓸수 있도록 하는 템플릿
template <typename T, T Value>
struct IntegralConstant { static constexpr T value = Value; };

struct TrueType : IntegralConstant<bool, true> {};
struct FalseType : IntegralConstant<bool, false> {};
/* =============================================================== */



/* =============================================================== */
// 참조자 제거해주는 템플릿
template <typename T>
struct RemoveReference
{
	using Type = typename T;
};

template <typename T>
struct RemoveReference<T&>
{
	using Type = typename T;
};

template <typename T>
struct RemoveReference<T&&>
{
	using Type = typename T;
};

template <typename T>
using RemoveReference_t = typename RemoveReference<T>::Type;
/* =============================================================== */



/* =============================================================== */
// 한정자 제거해주는 템플릿
// 포인터 타입은 제거가 안댐
template <typename T>
struct RemoveQulifier
{
	using Type = typename T;
};

template <typename T>
struct RemoveQulifier<const T>
{
	using Type = typename T;
};

template <typename T>
struct RemoveQulifier<volatile T>
{
	using Type = typename T;
};

template <typename T>
struct RemoveQulifier<volatile const T>
{
	using Type = typename T;
};

template <typename T>
using RemoveQulifier_t = typename RemoveQulifier<T>::Type;
/* =============================================================== */



/* =============================================================== */
// 배열 괄호 제거해서 기본 타입만 얻도록 하는 템플릿
template <typename T>
struct RemoveArray
{
	using Type = typename T;
};

template <typename T>
struct RemoveArray<T[]>
{
	using Type = typename T;
};

template <typename T, Int32U ArraySize>
struct RemoveArray<T[ArraySize]>
{
	using Type = typename T;
};

template <typename T>
using RemoveArray_t = typename RemoveArray<T>::Type;
/* =============================================================== */


/* =============================================================== */
// 한정자 제거해주는 템플릿
// 포인터 타입은 제거가 안댐
template <typename T>
struct RevmovePointer
{
	using Type = typename T;
};

template <typename T>
struct RevmovePointer<T*>
{
	using Type = typename T;
};


template <typename T>
using RemovePointer_t = typename RevmovePointer<T>::Type;
/* =============================================================== */



/* =============================================================== */
// 무브 시멘틱과 완벽한 전달
template <typename T>
inline constexpr RemoveReference_t<T>&& Move(T&& arg) {
	return static_cast<RemoveReference_t<T>&&>(arg);
}

template <typename T>
inline constexpr T&& Forward(RemoveReference_t<T>& arg) {
	return static_cast<T&&>(arg);
}
/* =============================================================== */






/* =============================================================== */
// 포인터 타입인지 확인하는 템플릿
template <typename T> 
struct IsPointerType : FalseType {};

template <typename T> 
struct IsPointerType<T*> : TrueType {};

template <typename T>
constexpr bool IsPointerType_v = IsPointerType<T>::value;
/* =============================================================== */


/* =============================================================== */
// 참조 타입인지 확인하는 템플릿
template <typename T> 
struct IsReferenceType : FalseType {};

template <typename T> 
struct IsReferenceType<T&> : TrueType {};

template <typename T> 
struct IsReferenceType<T&&> : TrueType {};

template <typename T>
constexpr bool IsReferenceType_v = IsReferenceType<T>::value;
/* =============================================================== */



/* =============================================================== */
// 한정자, 참조타입을 모두 제거해주는 템플릿
// ex) const voaltile int&& -> int타입으로댐
template <typename T>
struct NaturalType
{
	using Type = typename RemoveQulifier_t<RemoveReference_t<T>>;
};

template <typename T>
using NaturalType_t = typename NaturalType<T>::Type;
/* =============================================================== */



/* =============================================================== */
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

template <typename T>
constexpr bool IsIntegerType_v = IsIntegerType<NaturalType_t<T>>::value;
/* =============================================================== */


/* =============================================================== */
// 실수 타입인지 확인하는 템플릿
template <typename T>
struct IsFloatingPointType : FalseType {};

template <>
struct IsFloatingPointType<float> : TrueType {};
template <>
struct IsFloatingPointType<double> : TrueType {};
template <>
struct IsFloatingPointType<long double> : TrueType {};

template <typename T>
constexpr bool IsFloatingPointType_v = IsFloatingPointType<NaturalType_t<T>>::value;
/* =============================================================== */


/* =============================================================== */
// 문자형 타입인지 확인하는 템플릿
template <typename T>
struct IsCharaterType : FalseType {};

template <>
struct IsCharaterType<char> : TrueType {};
template <>
struct IsCharaterType<wchar_t> : TrueType {};

template <typename T>
constexpr bool IsCharaterType_v = IsCharaterType<NaturalType_t<T>>::value;
/* =============================================================== */

/* =============================================================== */
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

template <typename T>
constexpr bool IsStringType_v = IsStringType<NaturalType_t<T>>::value;
/* =============================================================== */


/* =============================================================== */
// 문자열 타입인지 확인하는 템플릿

template <typename T>
struct IsArrayType : FalseType {};

template <typename T>
struct IsArrayType<T[]> : TrueType {};

template <typename T, Int32U ArraySize>
struct IsArrayType<T[ArraySize]> : TrueType {};

template <typename T>
constexpr bool IsArrayType_v = IsArrayType<NaturalType_t<T>>::value;
/* =============================================================== */


// 런타임에 타입명을 가져올 때 사용합니다.
template <typename T>
String Type() {
	String szFuncSig(__FUNCSIG__);

	const int kiLeftIdx = szFuncSig.Find("<");
	const int kiRightIdx = szFuncSig.FindReverse(">");

	return szFuncSig.GetRange(kiLeftIdx + 1, kiRightIdx - 1);
}

// 컴파일타임에 타입명을 가져올 때 사용합니다.
// 반환타입 : StaticString<Size>
template <typename T>
constexpr auto CTType() {
	constexpr int iStartIdx = StringUtil::CTFindChar(__FUNCSIG__, '<') + 1;
	constexpr int iEndIdx = StringUtil::CTFindCharReverse(__FUNCSIG__, '>') - 1;
	constexpr int iSize = iEndIdx - iStartIdx + 1;

	StaticString<iSize + 1> szResult{};
	szResult.Source[iSize] = '\0';

	char* pDst = (char*)__FUNCSIG__;
	char* pSrc = (char*)szResult.Source;

	for (int i = 0; i < iSize; i++) {
		*pSrc = *(pDst + iStartIdx + i);
		pSrc++;
	}

	if (szResult.StartWith("class")) {
		//return szResult.Remove("class");
	} else if (szResult.StartWith("struct")) {
		//return szResult.Remove("struct");
	}

	return szResult;
}

template <typename T1, typename T2>
inline constexpr bool IsSameType() {
	return CTType<T1>() == CTType<T2>();
}

template <typename T>
constexpr auto Type_v = CTType<T>();

template <typename T1, typename T2>
constexpr bool IsSameType_v = IsSameType<T1, T2>();

// From에서 To로 암묵적 변환이 가능한지
// 즉 To ? = From이 가능한지
// @참고 : https://docs.microsoft.com/ko-kr/cpp/standard-library/is-convertible-class?view=msvc-170
template <typename From, typename To>
constexpr bool IsConvertible_v = std::is_convertible_v<From, To>;


template <typename Base, typename Derived>
constexpr bool IsBaseOf_v = std::is_base_of_v<Base, Derived>;



template <typename T>
constexpr bool IsPrimitiveType_v = std::is_fundamental<T>::value;



// 서로 다이나믹 캐스팅이 가능한지 알려주는 녀석
// 템플릿 인자로 전달한 녀석이 둘 모두 포인터 타입 또는 레퍼런스 타입인 경우
// 원시 타입은 원래 다이나믹 캐스팅이 안되지만 같은 타입끼리는 걍 된다고 처리
template <typename Lhs, typename Rhs>
constexpr bool DynamicCastable_v = (IsPointerType_v<Lhs> && IsPointerType_v<Rhs>)  ||
								   (IsReferenceType_v<Lhs> && IsReferenceType_v<Rhs>) 
								   ?
										IsSameType_v<Lhs, Rhs>	// 그냥 서로 동일한 타입이면 무조건 OK
										? 
										true
										:
										IsBaseOf_v<RemovePointer_t<RemoveReference_t<Lhs>>, RemovePointer_t<RemoveReference_t<Rhs>>> ||
										IsBaseOf_v<RemovePointer_t<RemoveReference_t<Rhs>>, RemovePointer_t<RemoveReference_t<Lhs>>>
								   :
								   false;

// From이 To의 부모 타입인지 포인터, 참조타입 인경우에 비교
template <typename From, typename To>
constexpr bool IsRPBasedOf_v =  (IsPointerType_v<From> && IsPointerType_v<To>)  ||
								(IsReferenceType_v<From> && IsReferenceType_v<To>)
								?
								IsBaseOf_v<RemovePointer_t<RemoveReference_t<From>>, RemovePointer_t<RemoveReference_t<To>>>
								:
								false;

/*

template <typename Lhs, typename Rhs>
constexpr bool DynamicCastable_v = (IsPointerType_v<Lhs> && IsPointerType_v<Rhs>)  ||
								   (IsReferenceType_v<Lhs> && IsReferenceType_v<Rhs>)
								   ?
										IsPrimitiveType_v<RemovePointer_t<RemoveReference_t<Lhs>>> &&
										IsPrimitiveType_v<RemovePointer_t<RemoveReference_t<Rhs>>>
										?
										IsSameType_v<Lhs, Rhs>
										:
										IsBaseOf_v<RemovePointer_t<RemoveReference_t<Lhs>>, RemovePointer_t<RemoveReference_t<Rhs>>> ||
										IsBaseOf_v<RemovePointer_t<RemoveReference_t<Rhs>>, RemovePointer_t<RemoveReference_t<Lhs>>>
								   :
								   false;

*/



template <typename T>
constexpr T* AddressOf(T& arg) {
	return __builtin_addressof((RemoveQulifier_t<T>&)arg);
}


/* ====================================================================== */
// 제일 처음에 구현한 정수, 실수, 문자열 타입 체크 함수
/*
template <typename T>
constexpr bool IsIntegerType() {
	const auto kType = CTType<RemoveQulifier_t<RemoveReference_t<T>>>();
	return	kType == "int"		|| kType == "unsigned int"		|| 
			kType == "short"	|| kType == "unsigned short"	||
			kType == "long"		|| kType == "unsigned long"		||
			kType == "long long"|| kType == "unsigned long long";
}

template <typename T>
constexpr bool IsFloatingType() {
	const auto kType = CTType<RemoveQulifier_t<RemoveReference_t<T>>>();
	return	kType == "float" || kType == "double";
}

template <typename T>
constexpr bool IsStringType() {
	const auto kType = CTType<RemoveQulifier_t<RemoveReference_t<T>>>();
	return	kType == "char *"		|| kType == "String" ||
			kType == "wchar_t *"	|| (kType.StartWith("char [") && kType.EndWith("]")) ||
			(kType.StartWith("wchar_t [") && kType.EndWith("]"));
}
*/
/* ====================================================================== */




} // namespace JCore