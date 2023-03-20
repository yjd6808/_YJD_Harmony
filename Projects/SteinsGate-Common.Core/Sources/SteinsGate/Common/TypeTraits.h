/*
 * 작성자: 윤정도
 * 생성일: 3/20/2023 8:08:37 PM
 * =====================
 *
 */


#pragma once

template <typename T, typename... Args>
constexpr bool IsAllSameType() {
	bool result = false;
	std::initializer_list<T>{ (result &= std::is_same_v<T, Args>)... };
	return result;
}