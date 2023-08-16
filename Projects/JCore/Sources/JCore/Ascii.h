/*
	작성자 : 윤정도
	아스키 문자 관련
*/

#pragma once

NS_JC_BEGIN

constexpr bool IsAlphabat(const char ch) {
	return ch >= 'a' && ch <= 'z' || ch >= 'A' && ch <= 'Z';
}

// 소문자 알파벳인지
constexpr bool IsLowerCaseAlphabat(const char ch) {
	return ch >= 'a' && ch <= 'z';
}

// 대문자 알파벳인지
constexpr bool IsUpperCaseAlphabat(const char ch) {
	return ch >= 'A' && ch <= 'Z';
}

constexpr bool IsNumeric(const char ch) {
	return ch >= '0' && ch <= '9';
}

constexpr bool IsSpace(const char ch) {
	return ch == ' ' || ch == '\t' || ch == '\n' || ch == '\v' || ch == '\r' || ch == '\f';
}

NS_JC_END

