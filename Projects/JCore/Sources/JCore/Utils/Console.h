/*
 * 작성자 : 윤정도
 */

#pragma once

#include <JCore/Sync/RecursiveLock.h>
#include <JCore/Sync/UnusedLock.h>
#include <JCore/Utils/ConsoleColor.h>

#include <JCore/Wrapper/StdLib.h>
#include <JCore/Wrapper/WinApi.h>
#include <JCore/Tuple.h>


namespace JCore {
    namespace Detail {
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
                ms_hStdout = GetStdoutHandle();
            }

            static void SetConsoleSize(int width, int height) {
                char cmd[TempBufferLen];
                sprintf_s(cmd, " mode  con lines=%d   cols=%d ", height, width);
                System(cmd);
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
            static void Write(ConsoleColor color, char* format, TArgs&&... args) {
                TLockGuard guard(ms_ConsoleLock);
                ConsoleColor prevColor = ms_iDefaultColor;
                SetColor(color);
                Write(format, Forward<TArgs>(args)...);
                SetColor(prevColor);
            }

            template <Int32U FormatBufferLen, typename... TArgs>
            static void Write(ConsoleColor color, const char(&format)[FormatBufferLen], TArgs&&... args) {
                TLockGuard guard(ms_ConsoleLock);
                ConsoleColor prevColor = ms_iDefaultColor;
                SetColor(color);
                Write(format, Forward<TArgs>(args)...);
                SetColor(prevColor);
            }

            template <typename... TArgs>
            static void Write(char* format, TArgs&&... args) {
                char buf[TempBufferLen];
                sprintf_s(buf, TempBufferLen, format);
                TLockGuard guard(ms_ConsoleLock);
                printf_s(buf, Forward<TArgs>(args)...);
            }

            template <Int32U FormatBufferLen, typename... TArgs>
            static void Write(const char(&format)[FormatBufferLen], TArgs&&... args) {
                TLockGuard guard(ms_ConsoleLock);
                printf_s(format, Forward<TArgs>(args)...);
            }

            template <typename... TArgs>
            static void WriteLine(ConsoleColor color, char* format, TArgs&&... args) {
                TLockGuard guard(ms_ConsoleLock);
                ConsoleColor prevColor = ms_iDefaultColor;
                SetColor(color);
                WriteLine(format, Forward<TArgs>(args)...);
                SetColor(prevColor);
            }

            template <Int32U FormatBufferLen, typename... TArgs>
            static void WriteLine(ConsoleColor color, const char(&format)[FormatBufferLen], TArgs&&... args) {
                TLockGuard guard(ms_ConsoleLock);
                ConsoleColor prevColor = ms_iDefaultColor;
                SetColor(color);
                WriteLine(format, Forward<TArgs>(args)...);
                SetColor(prevColor);
            }

            template <typename... TArgs>
            static void WriteLine(char* format, TArgs&&... args) {
                char buf[TempBufferLen + 1];
                sprintf_s(buf, TempBufferLen + 1, format);
                TLockGuard guard(ms_ConsoleLock);
                printf_s(buf, Forward<TArgs>(args)...);
                printf_s("\n");
            }

            template <Int32U FormatBufferLen, typename... TArgs>
            static void WriteLine(const char(&format)[FormatBufferLen], TArgs&&... args) {
                TLockGuard guard(ms_ConsoleLock);
                printf_s(format, Forward<TArgs>(args)...);
                printf_s("\n");
            }

            static void Clear() {
                TLockGuard guard(ms_ConsoleLock);
                System("cls");
            }

            static void SetCursorPosition(int x, int y) {
                TLockGuard guard(ms_ConsoleLock);
                DebugAssert(ms_hStdout != INVALID_HANDLE_VALUE);
                ms_iCursorPosX = x;
                ms_iCursorPosY = y;
                SetConsoleCursorPosition(ms_hStdout, x, y);
            }

            static Tuple<int, int> GetCursorPosition() {
                DebugAssert(ms_hStdout != INVALID_HANDLE_VALUE);
                int x;
                int y;
                if (GetConsoleCursorPosition(ms_hStdout, x, y)) {
                    return { x,  y };
                }
                return { -1, -1 };
            }

            static void SetOutputCodePage(int codePage) {
                SetConsoleOutputCodePage(codePage);
            }

            static int GetOutputCodePage() {
                return GetConsoleOutputCodePage();
            }
        };
    } // namespace Detail
    

    using SafeConsole   =  Detail::Console<RecursiveLock>;
    using NormalConsole =  Detail::Console<UnusedLock>;
    extern template        Detail::Console<RecursiveLock>;
    extern template        Detail::Console<UnusedLock>;
} // namespace JCore



