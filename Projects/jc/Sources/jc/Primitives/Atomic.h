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

#include <jc/TypeTraits.h>
#include <jc/Wrapper/WinApi.h>

NS_JC_BEGIN

template <typename T>
class Atomic
{
    using TInterlocked = Interlocked<T>;
    using TAtomic = Atomic<T>;
public:
    Atomic() : value_(T()) {}
    Atomic(T _value) : value_(_value) {}
    template <typename U>
    Atomic(Atomic<U>& _other) : value_(_other.Load()) {}


    void Store(T _operand) { Exchange(_operand); }
    T Load() const { return TInterlocked::Read(const_cast<T*>(&value_)); }

    T Add(T _operand) { return TInterlocked::Add(&value_, _operand); }

    bool TryCompareExchange(T _expected, T _desired) { return CompareExchange(_expected, _desired); }
    bool CompareExchange(T& _expected, T _desired) {
        T before = _expected;
        T initial = TInterlocked::CompareExchange(&value_, _expected, _desired);

        // 바꾸고자 하는 값이 초기값이랑 일치했다는건
        // 교환이 되기전 값과 일치한다는 것은
        // 교환이 성공했다는 것이다.
        if (before == initial)
            return true;

        _expected = initial;
        return false;
    }

    T ExchangeAdd(T _operand) { return TInterlocked::ExchangeAdd(&value_, _operand); }
    T Exchange(T _operand) { return TInterlocked::Exchange(&value_, _operand); }
    T Increment() { return TInterlocked::Increment(&value_); }
    T Decrement() { return TInterlocked::Decrement(&value_); }
    T Xor(T _operand) { return TInterlocked::Xor(&value_, _operand); }
    T Or(T _operand) { return TInterlocked::Or(&value_, _operand); }
    T And(T _operand) { return TInterlocked::And(&value_, _operand); }


    // =====================================================================================
    // 연산자 오버로딩
    // =====================================================================================
    operator T() const { return Load(); }

    T operator++() { return Increment(); }
    T operator++(int) {
        T result = Increment();
        return --result;
    }
    T operator--() { return Decrement(); }
    T operator--(int) {
        T result = Decrement();
        return ++result;
    }

    TAtomic& operator=(T _other) { Exchange(_other); return *this; }
    T operator+=(T _other) { return ExchangeAdd(_other) + _other; }
    T operator-=(T _other) { return ExchangeAdd(_other * -1) - _other; }
    T operator|(T _other) { return Load() | _other; }
    T operator|=(T _other) { return Or(_other) | _other; }
    T operator&(T _other) { return Load() & _other; }
    T operator&=(T _other) { return And(_other) & _other; }
    T operator^(T _other) { return Load() ^ _other; }
    T operator^=(T _other) { return Xor(_other) ^ _other; }
    T operator/(T _other) { return Load() / _other; }
    T operator*(T _other) { return Load() * _other; }
    T operator%(T _other) { return Load() % _other; }
    T operator+(T _other) { return Load() + _other; }
    T operator-(T _other) { return Load() - _other; }
    T operator==(T _other) { return Load() == _other; }
    T operator!=(T _other) { return Load() != _other; }
    T operator>(T _other) { return Load() > _other; }
    T operator<(T _other) { return Load() < _other; }
    T operator>=(T _other) { return Load() >= _other; }
    T operator<=(T _other) { return Load() <= _other; }
private:
    T value_;

    template<typename> friend class Atomic;
};

template <typename T>
class Atomic<T*>
{
    using TAtomic = Atomic<T*>;
    using TInterlocked = Interlocked<T*>;
public:
    Atomic() : value_(nullptr) {}
    template <typename U, DefaultEnableIf_t<IsConvertible_v<U, T*>> = nullptr>
    Atomic(U _pPtr) : value_(_pPtr) {}

    template <typename U, DefaultEnableIf_t<IsConvertible_v<U, T*>> = nullptr>
    void Store(U _pOperand) { Exchange(_pOperand); }

    T* Load() const { return TInterlocked::Read(const_cast<T**>(&value_)); }
    T* Add(int _operand) {  return TInterlocked::Add(&value_, _operand); }

    template <typename U, DefaultEnableIf_t<IsConvertible_v<U, T*>> = nullptr>
    bool TryCompareExchange(U _pExpected, U _pDesired) { return CompareExchange(_pExpected, _pDesired); }
    template <typename U, DefaultEnableIf_t<IsConvertible_v<U, T*>> = nullptr>
    bool CompareExchange(U& _pExpected, U _pDesired) {
        T* pBefore = _pExpected;
        T* pInitial = TInterlocked::CompareExchange(&value_, _pExpected, _pDesired);

        if (pBefore == pInitial)
            return true;

        _pExpected = pInitial;
        return false;
    }

    T* ExchangeAdd(int _operand) { return TInterlocked::ExchangeAdd(&value_, _operand); }
    template <typename U, DefaultEnableIf_t<IsConvertible_v<U, T*>> = nullptr>
    T* Exchange(U _pOperand) { return TInterlocked::Exchange(&value_, _pOperand); }


    // =====================================================================================
    // 연산자 오버로딩
    // =====================================================================================
    operator T* () const { return Load(); }

    T* operator++() { return Add(1); }
    T* operator++(int) {
        T result = Add(1);
        return --result;
    }
    T* operator--() { return Add(-1); }
    T* operator--(int) {
        T* pResult = Add(-1);
        return ++pResult;
    }

    template <typename U, DefaultEnableIf_t<IsConvertible_v<U, T*>> = nullptr>
    TAtomic& operator=(U _pOther) { Exchange(_pOther); return *this; }
    T* operator+=(int _other) { return ExchangeAdd(_other) + (sizeof(T) * _other); }
    T* operator-=(int _other) { return ExchangeAdd(_other * -1) - (sizeof(T) * _other); }

    template <typename U, DefaultEnableIf_t<IsConvertible_v<U, T*>> = nullptr>
    bool operator==(U _pOther) { return Load() == _pOther; }
    template <typename U, DefaultEnableIf_t<IsConvertible_v<U, T*>> = nullptr>
    bool operator!=(U _pOther) { return Load() != _pOther; }

    T& operator[](const int _idx) { return Load()[_idx]; }
private:
    T* value_;
};

template <>
class Atomic<bool>
{
    using TAtomic = Atomic<bool>;
    using TInterlocked = Interlocked<bool>;
public:
    Atomic() : value_(false) {}
    Atomic(bool _value) : value_(_value) {}
    Atomic(TAtomic& _other) : value_(_other.Load()) {}

    void Store(bool _operand) { Exchange(_operand); }
    bool Load() const { return TInterlocked::Read(const_cast<bool*>(&value_)); }

    bool Exchange(bool _operand) { return TInterlocked::Exchange(&value_, _operand); }

    bool TryCompareExchange(bool _expected, bool _desired) { return CompareExchange(_expected, _desired); }
    bool CompareExchange(bool& _expected, bool _desired) {
        bool before = _expected;
        bool initial = TInterlocked::CompareExchange(&value_, _expected, _desired);

        if (before == initial)
            return true;

        _expected = initial;
        return false;
    }

    // =====================================================================================
    // 연산자 오버로딩
    // =====================================================================================
    operator bool() const { return Load(); }

    TAtomic& operator=(bool _other) { Exchange(_other); return *this; }
    bool operator==(bool _other) { return Load() == _other; }
    bool operator!=(bool _other) { return Load() != _other; }
private:
    bool value_;
};

template <>
class Atomic<void*>
{
    using TAtomic = Atomic<void*>;
    using TInterlocked = Interlocked<void*>;
public:
    Atomic() : value_(nullptr) {}
    Atomic(void* _pValue) : value_(_pValue) {}
    Atomic(TAtomic& _other) : value_(_other.Load()) {}

    void Store(void* _pOperand) { Exchange(_pOperand); }
    void* Load() const { return TInterlocked::Read(const_cast<void**>(&value_)); }

    void* Exchange(void* _pOperand) { return TInterlocked::Exchange(&value_, _pOperand); }

    bool TryCompareExchange(void* _pExpected, void* _pDesired) { return CompareExchange(_pExpected, _pDesired); }
    bool CompareExchange(void*& _pExpected, void* _pDesired) {
        void* pBefore = _pExpected;
        void* pInitial = TInterlocked::CompareExchange(&value_, _pExpected, _pDesired);

        if (pBefore == pInitial)
            return true;

        _pExpected = pInitial;
        return false;
    }

    // =====================================================================================
    // 연산자 오버로딩
    // =====================================================================================
    operator bool() const { return Load(); }

    TAtomic& operator=(void* _pOther) { Exchange(_pOther); return *this; }
    bool operator==(void* _pOther) { return Load() == _pOther; }
    bool operator!=(void* _pOther) { return Load() != _pOther; }
private:
    void* value_;
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


NS_END

