#pragma once

#include <JCore/Type.h>
#include <JCore/Namespace.h>

NS_JC_BEGIN

// 한정자 제거해주는 템플릿
// 포인터 타입은 제거가 안댐
template <typename T>
struct RemoveQulifier
{
    using Type = T;
};

template <typename T>
struct RemoveQulifier<const T>
{
    using Type = T;
};

template <typename T>
struct RemoveQulifier<volatile T>
{
    using Type = T;
};

template <typename T>
struct RemoveQulifier<volatile const T>
{
    using Type = T;
};

template <typename T>
using RemoveQulifier_t = typename RemoveQulifier<T>::Type;


// 참조 제거자
template <typename T>
struct RemoveReference
{
    using Type = T;
};

template <typename T>
struct RemoveReference<T&>
{
    using Type = T;
};

template <typename T>
struct RemoveReference<T&&>
{
    using Type = T;
};

template <typename T>
using RemoveReference_t = typename RemoveReference<T>::Type;


// 배열 괄호 제거해서 기본 타입만 얻도록 하는 템플릿
template <typename T>
struct RemoveArray
{
    using Type = T;
};

template <typename T>
struct RemoveArray<T[]>
{
    using Type = T;
};

template <typename T, Int32U ArraySize>
struct RemoveArray<T[ArraySize]>
{
    using Type = T;
};

template <typename T>
using RemoveArray_t = typename RemoveArray<T>::Type;
/* =============================================================== */


/* =============================================================== */
template <typename T>
struct RevmovePointer
{
    using Type = T;
};

template <typename T>
struct RevmovePointer<T*>
{
    using Type = T;
};


template <typename T>
using RemovePointer_t = typename RevmovePointer<T>::Type;


// 한정자, 참조타입을 모두 제거해주는 템플릿
// ex) const voaltile int&& -> int타입으로댐
template <typename T>
struct NaturalType
{
    using Type = RemoveQulifier_t<RemoveReference_t<T>>;
};

// 한정자/참조자 제거
// const volatile int&  => int
// const volatile int*  => int*
// const volatile int[] => int[]
template <typename T>
using NaturalType_t = typename NaturalType<T>::Type;

// 걍 다 벗김
// const volatile int*[] => int
template <typename T>
using NakedType_t= RemoveArray_t<RemovePointer_t<NaturalType_t<T>>>;

NS_JC_END