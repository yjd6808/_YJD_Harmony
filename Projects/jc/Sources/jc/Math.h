/*
	작성자 : 윤정도
	간단한 수학 학수
*/

#pragma once

#include "jc/Namespace.h"

NS_JC_BEGIN

struct Math final
{
	template <typename T>
	static T Pow(T _base, const int _exponent)
	{
		if (_exponent == 0)
		{
			return 1;
		}

		T mul = _base;

		for (int i = 1; i < _exponent; i++)
		{
			_base *= mul;
		}

		return _base;
	}

	template <typename T>
	static constexpr T Max(const T _lhs, const T _rhs)
	{
		return _lhs > _rhs ? _lhs : _rhs;
	}

	template <typename T, typename... Args>
	static constexpr T Max(const T _arg0, const Args ... _args)
	{
		return Max(_arg0, Max(_args...));
	}

	template <typename T>
	static constexpr T Min(const T _lhs, const T _rhs)
	{
		return _lhs < _rhs ? _lhs : _rhs;
	}

	template <typename T, typename... Args>
	static constexpr T Min(const T _arg0, const Args ... _args)
	{
		return Min(_arg0, Min(_args...));
	}

	template <typename T>
	static constexpr T Abs(const T _arg)
	{
		return _arg < 0 ? _arg * -1 : _arg;
	}

	template <typename T>
	static constexpr T Clamp(const T _value, const T _min, const T _max)
	{
		return _value < _min ? _min : (_value > _max ? _max : _value);
	}

};

NS_END
