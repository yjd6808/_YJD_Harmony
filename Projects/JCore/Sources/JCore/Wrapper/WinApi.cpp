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
    JCoreWinApi
    SetConsoleCursorPosition(In_ WinHandle hStdoutHandle, In_ int x, In_ int y) {
        COORD p;
        p.X = static_cast<Int16>(x);
        p.Y = static_cast<Int16>(y);
        return ::SetConsoleCursorPosition(hStdoutHandle, p) != 0;
    }

    bool
    JCoreWinApi
    GetConsoleCursorPosition(In_ WinHandle hStdoutHandle, Out_ int& x, Out_ int& y) {
        CONSOLE_SCREEN_BUFFER_INFO cbsi;

        if (::GetConsoleScreenBufferInfo(hStdoutHandle, &cbsi)) {
            x = cbsi.dwCursorPosition.X;
            y = cbsi.dwCursorPosition.Y;
            return true;
        }

        return false;
    }

    bool
    JCoreWinApi
    SetConsoleTextAttribute(In_ WinHandle hStdoutHandle, In_ Int16 attribute) {
        return ::SetConsoleTextAttribute(hStdoutHandle, attribute) != 0;
    }


    bool
    JCoreWinApi
    SetConsoleOutputCodePage(In_ Int codePage) {
        return ::SetConsoleOutputCP(codePage) != 0;
    }


    Int
    JCoreWinApi
    GetConsoleOutputCodePage() {
        return static_cast<int>(::GetConsoleOutputCP());
    }

    WinHandle
    JCoreWinApi
    GetStdoutHandle() {
        return ::GetStdHandle(STD_OUTPUT_HANDLE);
    }


    


    // =========================================================================================
    // Interlocked Api 
    // =========================================================================================


    template <typename TOperand>
    JCoreForceInline
    TOperand
    Interlocked<TOperand>::Add(InOut_ TOperand* destination, In_ TOperand value) {
        return ExchangeAdd(destination, value) + value;
    }

    template <typename TOperand>
    JCoreForceInline
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
    JCoreForceInline
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
    JCoreForceInline
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
    JCoreForceInline
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
    JCoreForceInline
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
    JCoreForceInline
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
    JCoreForceInline
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
    JCoreForceInline
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
} // namespace JCore

