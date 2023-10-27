/*
 * 작성자: 윤정도
 * 생성일: 10/27/2023 1:04:33 PM
 * =====================
 *
 */


#pragma once

#include <JCore/Namespace.h>

NS_JC_BEGIN

/*
 * JCore 라이브러리를 사용한다면 프로그램 종료 직전 마지막에 FinalizeJCore() 함수를 무조건 호출해줄 것
 * NormalLock의 EnterCriticalSection이 main 함수가 끝난 이후로 오류를 일으킨다. (윈도우 라이브러리 문제 추정)
 * AppExited 상태를 변경해서 NormalLock 사용하는 기능들애 대해서 락을 처리하지 않도록 하기 위함이다.
 */

void InitializeJCore();
void FinalizeJCore();

extern bool AppExited;
extern long long AppStartUpTime_v;

NS_JC_END