/*
 * 작성자 : 윤정도
 * 목적 : MASM 어셈블리 코드에서 Console API를 호출하기 위한 extern "C" 래퍼 구현
 */

#include "Extern_Console.h"
#include "jc/Utils/Console.h"
#include "jc/Primitives/StringConvert.h"

USING_NS_JC;

//////////////////////////////////////////////////////////////////////////////////////////
_s32 Console_Init()
{
	return Console::Init() ? 1 : 0;
}

//////////////////////////////////////////////////////////////////////////////////////////
_s32 Console_SetSize(_s32 _width, _s32 _height)
{
	return Console::SetSize(_width, _height) ? 1 : 0;
}

//////////////////////////////////////////////////////////////////////////////////////////
void Console_RestoreColor()
{
	Console::RestoreColor();
}

//////////////////////////////////////////////////////////////////////////////////////////
void Console_SetColor(_s32 _color)
{
	Console::SetColor(static_cast<ConsoleColor>(_color));
}

//////////////////////////////////////////////////////////////////////////////////////////
_s32 Console_GetColor()
{
	return Console::GetColor();
}

//////////////////////////////////////////////////////////////////////////////////////////
_s32 Console_Write(const _s8* _pFmt)
{
	// C-ABI narrow 경계: ANSI 바이트를 String으로 들여서 TCHAR 출력으로 넘긴다.
	String msg = StringConvert::FromAnsi(_pFmt);
	return Console::Write(_T("%s"), msg.Source());
}

//////////////////////////////////////////////////////////////////////////////////////////
_s32 Console_WriteLine(const _s8* _pFmt)
{
	// C-ABI narrow 경계: ANSI 바이트를 String으로 들여서 TCHAR 출력으로 넘긴다.
	String msg = StringConvert::FromAnsi(_pFmt);
	return Console::WriteLine(_T("%s"), msg.Source());
}

//////////////////////////////////////////////////////////////////////////////////////////
_s32 Console_WriteFmt(const _s8* _pFmt, ...)
{
	va_list args;
	va_start(args, _pFmt);

	_s8 buf[1024];
	_s32 written = vsprintf_s(buf, sizeof(buf), _pFmt, args);
	va_end(args);

	if (written <= 0)
		return written;

	// C-ABI narrow 경계: narrow로 포맷한 뒤 String으로 들여서 넘긴다.
	String msg = StringConvert::FromAnsi(buf);
	return Console::Write(_T("%s"), msg.Source());
}

//////////////////////////////////////////////////////////////////////////////////////////
_s32 Console_WriteLineFmt(const _s8* _pFmt, ...)
{
	va_list args;
	va_start(args, _pFmt);

	_s8 buf[1024];
	_s32 written = vsprintf_s(buf, sizeof(buf), _pFmt, args);
	va_end(args);

	if (written <= 0)
		return written;

	// C-ABI narrow 경계: narrow로 포맷한 뒤 String으로 들여서 넘긴다.
	String msg = StringConvert::FromAnsi(buf);
	return Console::WriteLine(_T("%s"), msg.Source());
}

//////////////////////////////////////////////////////////////////////////////////////////
_s32 Console_ReadLineBuffered(const _s8* _pMsg, _s8* _pBuffer, _s32 _capacity)
{
	if (_pBuffer == nullptr || _capacity <= 0)
		return -1;

	// C-ABI narrow 경계: 프롬프트는 ANSI→String, 입력은 String으로 읽어 ANSI로 되돌린다.
	String line;
	if (_pMsg != nullptr)
	{
		String msg = StringConvert::FromAnsi(_pMsg);
		line = Console::ReadLine(msg.Source());
	}
	else
	{
		line = Console::ReadLine();
	}

	AString narrow = StringConvert::ToAnsi(line);
	int copyLen = narrow.Length();
	if (copyLen >= _capacity)
		copyLen = _capacity - 1;

	const char* pSrc = narrow.Source();
	for (int i = 0; i < copyLen; ++i)
		_pBuffer[i] = pSrc[i];
	_pBuffer[copyLen] = '\0';
	return copyLen;
}

//////////////////////////////////////////////////////////////////////////////////////////
void Console_Clear(void)
{
	Console::Clear();
}

//////////////////////////////////////////////////////////////////////////////////////////
void Console_SetCursorPosition(_s32 _x, _s32 _y)
{
	Console::SetCursorPosition(_x, _y);
}

//////////////////////////////////////////////////////////////////////////////////////////
void Console_GetCursorPosition(_s32* _pX, _s32* _pY)
{
	if (_pX == nullptr || _pY == nullptr)
		return;

	auto pos = Console::GetCursorPosition();
	*_pX = pos.item1;
	*_pY = pos.item2;
}

//////////////////////////////////////////////////////////////////////////////////////////
_s32 Console_SetOutputCodePage(_s32 _codePage)
{
	return Console::SetOutputCodePage(_codePage) ? 1 : 0;
}

//////////////////////////////////////////////////////////////////////////////////////////
_s32 Console_GetOutputCodePage(void)
{
	return Console::GetOutputCodePage();
}

//////////////////////////////////////////////////////////////////////////////////////////
_s32 Console_SetEnableVTMode(_s32 _enabled)
{
	return Console::SetEnableVTMode(_enabled != 0) ? 1 : 0;
}
