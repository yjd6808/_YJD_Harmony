/*
 * 작성자: 윤정도
 * 생성일: 2/17/2023 5:14:50 PM
 * =====================
 *
 */


#pragma once

#include <JCore/Namespace.h>

NS_JC_BEGIN

/*
	무브 컨켓테스트
	중간에 const char(&)[] 문자열을 만날때마다 문자열을 생성하기때문에
	매개변수가 전부 R타입과 동일할때 효율이 제일 좋다.
	
	String c = "c";       // c: NULL
	String d = "d";       // d: NULL
	String g = MoveConcat<String>("a", "b", c, d, "e"); // abcde
	
*/

template <typename R, typename T>
static R MoveConcat(T&& str) {
	R ret = Move(str);
	return ret;
}

template <typename R, typename T, typename... TOther>
static R MoveConcat(T&& a1, TOther&&... strings) {
	R ret = Move(a1);
	ret += MoveConcat<R>(Forward<TOther>(strings)...);
	return ret;
}

/*
	카피 컨켓테스트
	String c = "c";       // c: "c"
	String d = "d";       // d: "d'
	String g = CopyConcat<String>("a", "b", c, d, "e"); // abcde
*/

template <typename R, typename T>
static R CopyConcat(T&& str) {
	R ret = str;
	return ret;
}

template <typename R, typename T, typename... TOther>
static R CopyConcat(T&& a1, TOther&&... strings) {
	R ret = a1;
	ret += CopyConcat<R>(Forward<TOther>(strings)...);
	return ret;
}
NS_JC_END