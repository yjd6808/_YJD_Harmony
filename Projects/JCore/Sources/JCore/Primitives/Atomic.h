/*
 * 작성자: 윤정도
 * 생성일: 12/6/2022 3:54:27 PM
 * =====================
 */


#pragma once

#include <JCore/TypeTraits.h>
#include <JCore/Wrapper/WinApi.h>

namespace JCore {

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
        T Load() {
            T expected = 0;
            if (CompareExchange(expected, 0)) {
                return 0;
            }

            return expected;
        }

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
        Atomic() : m_pValue(nullptr) {}
        template <typename U, EnableIf_t<IsConvertible_v<U, T*>, int> = 0>
        Atomic(U ptr) : m_pValue(ptr) {}

        template <typename U, EnableIf_t<IsConvertible_v<U, T*>, int> = 0>
        void Store(U operand) { Exchange(operand); }

        T* Load() {
            T* expected = nullptr;
            if (CompareExchange(expected, nullptr)) {
                return nullptr;
            }

            return expected;
        }

        T* Add(int operand) { return TInterlocked::Add(&m_pValue, operand); }

        template <typename U, EnableIf_t<IsConvertible_v<U, T*>, int> = 0>
        bool TryCompareExchange(U expected, U desired) { return CompareExchange(expected, desired); }
        template <typename U, EnableIf_t<IsConvertible_v<U, T*>, int> = 0>
        bool CompareExchange(U& expected, U desired) {
            T* before = expected;
            T* initial = TInterlocked::CompareExchange(&m_pValue, expected, desired);

            if (before == initial)
                return true;

            expected = initial;
            return false;
        }

        T* ExchangeAdd(int operand) { return TInterlocked::ExchangeAdd(&m_pValue, operand); }
        template <typename U, EnableIf_t<IsConvertible_v<U, T*>, int> = 0>
        T* Exchange(U operand) { return TInterlocked::Exchange(&m_pValue, operand); }


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

        T* operator+=(int other) { return ExchangeAdd(other) + other; }
        T* operator-=(int other) { return ExchangeAdd(other * -1) - other; }

        template <typename U, EnableIf_t<IsConvertible_v<U, T*>, int> = 0>
        T* operator==(U other) { return Load() == other; }
        template <typename U, EnableIf_t<IsConvertible_v<U, T*>, int> = 0>
        T* operator!=(U other) { return Load() != other; }
    private:
        T* m_pValue;
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
        bool Load() {
            bool expected = false;
            if (CompareExchange(expected, false)) {
                return false;
            }

            return expected;
        }

        bool Exchange(bool operand) { return TInterlocked::Exchange(&m_Value, operand); }

        bool TryCompareExchange(bool expected, bool desired) { return CompareExchange(expected, desired); }
        bool CompareExchange(bool& expected, bool desired) {
            bool before = expected;
            bool initial = TInterlocked::CompareExchange(&m_Value, expected, desired);

            // 바꾸고자 하는 값이 초기값이랑 일치했다는건
            // 교환이 되기전 값과 일치한다는 것은
            // 교환이 성공했다는 것이다.
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


} // namespace JCore

