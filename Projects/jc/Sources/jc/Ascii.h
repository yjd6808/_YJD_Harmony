/*
	작성자 : 윤정도
	아스키 문자 관련
*/

#pragma once

#include "jc/Namespace.h"

NS_JC_BEGIN

template <typename CharT>
constexpr bool IsAlphabat(const CharT _ch) {
	return _ch >= CharT('a') && _ch <= CharT('z') || _ch >= CharT('A') && _ch <= CharT('Z');
}

// 소문자 알파벳인지
template <typename CharT>
constexpr bool IsLowerCaseAlphabat(const CharT ch) {
	return ch >= CharT('a') && ch <= CharT('z');
}

// 대문자 알파벳인지
template <typename CharT>
constexpr bool IsUpperCaseAlphabat(const CharT ch) {
	return ch >= CharT('A') && ch <= CharT('Z');
}

template <typename CharT>
constexpr bool IsNumeric(const CharT ch) {
	return ch >= CharT('0') && ch <= CharT('9');
}

template <typename CharT>
constexpr bool IsSpace(const CharT ch) {
	return ch == CharT(' ') || ch == CharT('\t') || ch == CharT('\n') || ch == CharT('\v') || ch == CharT('\r') || ch == CharT('\f');
}

template <typename CharT>
constexpr CharT ToLower(const CharT ch) {
	return  (ch >= CharT('A') && ch <= CharT('Z'))
	? (ch - CharT('A') + CharT('a'))	// 대문자인 경우 소문자로 변경
	: ch;
}

template <typename CharT>
constexpr CharT ToUpper(const CharT ch) {
	return (ch >= CharT('a') && ch <= CharT('z'))
	? (ch - CharT('a') + CharT('A'))	// 소문자인 경우 대문자로 변경
	: ch;
}

NS_END
