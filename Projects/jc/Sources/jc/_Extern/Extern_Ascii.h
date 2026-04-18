/*
 * 작성자 : 윤정도
 * 목적 : MASM 어셈블리 코드에서 Ascii API를 호출하기 위한 extern "C" 래퍼
 * created by AI.
 */

#pragma once

#ifdef __cplusplus
#include "jc/Type.h"
extern "C" {
#endif

/*
 * 알파벳 문자인지 확인 (a-z, A-Z)
 * @param _ch : 검사할 문자
 * @return 알파벳이면 1, 아니면 0
 */
_s32 Ascii_IsAlphabat(_s8 _ch);

/*
 * 소문자 알파벳 문자인지 확인 (a-z)
 * @param _ch : 검사할 문자
 * @return 소문자 알파벳이면 1, 아니면 0
 */
_s32 Ascii_IsLowerCaseAlphabat(_s8 _ch);

/*
 * 대문자 알파벳 문자인지 확인 (A-Z)
 * @param _ch : 검사할 문자
 * @return 대문자 알파벳이면 1, 아니면 0
 */
_s32 Ascii_IsUpperCaseAlphabat(_s8 _ch);

/*
 * 숫자 문자인지 확인 (0-9)
 * @param _ch : 검사할 문자
 * @return 숫자이면 1, 아니면 0
 */
_s32 Ascii_IsNumeric(_s8 _ch);

/*
 * 공백 문자인지 확인 (space, tab, newline 등)
 * @param _ch : 검사할 문자
 * @return 공백이면 1, 아니면 0
 */
_s32 Ascii_IsSpace(_s8 _ch);

/*
 * 문자를 소문자로 변환
 * @param _ch : 변환할 문자
 * @return 소문자로 변환된 문자 (알파벳이 아닌 경우 원본 반환)
 */
_s8 Ascii_ToLower(_s8 _ch);

/*
 * 문자를 대문자로 변환
 * @param _ch : 변환할 문자
 * @return 대문자로 변환된 문자 (알파벳이 아닌 경우 원본 반환)
 */
_s8 Ascii_ToUpper(_s8 _ch);

#ifdef __cplusplus
}
#endif
