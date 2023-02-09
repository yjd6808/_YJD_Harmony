/*
	작성자 : 윤정도
	비교 펑터
*/

#pragma once

#include <JCore/Primitives/StringUtil.h>
#include <JCore/Primitives/String.h>
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
};

template <>
struct Comparator<String>
{
	int operator()(const String& lhs, const String& rhs) const {
		return lhs.Compare(rhs);
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

NS_JC_END