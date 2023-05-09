/*
 * 작성자: 윤정도
 * 생성일: 12/6/2022 3:54:27 PM
 * =====================
 * 만들고보니까 enum이 있었네;;
 * 하......
 * 여기서 enum 추가할려면 TypeTraits 를 써야해서
 * 코드가 아름다워지지가 않는다.
 * 깔끔하게 구현할만한 방법이 떠오르면 그때 하는걸로
 */


#pragma once

#include <JCore/TypeTraits.h>
#include <JCore/Wrapper/WinApi.h>

NS_JC_BEGIN

template <typename T>
class Atomic
{
    using TInterlocked = Interlocked<T>;
    using TAtomic = Atomic<T>;
public:
    Atomic() : m_Value(T()) {}
    Atomic(T value) : m_Value(value) {}
    template <typename U>
    Atomic(Atomic<U>& other) : m_Value(other.Load()) {}


    void Store(T operand) { Exchange(operand); }
    T Load() { return TInterlocked::Read(&m_Value); }

    T Add(T operand) { return TInterlocked::Add(&m_Value, operand); }

    bool TryCompareExchange(T expected, T desired) { return CompareExchange(expected, desired); }
    bool CompareExchange(T& expected, T desired) {
        T before = expected;
        T initial = TInterlocked::CompareExchange(&m_Value, expected, desired);

        // 바꾸고자 하는 값이 초기값이랑 일치했다는건
        // 교환이 되기전 값과 일치한다는 것은
        // 교환이 성공했다는 것이다.
        if (before == initial)
            return true;

        expected = initial;
        return false;
    }

    T ExchangeAdd(T operand) { return TInterlocked::ExchangeAdd(&m_Value, operand); }
    T Exchange(T operand) { return TInterlocked::Exchange(&m_Value, operand); }
    T Increment() { return TInterlocked::Increment(&m_Value); }
    T Decrement() { return TInterlocked::Decrement(&m_Value); }
    T Xor(T operand) { return TInterlocked::Xor(&m_Value, operand); }
    T Or(T operand) { return TInterlocked::Or(&m_Value, operand); }
    T And(T operand) { return TInterlocked::And(&m_Value, operand); }


    // =====================================================================================
    // 연산자 오버로딩
    // =====================================================================================
    operator T() { return Load(); }

    T operator++() { return Increment(); }
    T operator++(int) {
        T iResult = Increment();
        return --iResult;
    }
    T operator--() { return Decrement(); }
    T operator--(int) {
        T iResult = Decrement();
        return ++iResult;
    }

    T operator+=(T other) { return ExchangeAdd(other) + other; }
    T operator-=(T other) { return ExchangeAdd(other * -1) - other; }
    T operator|(T other) { return Load() | other; }
    T operator|=(T other) { return Or(other) | other; }
    T operator&(T other) { return Load() & other; }
    T operator&=(T other) { return And(other) & other; }
    T operator^(T other) { return Load() ^ other; }
    T operator^=(T other) { return Xor(other) ^ other; }
    T operator/(T other) { return Load() / other; }
    T operator*(T other) { return Load() * other; }
    T operator%(T other) { return Load() % other; }
    T operator+(T other) { return Load() + other; }
    T operator-(T other) { return Load() - other; }
    T operator==(T other) { return Load() == other; }
    T operator!=(T other) { return Load() != other; }
    T operator>(T other) { return Load() > other; }
    T operator<(T other) { return Load() < other; }
    T operator>=(T other) { return Load() >= other; }
    T operator<=(T other) { return Load() <= other; }
private:
    T m_Value;

    template<typename> friend class Atomic;
};

template <typename T>
class Atomic<T*>
{
    using TAtomic = Atomic<T*>;
    using TInterlocked = Interlocked<T*>;
public:
    Atomic() : m_Value(nullptr) {}
    template <typename U, DefaultEnableIf_t<IsConvertible_v<U, T*>> = nullptr>
    Atomic(U ptr) : m_Value(ptr) {}

    template <typename U, DefaultEnableIf_t<IsConvertible_v<U, T*>> = nullptr>
    void Store(U operand) { Exchange(operand); }

    T* Load() { return TInterlocked::Read(&m_Value); }
    T* Add(int operand) {  return TInterlocked::Add(&m_Value, operand); }

    template <typename U, DefaultEnableIf_t<IsConvertible_v<U, T*>> = nullptr>
    bool TryCompareExchange(U expected, U desired) { return CompareExchange(expected, desired); }
    template <typename U, DefaultEnableIf_t<IsConvertible_v<U, T*>> = nullptr>
    bool CompareExchange(U& expected, U desired) {
        T* before = expected;
        T* initial = TInterlocked::CompareExchange(&m_Value, expected, desired);

        if (before == initial)
            return true;

        expected = initial;
        return false;
    }

    T* ExchangeAdd(int operand) { return TInterlocked::ExchangeAdd(&m_Value, operand); }
    template <typename U, DefaultEnableIf_t<IsConvertible_v<U, T*>> = nullptr>
    T* Exchange(U operand) { return TInterlocked::Exchange(&m_Value, operand); }


    // =====================================================================================
    // 연산자 오버로딩
    // =====================================================================================
    operator T* () { return Load(); }

    T* operator++() { return Add(1); }
    T* operator++(int) {
        T iResult = Add(1);
        return --iResult;
    }
    T* operator--() { return Add(-1); }
    T* operator--(int) {
        T* iResult = Add(-1);
        return ++iResult;
    }

    T* operator+=(int other) { return ExchangeAdd(other) + (sizeof(T) * other); }
    T* operator-=(int other) { return ExchangeAdd(other * -1) - (sizeof(T) * other); }

    template <typename U, DefaultEnableIf_t<IsConvertible_v<U, T*>> = nullptr>
    bool operator==(U other) { return Load() == other; }
    template <typename U, DefaultEnableIf_t<IsConvertible_v<U, T*>> = nullptr>
    bool operator!=(U other) { return Load() != other; }

    T& operator[](const int idx) { return Load()[idx]; }
private:
    T* m_Value;
};

template <>
class Atomic<bool>
{
    using TAtomic = Atomic<bool>;
    using TInterlocked = Interlocked<bool>;
public:
    Atomic() : m_Value(false) {}
    Atomic(bool value) : m_Value(value) {}
    Atomic(TAtomic& other) : m_Value(other.Load()) {}

    void Store(bool operand) { Exchange(operand); }
    bool Load() { return TInterlocked::Read(&m_Value); }

    bool Exchange(bool operand) { return TInterlocked::Exchange(&m_Value, operand); }

    bool TryCompareExchange(bool expected, bool desired) { return CompareExchange(expected, desired); }
    bool CompareExchange(bool& expected, bool desired) {
        bool before = expected;
        bool initial = TInterlocked::CompareExchange(&m_Value, expected, desired);

        if (before == initial)
            return true;

        expected = initial;
        return false;
    }

    // =====================================================================================
    // 연산자 오버로딩
    // =====================================================================================
    operator bool() { return Load(); }


    bool operator==(bool other) { return Load() == other; }
    bool operator!=(bool other) { return Load() != other; }
private:
    bool m_Value;
};

template <>
class Atomic<void*>
{
    using TAtomic = Atomic<void*>;
    using TInterlocked = Interlocked<void*>;
public:
    Atomic() : m_Value(nullptr) {}
    Atomic(void* value) : m_Value(value) {}
    Atomic(TAtomic& other) : m_Value(other.Load()) {}

    void Store(void* operand) { Exchange(operand); }
    void* Load() {
        void* expected = nullptr;
        if (CompareExchange(expected, nullptr)) {
            return nullptr;
        }

        return expected;
    }

    void* Exchange(void* operand) { return TInterlocked::Exchange(&m_Value, operand); }

    bool TryCompareExchange(void* expected, void* desired) { return CompareExchange(expected, desired); }
    bool CompareExchange(void*& expected, void* desired) {
        void* before = expected;
        void* initial = TInterlocked::CompareExchange(&m_Value, expected, desired);

        if (before == initial)
            return true;

        expected = initial;
        return false;
    }

    // =====================================================================================
    // 연산자 오버로딩
    // =====================================================================================
    operator bool() { return Load(); }


    bool operator==(void* other) { return Load() == other; }
    bool operator!=(void* other) { return Load() != other; }
private:
    void* m_Value;
};

using AtomicInt64    = Atomic<Int64>;
using AtomicInt64U   = Atomic<Int64U>;
using AtomicInt      = Atomic<Int>;
using AtomicInt32    = Atomic<Int32>;
using AtomicInt32U   = Atomic<Int32U>;
using AtomicInt32L   = Atomic<Int32L>;
using AtomicInt32UL  = Atomic<Int32UL>;
using AtomicInt16    = Atomic<Int16>;
using AtomicInt16U   = Atomic<Int16U>;
using AtomicInt8     = Atomic<Int8>;
using AtomicInt8U    = Atomic<Int8U>;
using AtomicChar     = Atomic<Char>;
using AtomicBool     = Atomic<bool>;
using AtomicByte     = Atomic<Byte>;
using AtomicWideChar = Atomic<WideChar>;
using AtomicIntPtr   = Atomic<IntPtr>;


NS_JC_END

