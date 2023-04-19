/*
 * 작성자: 윤정도
 * 생성일: 4/19/2023 6:07:32 PM
 * =====================
 *
 * 주의! NonCopyable<T>의 구현은 잘못된 것임!
 * @참고: https://en.wikibooks.org/wiki/Talk:More_C%2B%2B_Idioms/Non-copyable_Mixin
 *
 * template <class T>
 * class NonCopyable { NonCopyable() = default; T& operator=(const T&); };
 * class CantCopy : private NonCopyable<CantCopy> {};
 *
 * CanCopy a;
 * CanCopy b;
 * a = b가 컴파일 에러를 일으켜야하지만 CanCopy의 보이지 않는 CanCopy& operator= 함수가 존재함
 *
 * template <class T>
 * class NonCopyable { NonCopyable() = default; NonCopyable<T>& operator=(const NonCopyable<T>&); };
 * class CantCopy : private NonCopyable<CantCopy> {};
 *
 * CanCopy a;
 * CanCopy b;
 * a = b가 컴파일 에러를 정상적으로 일으킴. 근데 굳이 템플릿을 쓸 필요는 없을듯?
 */


#pragma once

#include <JCore/Namespace.h>

NS_JC_BEGIN


struct NonCopyableButMovable
{
protected:
	NonCopyableButMovable() = default;
public:
	NonCopyableButMovable(const NonCopyableButMovable& other) = delete;
	NonCopyableButMovable(NonCopyableButMovable&& other) = default;
	NonCopyableButMovable& operator=(const NonCopyableButMovable& other) = delete;
	NonCopyableButMovable& operator=(NonCopyableButMovable&& other) = default;
};

struct NonCopyable
{
protected:
	NonCopyable() = default;
public:
	NonCopyable(const NonCopyable& other) = delete;
	NonCopyable& operator=(const NonCopyable& other) = delete;
};

struct NonCopyableNonMovable
{
protected:
	NonCopyableNonMovable() = default;
public:
	NonCopyableNonMovable(const NonCopyableNonMovable& other) = delete;
	NonCopyableNonMovable(NonCopyableNonMovable&& other) = default;
	NonCopyableNonMovable& operator=(const NonCopyableNonMovable& other) = delete;
	NonCopyableNonMovable& operator=(NonCopyableNonMovable&& other) = default;
};


NS_JC_END