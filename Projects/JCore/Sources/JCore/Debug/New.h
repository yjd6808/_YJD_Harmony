/*
 * 작성자: 윤정도
 * 생성일: 2/11/2023 12:20:01 AM
 * =====================
 *
 */


#pragma once

#include <JCore/Type.h>
#include <JCore/Define.h>

/*
 * 정녕.. 그냥 new와 operator new에 파일명 라인 출력을 할 수 없는건가..
 * 1. new 오버라이딩시 내부에서 파일명 출력을 할려면 caller의 파일명이 필요한데
 *    그럴려면 stacktrace 정보가 필요함.
 *     -> 시도해봤는데 너무 느림. 모든 동적할당시마다 stacktrace를 얻는거 자체가 말도안된다.
 */

#if DebugMode
	#define dbg_new new (_NORMAL_BLOCK, JCORE_FILENAME, __LINE__)
	#define dbg_operator_new(size) operator new((size), _NORMAL_BLOCK, JCORE_FILENAME, __LINE__)
#else
	#define dbg_new new
	#define dbg_operator_new(size) operator new((size))
#endif