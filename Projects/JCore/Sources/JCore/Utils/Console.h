/*
 * 작성자 : 윤정도
 */

#pragma once



#include <JCore/Sync/RecursiveLock.h>
#include <JCore/Sync/UnusedLock.h>
#include <JCore/Utils/ConsoleColor.h>

#include <JCore/Wrapper/CRuntime.h>
#include <JCore/Wrapper/WinApi.h>

#include <JCore/Math.h>
#include <JCore/Tuple.h>
#include <JCore/Assert.h>


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

class Console
{
	using TLockGuard = RecursiveLockGuard;
    using TLock = RecursiveLock;

    inline static WinHandle     ms_hStdout = (WinHandle)-1;
    inline static ConsoleColor	ms_iDefaultColor = LightGray;
    inline static TLock         ms_ConsoleLock{};
    inline static bool          ms_UseConsoleLock{};
    inline static int           ms_iCursorPosX{};
    inline static int           ms_iCursorPosY{};
    inline static int           TempBufferLen = 1024;
public:
    static const char*   VTForeColor[ConsoleColor::Max];
    static const char*   VTBackColor[ConsoleColor::Max];

    static const char*   VTForeToken[ConsoleColor::Max];
    static const char*   VTBackToken[ConsoleColor::Max];
public:
    static bool Init();
    static bool SetSize(int width, int height);

    static void SetColor(ConsoleColor color);
    static void GetColor(ConsoleColor color);
    static ConsoleColor GetColor();

    template <typename... TArgs>
    static int Write(ConsoleColor color, char* format, TArgs&&... args) {
        TLockGuard guard(ms_ConsoleLock);
        ConsoleColor prevColor = ms_iDefaultColor;
        SetColor(color);
        int iRet = Write(format, Forward<TArgs>(args)...);
        SetColor(prevColor);
        return iRet;
    }

    template <Int32U FormatBufferLen, typename... TArgs>
    static int Write(ConsoleColor color, const char(&format)[FormatBufferLen], TArgs&&... args) {
        TLockGuard guard(ms_ConsoleLock);
        ConsoleColor prevColor = ms_iDefaultColor;
        SetColor(color);
        int iRet = Write(format, Forward<TArgs>(args)...);
        SetColor(prevColor);
        return iRet;
    }

    template <typename... TArgs>
    static int Write(char* format, TArgs&&... args) {
        char buf[TempBufferLen];
        sprintf_s(buf, TempBufferLen, format);
        TLockGuard guard(ms_ConsoleLock);
        return printf_s(buf, Forward<TArgs>(args)...);
    }

    template <Int32U FormatBufferLen, typename... TArgs>
    static int Write(const char(&format)[FormatBufferLen], TArgs&&... args) {
        TLockGuard guard(ms_ConsoleLock);
        return printf_s(format, Forward<TArgs>(args)...);
    }

    static String ReadLine();

    template <typename... TArgs>
    static int WriteLine(ConsoleColor color, char* format, TArgs&&... args) {
        TLockGuard guard(ms_ConsoleLock);
        ConsoleColor prevColor = ms_iDefaultColor;
        SetColor(color);
        WriteLine(format, Forward<TArgs>(args)...);
        SetColor(prevColor);
        return 0;
    }

    template <Int32U FormatBufferLen, typename... TArgs>
    static int WriteLine(ConsoleColor color, const char(&format)[FormatBufferLen], TArgs&&... args) {
        TLockGuard guard(ms_ConsoleLock);
        ConsoleColor prevColor = ms_iDefaultColor;
        SetColor(color);
        int iRet = WriteLine(format, Forward<TArgs>(args)...);
        SetColor(prevColor);
        return iRet;
    }

    template <typename... TArgs>
    static int WriteLine(char* format, TArgs&&... args) {
        
        char buf[TempBufferLen + 1];
        sprintf_s(buf, TempBufferLen + 1, format);
        TLockGuard guard(ms_ConsoleLock);
        return Math::Min(printf_s("\n"), printf_s(buf, Forward<TArgs>(args)...));
    }

    template <Int32U FormatBufferLen, typename... TArgs>
    static int WriteLine(const char(&format)[FormatBufferLen], TArgs&&... args) {
        TLockGuard guard(ms_ConsoleLock);
        return Math::Min(printf_s("\n"), printf_s(format, Forward<TArgs>(args)...));
    }

    static void Clear();
    static void SetCursorPosition(int x, int y);
    static Tuple<int, int> GetCursorPosition();

    static bool SetOutputCodePage(int codePage);

	// 출력 스트림에 문자를 입력해서 콘솔을 제어할 수 있도록 버철 터미널 옵션을 켜주자.
    static bool SetEnableVTMode(bool enabled);
    static int GetOutputCodePage();
};

NS_JC_END



