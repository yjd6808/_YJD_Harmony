/*
 * 작성자 : 윤정도
 * 목적 : MASM 어셈블리 코드에서 Console API를 호출하기 위한 extern "C" 래퍼
 */

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

/*
 * Console 초기화
 * @return true if successful, false otherwise
 */
int ConsoleInit();

/*
 * Console 콘솔 크기 설정
 * @param _width : 콘솔 너비
 * @param _height : 콘솔 높이
 * @return true if successful, false otherwise
 */
int ConsoleSetSize(int _width, int _height);

/*
 * 콘솔 색상 복원
 */
void ConsoleRestoreColor();

/*
 * 콘솔 색상 설정
 * @param _color : 색상 (ConsoleColor 열거형 값)
 */
void ConsoleSetColor(int _color);

/*
 * 현재 콘솔 색상 가져오기
 * @return 현재 색상
 */
int ConsoleGetColor();


int ConsoleWrite(const char* _pFmt);
int ConsoleWriteLine(const char* _pFmt);

int ConsoleWriteWithArgs(const char* _pFmt, ...);
int ConsoleWriteLineWithArgs(const char* _pFmt, ...);

/*
 * 콘솔 라인 입력
 * @param _pMsg : 프롬프트 메시지 (NULL 가능)
 * @param _pBuffer : 입력 버퍼
 * @param _capacity : 버퍼 크기
 * @return 입력된 문자 개수
 */
int ConsoleReadLineBuffered(const char* _pMsg, char* _pBuffer, int _capacity);

/*
 * 콘솔 클리어
 */
void ConsoleClear();

/*
 * 커서 위치 설정
 * @param _x : X 좌표
 * @param _y : Y 좌표
 */
void ConsoleSetCursorPosition(int _x, int _y);

/*
 * 현재 커서 위치 가져오기
 * @param _pX : X 좌표를 저장할 포인터
 * @param _pY : Y 좌표를 저장할 포인터
 */
void ConsoleGetCursorPosition(int* _pX, int* _pY);

/*
 * 출력 코드 페이지 설정
 * @param _codePage : 코드 페이지 (예: 65001 for UTF-8)
 * @return true if successful, false otherwise
 */
int ConsoleSetOutputCodePage(int _codePage);

/*
 * 현재 출력 코드 페이지 가져오기
 * @return 현재 코드 페이지
 */
int ConsoleGetOutputCodePage();

/*
 * 가상 터미널 모드 활성화/비활성화
 * @param _enabled : true to enable, false to disable
 * @return true if successful, false otherwise
 */
int ConsoleSetEnableVTMode(int _enabled);

#ifdef __cplusplus
}
#endif
