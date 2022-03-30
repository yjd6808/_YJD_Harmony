#pragma once

#include <JCore/String.h>
#include <JCore/StringUtil.h>
#include <JCore/StaticString.h>

#include <type_traits>

namespace JCore {

/* =============================================================== */
// ���� Ÿ������ ���� �ֵ��� �ϴ� ���ø�
template <typename T, T Value>
struct IntegralConstant { static constexpr T value = Value; };

struct TrueType : IntegralConstant<bool, true> {};
struct FalseType : IntegralConstant<bool, false> {};
/* =============================================================== */



/* =============================================================== */
// ������ �������ִ� ���ø�
template <typename T>
struct RemoveReference
{
	using Type = T;
};

template <typename T>
struct RemoveReference<T&>
{
	using Type = T;
};

template <typename T>
struct RemoveReference<T&&>
{
	using Type = T;
};

template <typename T>
using RemoveReference_t = typename RemoveReference<T>::Type;
/* =============================================================== */



/* =============================================================== */
// ������ �������ִ� ���ø�
// ������ Ÿ���� ���Ű� �ȴ�
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
// �迭 ��ȣ �����ؼ� �⺻ Ÿ�Ը� �򵵷� �ϴ� ���ø�
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
// ������ �������ִ� ���ø�
// ������ Ÿ���� ���Ű� �ȴ�
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
// ���� �ø�ƽ�� �Ϻ��� ����
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
// ������ Ÿ������ Ȯ���ϴ� ���ø�
template <typename T> 
struct IsPointerType : FalseType {};

template <typename T> 
struct IsPointerType<T*> : TrueType {};

template <typename T>
constexpr bool IsPointerType_v = IsPointerType<T>::value;
/* =============================================================== */


/* =============================================================== */
// ���� Ÿ������ Ȯ���ϴ� ���ø�
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
// ������, ����Ÿ���� ��� �������ִ� ���ø�
// ex) const voaltile int&& -> intŸ�����δ�
template <typename T>
struct NaturalType
{
	using Type = RemoveQulifier_t<RemoveReference_t<T>>;
};

template <typename T>
using NaturalType_t = typename NaturalType<T>::Type;
/* =============================================================== */



/* =============================================================== */
// ���� Ÿ������ Ȯ���ϴ� ���ø�
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
// �Ǽ� Ÿ������ Ȯ���ϴ� ���ø�
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
// ������ Ÿ������ Ȯ���ϴ� ���ø�
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
// ���ڿ� Ÿ������ Ȯ���ϴ� ���ø�

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
// ���ڿ� Ÿ������ Ȯ���ϴ� ���ø�

template <typename T>
struct IsArrayType : FalseType {};

template <typename T>
struct IsArrayType<T[]> : TrueType {};

template <typename T, Int32U ArraySize>
struct IsArrayType<T[ArraySize]> : TrueType {};

template <typename T>
constexpr bool IsArrayType_v = IsArrayType<NaturalType_t<T>>::value;
/* =============================================================== */


// ��Ÿ�ӿ� Ÿ�Ը��� ������ �� ����մϴ�.
template <typename T>
String Type() {
	const String szFuncSig(__FUNCSIG__);

	const int kiLeftIdx = szFuncSig.Find("<");
	const int kiRightIdx = szFuncSig.FindReverse(">");

	return szFuncSig.GetRange(kiLeftIdx + 1, kiRightIdx - 1);
}

// ������Ÿ�ӿ� Ÿ�Ը��� ������ �� ����մϴ�.
// ��ȯŸ�� : StaticString<Size>
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

// From���� To�� �Ϲ��� ��ȯ�� ��������
// �� To ? = From�� ��������
// @���� : https://docs.microsoft.com/ko-kr/cpp/standard-library/is-convertible-class?view=msvc-170
template <typename From, typename To>
constexpr bool IsConvertible_v = std::is_convertible_v<From, To>;


template <typename Base, typename Derived>
constexpr bool IsBaseOf_v = std::is_base_of_v<Base, Derived>;



// Base Ÿ�Ե��� ��� Derived�� �ڽ����� �ΰ� �ִ��� �˻�
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

// Derived Ÿ�Ե��� ��� Base�� �θ�� �ΰ� �ִ��� �˻�
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

// ���ø� �ĸ����� ��(DerivedArgs)���� ������ Ÿ�Ե��� ��� Base�� �ڽ����� ����
/*
 �׽�Ʈ �ڵ�(�Ʒ� 6�� �״�� ����)
 struct A{}   ;
 struct B:A{} ;
 struct C:A{} ;
 struct D{}   ;
 IsBaseOf_1Base_MultipleDerived_v<A, B, B, C>; // true	B, B, C ��� �ڽ��� ����
 IsBaseOf_1Base_MultipleDerived_v<A, B, B, D>; // false	B, B�� �ڽ��� ������ D�� �ƴ�
 */

template <typename Base, typename... DerivedArgs>
constexpr const bool IsBaseOf_1Base_MultipleDerived_v 
	= IsBaseOfMultipleDrived<Base, DerivedArgs...>::Value();

// ���ø� �Ķ���� ��(BaseArgs)���� ������ Ÿ�Ե��� ��� Derived�� �θ����� ����
/*
 �׽�Ʈ �ڵ�(�Ʒ� 8�� �״�� ����)
 struct A{}			;
 struct B{}			;
 struct C{}			;
 struct D:A,B,C{}	;
 struct E{}			;
 IsBaseOf_1Derived_MultipleBase_v<D, A, B>;		  // true	A, B ��� D�� �θ��̹Ƿ� 
 IsBaseOf_1Derived_MultipleBase_v<D, A, B, C>;	  // true	A, B, C ��� D�� �θ��̹Ƿ� 
 IsBaseOf_1Derived_MultipleBase_v<D, A, B, C, E>; // false   A, B, C ��� D�� �θ������� E�� �ƴ�
 */

template <typename Derived, typename... BaseArgs>
constexpr bool IsBaseOf_1Derived_MultipleBase_v = IsBaseOfMultipleBase<Derived, BaseArgs...>::Value();

template <typename T>
constexpr bool IsPrimitiveType_v = std::is_fundamental_v<T>;


template <typename T, typename... Rest>
struct IsAllPrimitiveType
{
	// �̰� �� �ȵ���;
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



// ���� ���̳��� ĳ������ �������� �˷��ִ� �༮
// ���ø� ���ڷ� ������ �༮�� �� ��� ������ Ÿ�� �Ǵ� ���۷��� Ÿ���� ���
// ���� Ÿ���� ���� ���̳��� ĳ������ �ȵ����� ���� Ÿ�Գ����� �� �ȴٰ� ó��
template <typename Lhs, typename Rhs>
constexpr bool DynamicCastable_v = (IsPointerType_v<Lhs> && IsPointerType_v<Rhs>)  ||
								   (IsReferenceType_v<Lhs> && IsReferenceType_v<Rhs>) 
								   ?
										IsSameType_v<Lhs, Rhs>	// �׳� ���� ������ Ÿ���̸� ������ OK
										? 
										true
										:
										IsBaseOf_v<RemovePointer_t<RemoveReference_t<Lhs>>, RemovePointer_t<RemoveReference_t<Rhs>>> ||
										IsBaseOf_v<RemovePointer_t<RemoveReference_t<Rhs>>, RemovePointer_t<RemoveReference_t<Lhs>>>
								   :
								   false;

// From�� To�� �θ� Ÿ������ ������, ����Ÿ�� �ΰ�쿡 ��
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
// ���� ó���� ������ ����, �Ǽ�, ���ڿ� Ÿ�� üũ �Լ�
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