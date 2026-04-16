; ========================================================================
; 작성자 : 윤정도
; 목적 : RSPGame에서 사용하는 외부 함수 선언 및 상수 정의
; ========================================================================

OPTION CASEMAP: NONE

; ========================================================================
; Random 관련 extern 함수 선언
; ========================================================================
extern RandomInit : proc
extern RandomGenerateInt : proc
extern RandomGenerateDouble : proc
extern RandomGenerateAlphabat : proc
extern RandomChance : proc
extern RandomWriteAlphabatTextBuffered : proc

; ========================================================================
; Console 관련 extern 함수 선언
; ========================================================================
extern ConsoleInit : proc
extern ConsoleSetSize : proc
extern ConsoleRestoreColor : proc
extern ConsoleSetColor : proc
extern ConsoleGetColor : proc
extern ConsoleWrite : proc
extern ConsoleWriteLine : proc
extern ConsoleWriteWithArgs : proc
extern ConsoleWriteLineWithArgs : proc
extern ConsoleReadLineBuffered : proc
extern ConsoleClear : proc
extern ConsoleSetCursorPosition : proc
extern ConsoleGetCursorPosition : proc
extern ConsoleSetOutputCodePage : proc
extern ConsoleGetOutputCodePage : proc
extern ConsoleSetEnableVTMode : proc