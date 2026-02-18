/*
	작성자 : 윤정도
	비교 펑터
*/

#pragma once

#include <jc/Primitives/StringUtil.h>
#include <jc/Primitives/String.h>
#include <jc/Primitives/StaticString.h>

#include <jc/TypeTraits/IntegralConstant.h>
#include <jc/TypeCast.h>

NS_JC_BEGIN
template <typename T>
struct Comparator
{
	int operator()(const T& _lhs, const T& _rhs)
	{
		if (_lhs < _rhs)
		{
			return -1;
		}

		if (_lhs > _rhs)
		{
			return 1;
		}

		return 0;
	}
};

template <>
struct Comparator<const char*>
{
	int operator()(const char* _lhs, const char* _rhs) const
	{
		return StringUtil::CTCompare(_lhs, _rhs);
	}

	int operator()(const char* _lhs, const String& _rhs) const
	{
		return StringUtil::CTCompare(_lhs, _rhs.Source());
	}

	template <Int32U Size>
	int operator()(const char* _lhs, const StaticString<Size>& _rhs) const
	{
		return StringUtil::CTCompare(_lhs, _rhs.Source());
	}
};


// TODO: operator() 함수가 오버로딩 되어있어서 CallableSignatureParam_t<TComparator>::Count 호출시 0이 나오는 문제 수정
// decltype(&Fn::operator()) 실행시  cannot determine which instance of overloaded function is intended오류가 발생해서 강제로 SFINAE가 발동됨.
template <>
struct Comparator<String>
{
	int operator()(const String& _lhs, const String& _rhs)
	{
		return _lhs.Compare(_rhs);
	}

	int operator()(const String& _lhs, const char* _rhs)
	{
		return _lhs.Compare(_rhs);
	}

	template <Int32U Size>
	int operator()(const String& _lhs, const StaticString<Size>& _rhs)
	{
		return _lhs.Compare(_rhs.Source);
	}
};

struct NaturalOrder
{
	template <typename U>
	bool operator()(U&& _lhs, U&& _rhs)
	{
		return Comparator<NaturalType_t<U>>()(Forward<U>(_lhs), Forward<U>(_rhs)) < 0;
	}
};

struct ReverseOrder
{
	template <typename U>
	bool operator()(U&& _lhs, U&& _rhs)
	{
		return Comparator<NaturalType_t<U>>()(Forward<U>(_lhs), Forward<U>(_rhs)) > 0;
	}
};


// Comparator<Strng>의 경우 오버로딩된 케이스라서 IsCallable 체크시 false로 나와버린다. 어쩔수없이 Comparator 체크는 스킵하는걸로..
template <typename TComparator>
constexpr bool IsComparator_v = true;

//template <typename TComparator>
//constexpr bool IsComparator_v = IsCallable_v<TComparator>;


NS_END
