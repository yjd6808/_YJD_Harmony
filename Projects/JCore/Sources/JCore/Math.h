/*
	작성자 : 윤정도
	간단한 수학 학수
*/

#pragma once

NS_JC_BEGIN

struct Math final
{
	template <typename T>
	static T Pow(T base, const int exponent) {
		if (exponent == 0) {
			return 1;
		}

		T mul = base;

		for (int i = 1; i < exponent; i++) {
			base *= mul;
		}

		return base;
	}

	template <typename T>
	static constexpr T Max(const T lhs, const T rhs) {
		return lhs > rhs ? lhs : rhs;
	}

	template <typename T, typename... Args>
	static constexpr T Max(const T arg0, const Args... args) {
		return Max(arg0, Max(args...));
	}

	template <typename T>
	static constexpr T Min(const T lhs, const T rhs) {
		return lhs < rhs ? lhs : rhs;
	}

	template <typename T, typename... Args>
	static constexpr T Min(const T arg0, const Args... args) {
		return Min(arg0, Min(args...));
	}

	template <typename T>
	static constexpr T Abs(const T arg) {
		return arg < 0 ? arg * -1 : arg;
	}
};

NS_JC_END
