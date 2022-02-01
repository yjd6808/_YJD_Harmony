#pragma once

/*
	작성자 : 윤정도
	아스키 문자 관련
*/

namespace JCore {
	namespace Ascii {
		inline bool IsAlphabat(const char ch) {
			return ch >= 'a' && ch <= 'z' || ch >= 'A' && ch <= 'Z';
		}

		inline bool IsSpace(const char ch) {
			return ch == ' ' || ch == '\t' || ch == '\n' || ch == '\v' || ch == '\r' || ch == '\f';
		}
	}
} // namespace JCore