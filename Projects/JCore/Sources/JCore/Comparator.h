/*
	작성자 : 윤정도
	비교 펑터
*/

#pragma once

#include <JCore/Primitives/StringUtil.h>
#include <JCore/Primitives/String.h>
#include <JCore/Primitives/StaticString.h>

#include <JCore/TypeTraits/IntegralConstant.h>
#include <JCore/TypeCast.h>

NS_JC_BEGIN
template <typename T>
struct Comparator
{
	int operator()(const T& lhs, const T& rhs) {
		if (lhs < rhs) {
			return -1;
		}

	    if (lhs > rhs) {
			return 1;
		}

		return 0;
	}
};

template <>
struct Comparator<const char*>
{
	int operator()(const char* lhs, const char* rhs) const {
		return StringUtil::CTCompare(lhs, rhs);
	}

	int operator()(const char* lhs, const String& rhs) const {
		return StringUtil::CTCompare(lhs, rhs.Source());
	}

	template <Int32U Size>
	int operator()(const char* lhs, const StaticString<Size>& rhs) const {
		return StringUtil::CTCompare(lhs, rhs.Source());
	}
};


// TODO: operator() 함수가 오버로딩 되어있어서 CallableSignatureParam_t<TComparator>::Count 호출시 0이 나오는 문제 수정
// decltype(&Fn::operator()) 실행시  cannot determine which instance of overloaded function is intended오류가 발생해서 강제로 SFINAE가 발동됨.
template <>
struct Comparator<String>
{
	int operator()(const String& lhs, const String& rhs) {
		return lhs.Compare(rhs);
	}

	int operator()(const String& lhs, const char* rhs) {
		return lhs.Compare(rhs);
	}

	template <Int32U Size>
	int operator()(const String& lhs, const StaticString<Size>& rhs) {
		return lhs.Compare(rhs.Source);
	}
};

struct NaturalOrder
{
	template <typename U>
	bool operator()(U&& lhs, U&& rhs) {
		return Comparator<NaturalType_t<U>>()(Forward<U>(lhs), Forward<U>(rhs)) < 0;
	}
};

struct ReverseOrder
{
	template <typename U>
	bool operator()(U&& lhs, U&& rhs) {
		return Comparator<NaturalType_t<U>>()(Forward<U>(lhs), Forward<U>(rhs)) > 0;
	}
};


// Comparator<Strng>의 경우 오버로딩된 케이스라서 IsCallable 체크시 false로 나와버린다. 어쩔수없이 Comparator 체크는 스킵하는걸로..
template <typename TComparator>
constexpr bool IsComparator_v = true;

//template <typename TComparator>
//constexpr bool IsComparator_v = IsCallable_v<TComparator>;





NS_JC_END