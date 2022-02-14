/*
	작성자 : 윤정도
	비교 펑터
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
	int operator()(const char* lhs, const char* rhs) {
		return StringUtil::CTCompare(lhs, rhs);
	}
};

template <>
struct Comparator<String>
{
	int operator()(const String& lhs, const String& rhs) {
		return lhs.Compare(rhs);
	}
};

} // namespace JCore