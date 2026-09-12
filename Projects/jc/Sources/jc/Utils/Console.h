/*
 * 작성자 : 윤정도
 */

#pragma once



#include "jc/Sync/RecursiveLock.h"
#include "jc/Sync/UnusedLock.h"

#include "jc/Utils/VirtualKey.h"
#include "jc/Utils/ConsoleColor.h"
#include "jc/Utils/ConsoleKey.h"

#include "jc/Wrapper/CRuntime.h"
#include "jc/Wrapper/WinApi.h"

#include "jc/Math.h"
#include "jc/Tuple.h"
#include "jc/Assert.h"
#include "jc/Primitives/String.h"


// @참고 https://learn.microsoft.com/en-us/windows/console/console-virtual-terminal-sequences
// VT 시퀀스는 ASCII 범위라 바이트 값은 동일하다. 문자셋을 따라가도록 _T()로 둔다.
#define CSI _T("\x1b[")
#define CSI_AND _T(";")

// 우선색상만..
#define CSI_GRAPHIC_RENDITION(x) CSI _T(#x) _T("m")
#define CSI_GRAPHIC_RENDITION_END _T("m")

#define VT_RESET                    _T("0")
#define VT_FORE_COLOR_BLACK         _T("30")
#define VT_FORE_COLOR_BLUE          _T("34")
#define VT_FORE_COLOR_GREEN         _T("32")
#define VT_FORE_COLOR_CYAN          _T("36")
#define VT_FORE_COLOR_RED           _T("31")
#define VT_FORE_COLOR_MAGNETA       _T("35")
#define VT_FORE_COLOR_YELLOW        _T("33")
#define VT_FORE_COLOR_LIGHT_GRAY    _T("1;39")
#define VT_FORE_COLOR_GRAY          _T("39")
#define VT_FORE_COLOR_LIGHT_BLUE    _T("94")
#define VT_FORE_COLOR_LIGHT_GREEN   _T("92")
#define VT_FORE_COLOR_LIGHT_CYAN    _T("96")
#define VT_FORE_COLOR_LIGHT_RED     _T("91")
#define VT_FORE_COLOR_LIGHT_MAGNETA _T("95")
#define VT_FORE_COLOR_LIGHT_YELLOW  _T("93")
#define VT_FORE_COLOR_WHITE         _T("97")

#define VT_BACK_COLOR_BLACK         _T("40")
#define VT_BACK_COLOR_BLUE          _T("44")
#define VT_BACK_COLOR_GREEN         _T("42")
#define VT_BACK_COLOR_CYAN          _T("46")
#define VT_BACK_COLOR_RED           _T("41")
#define VT_BACK_COLOR_MAGNETA       _T("45")
#define VT_BACK_COLOR_YELLOW        _T("43")
#define VT_BACK_COLOR_LIGHT_GRAY    _T("49")        // 배경은 회색 우째하지
#define VT_BACK_COLOR_GRAY          _T("49")        // 배경은 회색 우째하지
#define VT_BACK_COLOR_LIGHT_BLUE    _T("104")
#define VT_BACK_COLOR_LIGHT_GREEN   _T("102")
#define VT_BACK_COLOR_LIGHT_CYAN    _T("106")
#define VT_BACK_COLOR_LIGHT_RED     _T("101")
#define VT_BACK_COLOR_LIGHT_MAGNETA _T("105")
#define VT_BACK_COLOR_LIGHT_YELLOW  _T("103")
#define VT_BACK_COLOR_WHITE         _T("107")

NS_JC_BEGIN

// ConsoleMenuItem / ConsoleMenuItemOption 전방 선언 (Console::PrintMenu 에서 사용)
class ConsoleMenuItem;
struct ConsoleMenuItemOption;

struct ConsoleKeyInfo
{
    ConsoleKeyInfo()
    : Key(ConsoleKey::None)
	, KeyChar(_T('\0'))
	, Success(false) {}

    ConsoleKeyInfo(ConsoleKey _key, _char _keyChar)
    : Key(_key)
	, KeyChar(_keyChar)
	, Success(true) {}

    ConsoleKey Key;
    _char KeyChar;
    bool Success;           // 성공적으로 키입력을 받았는지.

    operator bool()
    {
        return Success;
    }
};

class Console
{
	using TLockGuard = RecursiveLockGuard;
    using TLock = RecursiveLock;

    inline static _whandle     ms_hStdout = (_whandle)-1;
    inline static _whandle     ms_hStdin = (_whandle)-1;
    inline static ConsoleColor  ms_iDefaultColor = LightGray;
    inline static TLock         ms_ConsoleLock{};
    inline static bool          ms_UseConsoleLock{};
    inline static int           ms_iCursorPosX{};
    inline static int           ms_iCursorPosY{};
    inline constexpr static int TempBufferLen = 1024;
public:
    static const _char*   VTForeColor[ConsoleColor::Max];
    static const _char*   VTBackColor[ConsoleColor::Max];

    static const _char*   VTForeToken[ConsoleColor::Max];
    static const _char*   VTBackToken[ConsoleColor::Max];
public:
    static bool Init();
    static bool SetSize(int _width, int _height);

	static void RestoreColor();
    static void SetColor(ConsoleColor _color);
    static ConsoleColor GetColor();
    static ConsoleColor ConvertColorString(const String& _colorString);

    template <typename... TArgs>
    static int Write(ConsoleColor _color, const _char* _pFormat, TArgs&&... _args)
    {
        TLockGuard guard(ms_ConsoleLock);
        ConsoleColor prevColor = ms_iDefaultColor;
        SetColor(_color);
        int ret = Write(_pFormat, Forward<TArgs>(_args)...);
        SetColor(prevColor);
        return ret;
    }

    template <_u32 FormatBufferLen, typename... TArgs>
    static int Write(ConsoleColor _color, _char(&_format)[FormatBufferLen], TArgs&&... _args)
    {
        TLockGuard guard(ms_ConsoleLock);
        ConsoleColor prevColor = ms_iDefaultColor;
        SetColor(_color);
        int ret = Write(_format, Forward<TArgs>(_args)...);
        SetColor(prevColor);
        return ret;
    }

    template <typename... TArgs>
    static int Write(const _char* _pFormat, TArgs&&... _args)
    {
		if constexpr (sizeof...(_args) == 0)
		{
			TLockGuard guard(ms_ConsoleLock);
			return _tprintf(_T("%s"), _pFormat);
		}
		else
		{
			_char buf[TempBufferLen];
			int written = _stprintf_s(buf, TempBufferLen, _pFormat, Forward<TArgs>(_args)...);
			if (written <= 0)
			{
				return written;
			}

			TLockGuard guard(ms_ConsoleLock);
			return _tprintf(_T("%s"), buf);
		}
    }

    template <_u32 FormatBufferLen, typename... TArgs>
    static int Write(_char(&_format)[FormatBufferLen], TArgs&&... _args)
    {
        TLockGuard guard(ms_ConsoleLock);
        return _tprintf(_format, Forward<TArgs>(_args)...);
    }

    static String ReadLine();
    static String ReadLine(const _char* _pMsg);
	static int ReadLineBuffered(_char* _pBuff, int _capacity) { return ReadLineBuffered(nullptr, _pBuff, _capacity); }
    static int ReadLineBuffered(const _char* _pMsg, _char* _pBuff, int _capacity);

    static ConsoleKeyInfo ReadKey()
    {
        return ReadKey(nullptr);
    }

    static ConsoleKeyInfo ReadKey(const _char* _pMsg);

    // 특정키를 입력받을때까지 체크
    static ConsoleKeyInfo ReadKeyWhile(ConsoleKey _key)
    {
        return ReadKeyWhile(nullptr, _key);
    }

    static ConsoleKeyInfo ReadKeyWhile(const _char* _pMsg, ConsoleKey _key);

    template <typename... TArgs>
    static int WriteLine(ConsoleColor _color, const _char* _pFormat, TArgs&&... _args)
    {
        TLockGuard guard(ms_ConsoleLock);
        ConsoleColor prevColor = ms_iDefaultColor;
        SetColor(_color);
        WriteLine(_pFormat, Forward<TArgs>(_args)...);
        SetColor(prevColor);
        return 0;
    }

    template <_u32 FormatBufferLen, typename... TArgs>
    static int WriteLine(ConsoleColor _color, _char(&_format)[FormatBufferLen], TArgs&&... _args)
    {
        TLockGuard guard(ms_ConsoleLock);
        ConsoleColor prevColor = ms_iDefaultColor;
        SetColor(_color);
        int ret = WriteLine(_format, Forward<TArgs>(_args)...);
        SetColor(prevColor);
        return ret;
    }

    template <typename... TArgs>
    static int WriteLine(const _char* _pFormat = nullptr, TArgs&&... _args)
    {
        if constexpr (sizeof...(_args) == 0)
        {
			if (_pFormat == nullptr || _pFormat[0] == _T('\0'))
			{
				_puttchar(_T('\n'));
			}
			else
			{
				_tprintf(_T("%s\n"), _pFormat);
			}
            return 0;
        }
        else
        {
            TLockGuard guard(ms_ConsoleLock);
            return Math::Min(_tprintf(_T("\n")), _tprintf(_pFormat, Forward<TArgs>(_args)...));
        }
    }

    template <_u32 FormatBufferLen, typename... TArgs>
    static int WriteLine(_char(&_format)[FormatBufferLen], TArgs&&... _args)
    {
        TLockGuard guard(ms_ConsoleLock);
        return Math::Min<int>(_puttchar(_T('\n')), _tprintf(_format, Forward<TArgs>(_args)...));
    }

    static void Clear();
    static void SetCursorPosition(int _x, int _y);
    static Tuple<int, int> GetCursorPosition();

    static bool SetOutputCodePage(int _codePage);

	// 출력 스트림에 문자를 입력해서 콘솔을 제어할 수 있도록 버철 터미널 옵션을 켜주자.
    static bool SetEnableVTMode(bool _enabled);
    static int GetOutputCodePage();

    // ConsoleMenuItem 트리를 이용한 인터랙티브 메뉴 루프 실행
    // _pOption 이 nullptr 이면 기본 옵션 사용
    static void PrintMenu(ConsoleMenuItem* _pItem, ConsoleMenuItemOption* _pOption = nullptr);
};

NS_END



