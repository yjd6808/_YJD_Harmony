/*
 * 작성자 : 윤정도
 * 목적 : MASM 어셈블리 코드에서 Console API를 호출하기 위한 extern "C" 래퍼
 */

#pragma once

#ifdef __cplusplus
#include "jc/Type.h"
extern "C" {
#endif

/*
 * Console 초기화
 * @return true if successful, false otherwise
 */
_s32 Console_Init();

/*
 * Console 콘솔 크기 설정
 * @param _width : 콘솔 너비
 * @param _height : 콘솔 높이
 * @return true if successful, false otherwise
 */
_s32 Console_SetSize(_s32 _width, _s32 _height);

/*
 * 콘솔 색상 복원
 */
void Console_RestoreColor();

/*
 * 콘솔 색상 설정
 * @param _color : 색상 (ConsoleColor 열거형 값)
 */
void Console_SetColor(_s32 _color);

/*
 * 현재 콘솔 색상 가져오기
 * @return 현재 색상
 */
_s32 Console_GetColor();

_s32 Console_Write(const _s8* _pFmt);
_s32 Console_WriteLine(const _s8* _pFmt);

_s32 Console_WriteWithArgs(const _s8* _pFmt, ...);
_s32 Console_WriteLineWithArgs(const _s8* _pFmt, ...);

/*
 * 콘솔 라인 입력
 * @param _pMsg : 프롬프트 메시지 (NULL 가능)
 * @param _pBuffer : 입력 버퍼
 * @param _capacity : 버퍼 크기
 * @return 입력된 문자 개수
 */
_s32 Console_ReadLineBuffered(const _s8* _pMsg, _s8* _pBuffer, _s32 _capacity);

/*
 * 콘솔 클리어
 */
void Console_Clear();

/*
 * 커서 위치 설정
 * @param _x : X 좌표
 * @param _y : Y 좌표
 */
void Console_SetCursorPosition(_s32 _x, _s32 _y);

/*
 * 현재 커서 위치 가져오기
 * @param _pX : X 좌표를 저장할 포인터
 * @param _pY : Y 좌표를 저장할 포인터
 */
void Console_GetCursorPosition(_s32* _pX, _s32* _pY);

/*
 * 출력 코드 페이지 설정
 * @param _codePage : 코드 페이지 (예: 65001 for UTF-8)
 * @return true if successful, false otherwise
 */
_s32 Console_SetOutputCodePage(_s32 _codePage);

/*
 * 현재 출력 코드 페이지 가져오기
 * @return 현재 코드 페이지
 */
_s32 Console_GetOutputCodePage();

/*
 * 가상 터미널 모드 활성화/비활성화
 * @param _enabled : true to enable, false to disable
 * @return true if successful, false otherwise
 */
_s32 Console_SetEnableVTMode(_s32 _enabled);

#ifdef __cplusplus
}
#endif
