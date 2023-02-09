/*
	작성자 : 윤정도
	C#에 있는 기능을 살짝 흉내냄
*/

#pragma once

#include <functional>

NS_JC_BEGIN

/* ====================================================================
 [사용법]

 (1) 람다를 이용한 초기화
 Action<int&> increaser = [](int& ref) { ref++; };


 (2) 펑터를 이용한 초기화
 struct Increaser {
	operator() (int& ref) {
		ref++;
	}
 }

 Action<int&> increaser = Increase();

 --------------------------------------------------------------------
 [사용]

 int a = 0;
 increaser(a); // a = 1

 ==================================================================== */


// 반환값이 없는 대리자
template <typename... Args>
using Action = std::function<void(Args...)>;

// 반환값이 있는 대리자
template <typename R, typename... Args>
using Func = std::function<R(Args...)>;

// 참/거짓을 판별하는 대리자
template <typename... Args>
using Predicate = std::function<bool(Args...)>;

NS_JC_END