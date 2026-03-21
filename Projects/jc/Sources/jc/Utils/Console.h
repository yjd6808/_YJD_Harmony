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


// @참고 https://learn.microsoft.com/en-us/windows/console/console-virtual-terminal-sequences
#define CSI "\x1b["
#define CSI_AND ";"

// 우선색상만..
#define CSI_GRAPHIC_RENDITION(x) CSI#x"m"
#define CSI_GRAPHIC_RENDITION_END "m"

#define VT_RESET                    "0"
#define VT_FORE_COLOR_BLACK         "30"
#define VT_FORE_COLOR_BLUE          "34"
#define VT_FORE_COLOR_GREEN         "32"
#define VT_FORE_COLOR_CYAN          "36"
#define VT_FORE_COLOR_RED           "31"
#define VT_FORE_COLOR_MAGNETA       "35"
#define VT_FORE_COLOR_YELLOW        "33"
#define VT_FORE_COLOR_LIGHT_GRAY    "1;39"
#define VT_FORE_COLOR_GRAY          "39"
#define VT_FORE_COLOR_LIGHT_BLUE    "94"
#define VT_FORE_COLOR_LIGHT_GREEN   "92"
#define VT_FORE_COLOR_LIGHT_CYAN    "96"
#define VT_FORE_COLOR_LIGHT_RED     "91"
#define VT_FORE_COLOR_LIGHT_MAGNETA "95"
#define VT_FORE_COLOR_LIGHT_YELLOW  "93"
#define VT_FORE_COLOR_WHITE         "97"

#define VT_BACK_COLOR_BLACK         "40"
#define VT_BACK_COLOR_BLUE          "44"
#define VT_BACK_COLOR_GREEN         "42"
#define VT_BACK_COLOR_CYAN          "46"
#define VT_BACK_COLOR_RED           "41"
#define VT_BACK_COLOR_MAGNETA       "45"
#define VT_BACK_COLOR_YELLOW        "43"
#define VT_BACK_COLOR_LIGHT_GRAY    "49"        // 배경은 회색 우째하지
#define VT_BACK_COLOR_GRAY          "49"        // 배경은 회색 우째하지
#define VT_BACK_COLOR_LIGHT_BLUE    "104"
#define VT_BACK_COLOR_LIGHT_GREEN   "102"
#define VT_BACK_COLOR_LIGHT_CYAN    "106"
#define VT_BACK_COLOR_LIGHT_RED     "101"
#define VT_BACK_COLOR_LIGHT_MAGNETA "105"
#define VT_BACK_COLOR_LIGHT_YELLOW  "103"
#define VT_BACK_COLOR_WHITE         "107"

NS_JC_BEGIN

struct ConsoleKeyInfo
{
    ConsoleKeyInfo()
	    : Key(ConsoleKey::None)
		, KeyChar(NULL)
		, Success(false) {}

    ConsoleKeyInfo(ConsoleKey _key, char _keyChar)
	    : Key(_key)
		, KeyChar(_keyChar)
		, Success(true) {}

    ConsoleKey Key;
    char KeyChar;
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
    static const char*   VTForeColor[ConsoleColor::Max];
    static const char*   VTBackColor[ConsoleColor::Max];

    static const char*   VTForeToken[ConsoleColor::Max];
    static const char*   VTBackToken[ConsoleColor::Max];
public:
    static bool Init();
    static bool SetSize(int _width, int _height);

    static void SetColor(ConsoleColor _color);
    static void GetColor(ConsoleColor _color);
    static ConsoleColor GetColor();
    static ConsoleColor ConvertColorString(const String& _colorString);

    template <typename... TArgs>
    static int Write(ConsoleColor _color, const char* _pFormat, TArgs&&... _args)
    {
        TLockGuard guard(ms_ConsoleLock);
        ConsoleColor prevColor = ms_iDefaultColor;
        SetColor(_color);
        int ret = Write(_pFormat, Forward<TArgs>(_args)...);
        SetColor(prevColor);
        return ret;
    }

    template <_u32 FormatBufferLen, typename... TArgs>
    static int Write(ConsoleColor _color, char(&_format)[FormatBufferLen], TArgs&&... _args)
    {
        TLockGuard guard(ms_ConsoleLock);
        ConsoleColor prevColor = ms_iDefaultColor;
        SetColor(_color);
        int ret = Write(_format, Forward<TArgs>(_args)...);
        SetColor(prevColor);
        return ret;
    }

    template <typename... TArgs>
    static int Write(const char* _pFormat, TArgs&&... _args)
    {
		char buf[TempBufferLen];
		int written = sprintf_s(buf, TempBufferLen, _pFormat, Forward<TArgs>(_args)...);
		if (written <= 0)
		{
			return written;
		}

		TLockGuard guard(ms_ConsoleLock);
		return printf_s("%s", buf);
    }

    template <_u32 FormatBufferLen, typename... TArgs>
    static int Write(char(&_format)[FormatBufferLen], TArgs&&... _args)
    {
        TLockGuard guard(ms_ConsoleLock);
        return printf_s(_format, Forward<TArgs>(_args)...);
    }

    static String ReadLine();
    static String ReadLine(const char* _pMsg);
    static int ReadLineBuffered(const char* _pMsg, char* _pBuff, int _capacity);

    static ConsoleKeyInfo ReadKey()
    {
        return ReadKey(nullptr);
    }

    static ConsoleKeyInfo ReadKey(const char* _pMsg);

    // 특정키를 입력받을때까지 체크
    static ConsoleKeyInfo ReadKeyWhile(ConsoleKey _key)
    {
        return ReadKeyWhile(nullptr, _key);
    }

    static ConsoleKeyInfo ReadKeyWhile(const char* _pMsg, ConsoleKey _key);

    template <typename... TArgs>
    static int WriteLine(ConsoleColor _color, const char* _pFormat, TArgs&&... _args)
    {
        TLockGuard guard(ms_ConsoleLock);
        ConsoleColor prevColor = ms_iDefaultColor;
        SetColor(_color);
        WriteLine(_pFormat, Forward<TArgs>(_args)...);
        SetColor(prevColor);
        return 0;
    }

    template <_u32 FormatBufferLen, typename... TArgs>
    static int WriteLine(ConsoleColor _color, char(&_format)[FormatBufferLen], TArgs&&... _args)
    {
        TLockGuard guard(ms_ConsoleLock);
        ConsoleColor prevColor = ms_iDefaultColor;
        SetColor(_color);
        int ret = WriteLine(_format, Forward<TArgs>(_args)...);
        SetColor(prevColor);
        return ret;
    }

    template <typename... TArgs>
    static int WriteLine(const char* _pFormat, TArgs&&... _args)
    {
        if constexpr (sizeof...(_args) == 0)
        {
            printf("%s\n", _pFormat);
            return 0;
        }
        else
        {
            TLockGuard guard(ms_ConsoleLock);
            return Math::Min(printf_s("\n"), printf_s(_pFormat, Forward<TArgs>(_args)...));
        }
    }

    template <_u32 FormatBufferLen, typename... TArgs>
    static int WriteLine(char(&_format)[FormatBufferLen], TArgs&&... _args)
    {
        TLockGuard guard(ms_ConsoleLock);
        return Math::Min(printf_s("\n"), printf_s(_format, Forward<TArgs>(_args)...));
    }

    static void Clear();
    static void SetCursorPosition(int _x, int _y);
    static Tuple<int, int> GetCursorPosition();

    static bool SetOutputCodePage(int _codePage);

	// 출력 스트림에 문자를 입력해서 콘솔을 제어할 수 있도록 버철 터미널 옵션을 켜주자.
    static bool SetEnableVTMode(bool _enabled);
    static int GetOutputCodePage();
};

NS_END



