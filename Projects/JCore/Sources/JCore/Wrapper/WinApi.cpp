/*
 * 작성자: 윤정도
 * 생성일: 12/6/2022 1:57:55 PM
 * =====================
 *
 */


#include <JCore/Core.h>
#include <JCore/Limit.h>
#include <JCore/Wrapper/WinApi.h>

namespace JCore {
    bool
    JCoreStdCall
    WinApi::SetConsoleCursorPosition(In_ WinHandle hStdoutHandle, In_ int x, In_ int y) {
        COORD p;
        p.X = static_cast<Int16>(x);
        p.Y = static_cast<Int16>(y);
        return ::SetConsoleCursorPosition(hStdoutHandle, p) != 0;
    }

    bool
    JCoreStdCall
    WinApi::GetConsoleCursorPosition(In_ WinHandle hStdoutHandle, Out_ int& x, Out_ int& y) {
        CONSOLE_SCREEN_BUFFER_INFO cbsi;

        if (::GetConsoleScreenBufferInfo(hStdoutHandle, &cbsi)) {
            x = cbsi.dwCursorPosition.X;
            y = cbsi.dwCursorPosition.Y;
            return true;
        }

        return false;
    }

    bool
    JCoreStdCall
    WinApi::SetConsoleTextAttribute(In_ WinHandle hStdoutHandle, In_ Int16 attribute) {
        return ::SetConsoleTextAttribute(hStdoutHandle, attribute) != 0;
    }


    bool
    JCoreStdCall
    WinApi::SetConsoleOutputCodePage(In_ Int codePage) {
        return ::SetConsoleOutputCP(codePage) != 0;
    }


    Int
    JCoreStdCall
    WinApi::GetConsoleOutputCodePage() {
        return static_cast<int>(::GetConsoleOutputCP());
    }

    WinHandle
    JCoreStdCall
    WinApi::GetStdoutHandle() {
        return ::GetStdHandle(STD_OUTPUT_HANDLE);
    }

    WinHandle
    JCoreStdCall
    WinApi::CreateEvent(bool initialState, bool manualReset, const char* name) {
        return ::CreateEvent(NULL, manualReset ? TRUE : FALSE, initialState ? TRUE : FALSE, name);
    }

    Int32U
    JCoreStdCall
    WinApi::WaitForMultipleObjectsEx(In_ Int32U eventCount, In_ WinHandle* handles, In_ bool waitAll, In_ Int32U timeout, In_ bool alertable) {
        return ::WaitForMultipleObjectsEx(eventCount, handles, waitAll ? TRUE : FALSE, timeout, alertable ? TRUE: FALSE);
    }

    Int32U
    JCoreStdCall
    WinApi::GetLastError() {
        return ::GetLastError();
    }

    bool
    JCoreStdCall
    WinApi::SetEvent(In_ WinHandle handle) {
        return ::SetEvent(handle) != 0;
    }

    bool
    JCoreStdCall
    WinApi::ResetEvent(In_ WinHandle handle) {
        return ::ResetEvent(handle) != 0;   
    }

    bool
    JCoreStdCall
    WinApi::CloseHandle(In_ WinHandle handle) {
        return ::CloseHandle(handle) != 0;
    }

    int
    JCoreStdCall
    WinApi::GetThreadPriority(In_ WinHandle threadHandle) {
        return ::GetThreadPriority(threadHandle);
    }

    bool
    JCoreStdCall
    WinApi::SetThreadPriority(In_ WinHandle threadHandle, In_ int priority) {
        return ::SetThreadPriority(threadHandle, priority) != 0;
    }


    Int32U
    JCoreStdCall
    WinApi::GetCurrentThreadId() {
        return ::GetCurrentThreadId();
    }

    

    // =========================================================================================
    // Interlocked Api 
    // =========================================================================================


    template <typename TOperand>
    TOperand
    Interlocked<TOperand>::Add(InOut_ TOperand* destination, In_ TOperand value) {
        return ExchangeAdd(destination, value) + value;
    }

    template <typename TOperand>
    TOperand
    Interlocked<TOperand>::CompareExchange(InOut_ TOperand* destination, In_ TOperand expected, TOperand desired) {
        if constexpr (sizeof(TOperand) == sizeof(Boundary8)) {
            return static_cast<TOperand>(::_InterlockedCompareExchange8(reinterpret_cast<volatile Boundary8*>(destination), desired, expected));
        }

        if constexpr (sizeof(TOperand) == sizeof(Boundary16)) {
            return static_cast<TOperand>(::_InterlockedCompareExchange16(reinterpret_cast<volatile Boundary16*>(destination), desired, expected));
        }

        if constexpr (sizeof(TOperand) == sizeof(Boundary32)) {
            return static_cast<TOperand>(::_InterlockedCompareExchange(reinterpret_cast<volatile Boundary32*>(destination), desired, expected));
        }

        return static_cast<TOperand>(::_InterlockedCompareExchange64(reinterpret_cast<volatile Boundary64*>(destination), desired, expected));
    }

    template <typename TOperand>
    TOperand
    Interlocked<TOperand>::Exchange(InOut_ TOperand* destination, In_ TOperand value) {
        if constexpr (sizeof(TOperand) == sizeof(Boundary8)) {
            return static_cast<TOperand>(::_InterlockedExchange8(reinterpret_cast<volatile Boundary8*>(destination), value));
        }

        if constexpr (sizeof(TOperand) == sizeof(Boundary16)) {
            return static_cast<TOperand>(::_InterlockedExchange16(reinterpret_cast<volatile Boundary16*>(destination), value));
        }

        if constexpr (sizeof(TOperand) == sizeof(Boundary32)) {
            return static_cast<TOperand>(::_InterlockedExchange(reinterpret_cast<volatile Boundary32*>(destination), value));
        }

        return static_cast<TOperand>(::_InlineInterlockedExchange64(reinterpret_cast<volatile Boundary64*>(destination), value));
    }

    template <typename TOperand>
    TOperand
    Interlocked<TOperand>::ExchangeAdd(InOut_ TOperand* destination, In_ TOperand value) {
        if constexpr (sizeof(TOperand) == sizeof(Boundary8)) {
            return static_cast<TOperand>(_InterlockedExchangeAdd8(reinterpret_cast<volatile Boundary8*>(destination), value));
        }

        if constexpr (sizeof(TOperand) == sizeof(Boundary16)) {
            return static_cast<TOperand>(_InterlockedExchangeAdd16(reinterpret_cast<volatile Boundary16*>(destination), value));
        }

        if constexpr (sizeof(TOperand) == sizeof(Boundary32)) {
            return static_cast<TOperand>(::_InterlockedExchangeAdd(reinterpret_cast<volatile Boundary32*>(destination), value));
        }

        return static_cast<TOperand>(::_InlineInterlockedExchangeAdd64(reinterpret_cast<volatile Boundary64*>(destination), value));
    }

    template <typename TOperand>
    TOperand
    Interlocked<TOperand>::Increment(InOut_ TOperand* destination) {
        if constexpr (sizeof(TOperand) == sizeof(Boundary8)) {
            return Add(destination, 1);
        }

        if constexpr (sizeof(TOperand) == sizeof(Boundary16)) {
            return static_cast<TOperand>(::_InterlockedIncrement16(reinterpret_cast<volatile Boundary16*>(destination)));
        }

        if constexpr (sizeof(TOperand) == sizeof(Boundary32)) {
            return static_cast<TOperand>(::_InterlockedIncrement(reinterpret_cast<volatile Boundary32*>(destination)));
        }

        return static_cast<TOperand>(::_InlineInterlockedIncrement64(reinterpret_cast<volatile Boundary64*>(destination)));
    }

    template <typename TOperand>
    TOperand
    Interlocked<TOperand>::Decrement(InOut_ TOperand* destination) {
        if constexpr (sizeof(TOperand) == sizeof(Boundary8)) {
            return Add(destination, -1);
        }

        if constexpr (sizeof(TOperand) == sizeof(Boundary16)) {
            return static_cast<TOperand>(::_InterlockedDecrement16(reinterpret_cast<volatile Boundary16*>(destination)));
        }

        if constexpr (sizeof(TOperand) == sizeof(Boundary32)) {
            return static_cast<TOperand>(::_InterlockedDecrement(reinterpret_cast<volatile Boundary32*>(destination)));
        }

        return static_cast<TOperand>(::_InlineInterlockedIncrement64(reinterpret_cast<volatile Boundary64*>(destination)));
    }

    template <typename TOperand>
    TOperand
    Interlocked<TOperand>::And(InOut_ TOperand* destination, In_ TOperand value) {
        if constexpr (sizeof(TOperand) == sizeof(Boundary8)) {
            return static_cast<TOperand>(::_InterlockedAnd8(reinterpret_cast<volatile Boundary8*>(destination), value));
        }

        if constexpr (sizeof(TOperand) == sizeof(Boundary16)) {
            return static_cast<TOperand>(::_InterlockedAnd16(reinterpret_cast<volatile Boundary16*>(destination), value));
        }

        if constexpr (sizeof(TOperand) == sizeof(Boundary32)) {
            return static_cast<TOperand>(::_InterlockedAnd(reinterpret_cast<volatile Boundary32*>(destination), value));
        }

        return static_cast<TOperand>(::_InlineInterlockedAnd64(reinterpret_cast<volatile Boundary64*>(destination), value));
    }

    template <typename TOperand>
    TOperand
    Interlocked<TOperand>::Or(InOut_ TOperand* destination, In_ TOperand value) {
        if constexpr (sizeof(TOperand) == sizeof(Boundary8)) {
            return static_cast<TOperand>(::_InterlockedOr8(reinterpret_cast<volatile Boundary8*>(destination), value));
        }

        if constexpr (sizeof(TOperand) == sizeof(Boundary16)) {
            return static_cast<TOperand>(::_InterlockedOr16(reinterpret_cast<volatile Boundary16*>(destination), value));
        }

        if constexpr (sizeof(TOperand) == sizeof(Boundary32)) {
            return static_cast<TOperand>(::_InterlockedOr(reinterpret_cast<volatile Boundary32*>(destination), value));
        }

        return static_cast<TOperand>(::_InlineInterlockedOr64(reinterpret_cast<volatile Boundary64*>(destination), value));
    }

    template <typename TOperand>
    TOperand
    Interlocked<TOperand>::Xor(InOut_ TOperand* destination, In_ TOperand value) {
        if constexpr (sizeof(TOperand) == sizeof(Boundary8)) {
            return static_cast<TOperand>(::_InterlockedXor8(reinterpret_cast<volatile Boundary8*>(destination), value));
        }

        if constexpr (sizeof(TOperand) == sizeof(Boundary16)) {
            return static_cast<TOperand>(::_InterlockedXor16(reinterpret_cast<volatile Boundary16*>(destination), value));
        }

        if constexpr (sizeof(TOperand) == sizeof(Boundary32)) {
            return static_cast<TOperand>(::_InterlockedXor(reinterpret_cast<volatile Boundary32*>(destination), value));
        }

        return static_cast<TOperand>(::_InlineInterlockedXor64(reinterpret_cast<volatile Boundary64*>(destination), value));
    }


    bool
    Interlocked<bool>::CompareExchange(InOut_ bool* destination, In_ bool expected, In_ bool desired) {
            Boundary8 iExpected = expected ? 1 : 0;
            Boundary8 iDesired = desired ? 1 : 0;

        return TInterlocked::CompareExchange(reinterpret_cast<Boundary8*>(destination), iExpected, iDesired) ? true : false;
    }

    bool
    Interlocked<bool>::Exchange(bool* destination, bool value) {
        Boundary8 iValue = value ? 1 : 0;
        return TInterlocked::Exchange(reinterpret_cast<Boundary8*>(destination), iValue) ? true : false;
    }

    template struct Interlocked<Int8>;
    template struct Interlocked<Int8U>;
    template struct Interlocked<Int16>;
    template struct Interlocked<Int16U>;
    template struct Interlocked<Int32>;
    template struct Interlocked<Int32U>;
    template struct Interlocked<Int32L>;
    template struct Interlocked<Int32UL>;
    template struct Interlocked<Int64>;
    template struct Interlocked<Int64U>;
    template struct Interlocked<bool>;
} // namespace JCore

