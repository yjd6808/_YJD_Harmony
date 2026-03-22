/*
	작성자 : 윤정도
	아스키 문자 관련
*/

#pragma once

NS_JC_BEGIN

constexpr bool IsAlphabat(const char _ch) {
	return _ch >= 'a' && _ch <= 'z' || _ch >= 'A' && _ch <= 'Z';
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

constexpr char ToLower(const char ch) {
	return  (ch >= 'A' && ch <= 'Z')
	? (ch - 'A' + 'a')	// 대문자인 경우 소문자로 변경
	: ch;
}

constexpr char ToUpper(const char ch) {
	return (ch >= 'a' && ch <= 'z')
	? (ch - 'a' + 'A')	// 소문자인 경우 대문자로 변경
	: ch;
}

NS_END

