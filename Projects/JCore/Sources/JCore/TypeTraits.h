#pragma once

#include <JCore/Primitives/String.h>
#include <JCore/Primitives/StringUtil.h>
#include <JCore/Primitives/StaticString.h>
#include <JCore/TypeCast.h>

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
// 한정자 제거해주는 템플릿
// 포인터 타입은 제거가 안댐
template <typename T>
struct RemoveQulifier
{
	using Type = T;
};

template <typename T>
struct RemoveQulifier<const T>
{
	using Type = T;
};

template <typename T>
struct RemoveQulifier<volatile T>
{
	using Type = T;
};

template <typename T>
struct RemoveQulifier<volatile const T>
{
	using Type = T;
};

template <typename T>
using RemoveQulifier_t = typename RemoveQulifier<T>::Type;
/* =============================================================== */



/* =============================================================== */
// 배열 괄호 제거해서 기본 타입만 얻도록 하는 템플릿
template <typename T>
struct RemoveArray
{
	using Type = T;
};

template <typename T>
struct RemoveArray<T[]>
{
	using Type = T;
};

template <typename T, Int32U ArraySize>
struct RemoveArray<T[ArraySize]>
{
	using Type = T;
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
	using Type = T;
};

template <typename T>
struct RevmovePointer<T*>
{
	using Type = T;
};


template <typename T>
using RemovePointer_t = typename RevmovePointer<T>::Type;
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
	using Type = RemoveQulifier_t<RemoveReference_t<T>>;
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
	const String szFuncSig(__FUNCSIG__);

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

	if (szResult.StartWith("class ")) {
		return szResult.Remove("class ");
	} else if (szResult.StartWith("struct ")) {
		return szResult.Remove("struct ");
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



// Base 타입들이 모두 Derived를 자식으로 두고 있는지 검사
template <typename Derived, typename Base, typename... BaseArgs>
struct IsBaseOfMultipleBase
{
	static constexpr bool Value() {
		if constexpr (sizeof...(BaseArgs) == 0) {
			return IsBaseOf_v<Base, Derived>;
		} else {
			return IsBaseOf_v<Base, Derived> && IsBaseOfMultipleBase<Derived, BaseArgs...>::Value();
		}
	}
};

// Derived 타입들이 모두 Base를 부모로 두고 있는지 검사
template <typename Base, typename Derived, typename... DerivedArgs>
struct IsBaseOfMultipleDrived
{
	static constexpr bool Value() {
		if constexpr (sizeof...(DerivedArgs) == 0) {
			return IsBaseOf_v<Base, Derived>;
		} else {
			return IsBaseOf_v<Base, Derived> && IsBaseOfMultipleDrived<Base, DerivedArgs...>::Value();
		}
	}
};

// 템플릿 파리미터 팩(DerivedArgs)으로 전달한 타입들이 모두 Base의 자식인지 여부
/*
 테스트 코드(아래 6줄 그대로 복붙)
 struct A{}   ;
 struct B:A{} ;
 struct C:A{} ;
 struct D{}   ;
 IsBaseOf_1Base_MultipleDerived_v<A, B, B, C>; // true	B, B, C 모두 자식이 맞음
 IsBaseOf_1Base_MultipleDerived_v<A, B, B, D>; // false	B, B는 자식이 맞지만 D가 아님
 */

template <typename Base, typename... DerivedArgs>
constexpr const bool IsBaseOf_1Base_MultipleDerived_v 
	= IsBaseOfMultipleDrived<Base, DerivedArgs...>::Value();

// 템플릿 파라미터 팩(BaseArgs)으로 전달한 타입들이 모두 Derived의 부모인지 여부
/*
 테스트 코드(아래 8줄 그대로 복붙)
 struct A{}			;
 struct B{}			;
 struct C{}			;
 struct D:A,B,C{}	;
 struct E{}			;
 IsBaseOf_1Derived_MultipleBase_v<D, A, B>;		  // true	A, B 모두 D의 부모이므로 
 IsBaseOf_1Derived_MultipleBase_v<D, A, B, C>;	  // true	A, B, C 모두 D의 부모이므로 
 IsBaseOf_1Derived_MultipleBase_v<D, A, B, C, E>; // false   A, B, C 모두 D의 부모이지만 E는 아님
 */

template <typename Derived, typename... BaseArgs>
constexpr bool IsBaseOf_1Derived_MultipleBase_v = IsBaseOfMultipleBase<Derived, BaseArgs...>::Value();

template <typename T>
constexpr bool IsPrimitiveType_v = std::is_fundamental_v<T>;


template <typename T, typename... Rest>
struct IsAllPrimitiveType
{
	// 이건 왜 안되지;
	// static constexpr bool value = (sizeof...(Rest)) == 0 ? IsPrimitiveType_v<T> : IsPrimitiveType_v<T> && IsPrimitiveTypes<Rest...>::value;

	static constexpr bool Value() {
		if constexpr (sizeof...(Rest) == 0) {
			return IsPrimitiveType_v<T>;
		} else {
			return IsPrimitiveType_v<T> && IsAllPrimitiveType<Rest...>::Value();
		}
	}
};


template <typename T, typename... Rest>
constexpr bool IsAllPrimitiveType_v = IsAllPrimitiveType<T, Rest...>::Value();



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


template <typename From, typename To>
constexpr bool IsAssignable_v = std::is_assignable_v<From, To>;

template <typename From, typename To>
constexpr bool IsCopyAssignable_v = std::is_copy_assignable_v<From>;

template <typename From, typename To>
constexpr bool IsMoveAssignable_v = std::is_move_assignable_v<From>;

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
	return	kType == "char *"		|| kType == "JCore::String" ||
			kType == "wchar_t *"	|| (kType.StartWith("char [") && kType.EndWith("]")) ||
			(kType.StartWith("wchar_t [") && kType.EndWith("]"));
}
*/
/* ====================================================================== */




} // namespace JCore