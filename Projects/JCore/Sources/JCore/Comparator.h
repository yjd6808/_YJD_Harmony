/*
	¿€º∫¿⁄ : ¿±¡§µµ
	∫Ò±≥ ∆„≈Õ
*/

#pragma once

#include <JCore/StringUtil.h>
#include <JCore/String.h>

namespace JCore {

template <typename T>
struct Comparator
{
	int operator()(const T& lhs, const T& rhs) {
		if (lhs < rhs) {
			return -1;
		} else if (lhs > rhs) {
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
		return Comparator<NaturalType_t<U>>()(static_cast<U&&>(lhs), static_cast<U&&>(rhs)) < 0;
	}
};

struct ReverseOrder
{
	template <typename U>
	bool operator()(U&& lhs, U&& rhs) {
		return Comparator<NaturalType_t<U>>()(static_cast<U&&>(lhs), static_cast<U&&>(rhs)) > 0;
	}
};





} // namespace JCores