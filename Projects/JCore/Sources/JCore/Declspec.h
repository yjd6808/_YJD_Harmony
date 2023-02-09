/*
 *	작성자 : 윤정도
 *
 *	__declspec : 마이크로소프트 확장 기능으로 타입 또는 함수에 Storage Class 속성을 부여하는 것을 허용하는 키워드이다.
 *			     뭔 말이지, 일단 MSVC 전용기능 정도로 알아두자.
 *				 dllimport, dllexport 이거 2개는
 *	Storage Class : 저장소 종류?는 변수 또는 함수의 수명과 공개 범위?(Visibility)를 정의한다.
 *
 *	@참고1 : https://stackoverflow.com/questions/2284610/what-is-declspec-and-when-do-i-need-to-use-it
 *	@참고2 : https://www.tutorialspoint.com/cplusplus/cpp_storage_classes.htm
 */

#pragma once

 // novtable이란?
 // @내가 쓴 글 : https://blog.naver.com/wjdeh313/222733324896
#define JCORE_NOVTABLE	__declspec(novtable)
#define JCORE_EXPORT    __declspec(dllexport)
#define JCORE_IMPORT    __declspec(dllimport)