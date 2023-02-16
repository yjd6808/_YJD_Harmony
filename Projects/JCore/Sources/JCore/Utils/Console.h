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

#define ESC "\x1b"
#define CSI "\x1b["

// 우선색상만..
#define CSI_GRAPHIC_RENDITION(x) CSI#x"m"

NS_JC_BEGIN

class Console
{
	using TLockGuard = RecursiveLockGuard;
    using TLock = RecursiveLock;

    static WinHandle     ms_hStdout;
    static ConsoleColor	 ms_iDefaultColor;
    static TLock         ms_ConsoleLock;
    static bool          ms_UseConsoleLock;
    static int           ms_iCursorPosX;
    static int           ms_iCursorPosY;
    static int           TempBufferLen;
    static const char*   VTForegroundColor[ConsoleColor::Max];
public:
    static bool Init();
    static bool SetSize(int width, int height);

    static const char* GetVTForegroundColor(ConsoleColor foreground);
    static const char* GetVTReset();

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



