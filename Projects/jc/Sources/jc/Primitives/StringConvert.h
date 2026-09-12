/*
 * 작성자: 윤정도
 * 생성일: 9/6/2026
 * =====================
 * AString(UTF-8 narrow) <-> WString(UTF-16 wide) 변환 + W API 경계 도우미
 *
 * CharT 간 변환은 복사가 아니라 트랜스코딩이다. 반드시 explicit 함수로만 오간다.
 * Windows API(W) 호출부는 이 헤더의 함수로 경계를 넘는다.
 * 코드 페이지는 UTF-8로 고정한다 (JC_DEFAULT_CODE_PAGE와 동일).
 */

#pragma once

#include "jc/Primitives/String.h"
#include "jc/Primitives/StringImpl_SSO.h"

NS_JC_BEGIN

class StringConvert final
{
public:
	StringConvert() = delete;

	static WString ToWide(const char* _pStr, int _len = -1);
	static WString ToWide(const AString& _str);
	static AString ToUtf8(const wchar_t* _pStr, int _len);
	static AString ToUtf8(const WString& _str);
	static AString ToUtf8(const AString& _str);

	// narrow(UTF-8) 바이트 -> String. 프로젝트 문자셋(_UNICODE)을 따라간다.
	// narrow 외부 입력(argv, 와이어, 파일 내용)을 String으로 들일 때의 유일한 통로다.
	static String FromUtf8(const char* _pStr, int _len = -1);
	static String FromUtf8(const AString& _str);

	// OS narrow(ANSI 코드페이지) 바이트 <-> String. WinAPI-A, argv, 파일시스템 narrow용.
	static String FromAnsi(const char* _pStr, int _len = -1);
	static String FromAnsi(const AString& _str);
	static AString ToAnsi(const String& _str);
};

NS_END
