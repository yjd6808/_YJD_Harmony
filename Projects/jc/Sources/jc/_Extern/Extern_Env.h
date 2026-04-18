/*
 * 작성자 : 윤정도
 * 목적 : MASM 어셈블리 코드에서 Env API를 호출하기 위한 extern "C" 래퍼
 * created by AI.
 */

#pragma once

#ifdef __cplusplus
#include "jc/Type.h"
extern "C" {
#endif

/*
 * timeGetTime() 기반 32비트 밀리초 타이머 반환
 * @return 시스템 시작 이후 경과 시간 (밀리초, 32비트)
 */
_u32 Env_TimeGetTime();

/*
 * 앱 실행 후 경과 시간 반환 (마이크로초 단위)
 * @return TimeSpan::Tick 값 (_s64, 마이크로초)
 */
_s64 Env_AppTime();

/*
 * 앱 실행 후 경과 시간 반환 (timeGetTime 기반, 밀리초 단위)
 * @return 경과 시간 (밀리초, 32비트)
 */
_u32 Env_AppTimeTgt();

/*
 * 시스템(OS) 부팅 후 경과 시간 반환 (GetTickCount64 기반, 마이크로초 단위)
 * @return TimeSpan::Tick 값 (_s64, 마이크로초)
 */
_s64 Env_SystemTime();

/*
 * 시스템(OS) 부팅 후 경과 시간 반환 (timeGetTime 64비트 확장, 밀리초 단위)
 * @return TimeSpan::Tick 값 (_s64, 밀리초)
 */
_s64 Env_SystemTimeTgt();

#ifdef __cplusplus
}
#endif
