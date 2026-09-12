/*
 * 작성자: 윤정도
 * 생성일: 9/6/2026
 * =====================
 * AString(UTF-8) <-> WString(UTF-16) 트랜스코딩 구현
 */

#include "jc/Primitives/StringConvert.h"
#include "jc/Primitives/StringUtil.h"
#include "jc/Memory.h"
#include "jc/Assert.h"
#include "jc/Wrapper/WinApi.h"

#include <cstring>

NS_JC_BEGIN

////////////////////////////////////////////////////////////////////////////////////////
WString StringConvert::ToWide(const char* _pStr, int _len)
{
	if (_pStr == nullptr)
		return WString();

	int charCount = _len;

	if (_len < 0)
		charCount = StringUtilA::Length(_pStr);

	if (charCount == 0)
		return WString();

	int wlen = ::MultiByteToWideChar(
		CP_UTF8,
		0,
		_pStr,
		charCount,
		nullptr,
		0);

	if (wlen <= 0)
	{
		jc_assert_msg(false, "MultiByteToWideChar size 계산 실패. 오류 코드: %d", GetLastError());
		return WString();
	}

	wchar_t* buf = Memory::Allocate<wchar_t*>(static_cast<_u32>((wlen + 1) * sizeof(wchar_t)));

	int r = ::MultiByteToWideChar(
		CP_UTF8,
		0,
		_pStr,
		charCount,
		buf,
		wlen);

	if (r <= 0)
	{
		jc_assert_msg(false, "MultiByteToWideChar 변환 실패. 오류 코드: %d", GetLastError());
		Memory::Deallocate(buf);
		return WString();
	}

	buf[r] = L'\0';

	WString result(0);
	result.ExchangeSource(buf, r);
	return result;
}

////////////////////////////////////////////////////////////////////////////////////////
WString StringConvert::ToWide(const AString& _str)
{
	return ToWide(_str.SafeSource(), _str.Length());
}

////////////////////////////////////////////////////////////////////////////////////////
AString StringConvert::ToUtf8(const wchar_t* _pStr, int _len)
{
	if (_pStr == nullptr)
		return AString();

	int wcharCount = _len;

	// null terminated 처리
	if (_len < 0)
		wcharCount = static_cast<int>(wcslen(_pStr));

	if (wcharCount == 0)
		return AString();

	// 필요한 UTF8 길이 계산
	int size = WideCharToMultiByte(
		CP_UTF8,
		0,
		_pStr,
		wcharCount,
		nullptr,
		0,
		nullptr,
		nullptr);

	if (size <= 0)
	{
		jc_assert_msg(false, "WideCharToMultiByte size 계산 실패. 오류 코드: %d", GetLastError());
		return AString();
	}

	char* buf = Memory::Allocate<char*>(static_cast<_u32>(size + 1));

	int r = WideCharToMultiByte(
		CP_UTF8,
		0,
		_pStr,
		wcharCount,
		buf,
		size,
		nullptr,
		nullptr);

	if (r <= 0)
	{
		jc_assert_msg(false, "WideCharToMultiByte 변환 실패. 오류 코드: %d", GetLastError());
		Memory::Deallocate(buf);
		return AString();
	}

	buf[r] = '\0';

	AString result(0);
	result.ExchangeSource(buf, r);
	return result;
}

////////////////////////////////////////////////////////////////////////////////////////
AString StringConvert::ToUtf8(const WString& _str)
{
	return ToUtf8(_str.Source(), _str.Length());
}

////////////////////////////////////////////////////////////////////////////////////////
AString StringConvert::ToUtf8(const AString& _str)
{
	return _str;
}

////////////////////////////////////////////////////////////////////////////////////////
String StringConvert::FromUtf8(const char* _pStr, int _len)
{
#ifdef _UNICODE
	return ToWide(_pStr, _len);
#else
	if (_pStr == nullptr)
		return String();
	if (_len < 0)
		return String(_pStr);
	if (_len == 0)
		return String();
	String str(0);
	str.Append(_pStr, _len);
	return str;
#endif
}

////////////////////////////////////////////////////////////////////////////////////////
String StringConvert::FromUtf8(const AString& _str)
{
#ifdef _UNICODE
	return ToWide(_str);
#else
	return _str;
#endif
}

////////////////////////////////////////////////////////////////////////////////////////
String StringConvert::FromAnsi(const char* _pStr, int _len)
{
	if (_pStr == nullptr)
		return String();
#ifdef _UNICODE
	int charCount = _len < 0 ? static_cast<int>(strlen(_pStr)) : _len;
	if (charCount == 0)
		return String();
	int wlen = ::MultiByteToWideChar(CP_ACP, 0, _pStr, charCount, nullptr, 0);
	if (wlen <= 0)
		return String();
	WString wstr;
	wstr.Resize(wlen);
	int r = ::MultiByteToWideChar(CP_ACP, 0, _pStr, charCount,
		const_cast<wchar_t*>(wstr.Source()), wlen);
	if (r <= 0)
		return String();
	wstr.SetLength(r);
	wstr.Source()[r] = 0;
	return wstr;
#else
	if (_len < 0)
		return String(_pStr);
	if (_len == 0)
		return String();
	String str(0);
	str.Append(_pStr, _len);
	return str;
#endif
}

////////////////////////////////////////////////////////////////////////////////////////
String StringConvert::FromAnsi(const AString& _str)
{
#ifdef _UNICODE
	return FromAnsi(_str.Source(), _str.Length());
#else
	return _str;
#endif
}

////////////////////////////////////////////////////////////////////////////////////////
AString StringConvert::ToAnsi(const String& _str)
{
#ifdef _UNICODE
	if (_str.IsEmpty())
		return AString();
	int bytes = ::WideCharToMultiByte(CP_ACP, 0, _str.Source(), _str.Length(), nullptr, 0, nullptr, nullptr);
	if (bytes <= 0)
		return AString();
	AString out;
	out.Resize(bytes);
	int r = ::WideCharToMultiByte(CP_ACP, 0, _str.Source(), _str.Length(),
		const_cast<char*>(out.Source()), bytes, nullptr, nullptr);
	if (r <= 0)
		return AString();
	out.SetLength(r);
	out.Source()[r] = '\0';
	return out;
#else
	return _str;
#endif
}

NS_END
