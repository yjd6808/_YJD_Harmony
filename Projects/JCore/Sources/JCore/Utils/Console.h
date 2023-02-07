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


NS_JC_BEGIN
    NS_DETAIL_BEGIN
    template <typename TLock>
    class Console
    {
        using TLockGuard = LockGuard<TLock>;

        inline static WinHandle     ms_hStdout = INVALID_HANDLE_VALUE;
        inline static ConsoleColor	ms_iDefaultColor = ConsoleColor::LightGray;
        inline static TLock         ms_ConsoleLock;
        inline static int           ms_iCursorPosX;
        inline static int           ms_iCursorPosY;
        inline static constexpr int TempBufferLen = 1024;
    public:
        static void Init() {
            TLockGuard guard(ms_ConsoleLock);
            ms_hStdout = WinApi::GetStdoutHandle();
        }

        static void SetConsoleSize(int width, int height) {
            char cmd[TempBufferLen];
            sprintf_s(cmd, " mode  con lines=%d   cols=%d ", height, width);
            CRuntime::System(cmd);
        }

        static void SetColor(ConsoleColor color) {
            TLockGuard guard(ms_ConsoleLock);
            ms_iDefaultColor = color;
            SetConsoleTextAttribute(ms_hStdout, static_cast<Int16>(color));
        }

        static ConsoleColor GetColor() {
            TLockGuard guard(ms_ConsoleLock);
            return ms_iDefaultColor;
        }

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

        static void Clear() {
            TLockGuard guard(ms_ConsoleLock);
            CRuntime::System("cls");
        }

        static void SetCursorPosition(int x, int y) {
            TLockGuard guard(ms_ConsoleLock);
            DebugAssert(ms_hStdout != INVALID_HANDLE_VALUE);
            ms_iCursorPosX = x;
            ms_iCursorPosY = y;
            WinApi::SetConsoleCursorPosition(ms_hStdout, x, y);
        }

        static Tuple<int, int> GetCursorPosition() {
            DebugAssert(ms_hStdout != INVALID_HANDLE_VALUE);
            int x;
            int y;
            if (WinApi::GetConsoleCursorPosition(ms_hStdout, x, y)) {
                return { x,  y };
            }
            return { -1, -1 };
        }

        static bool SetOutputCodePage(int codePage) {
            bool bRet = WinApi::SetConsoleOutputCodePage(codePage);
            // DebugAssertMessage(bRet, "인코딩 변경 실패");
            return bRet;
        }

        static int GetOutputCodePage() {
            return WinApi::GetConsoleOutputCodePage();
        }
    };
	NS_DETAIL_END
    

using SafeConsole   =  Detail::Console<RecursiveLock>;
using NormalConsole =  Detail::Console<UnusedLock>;
extern template        Detail::Console<RecursiveLock>;
extern template        Detail::Console<UnusedLock>;
NS_JC_END



