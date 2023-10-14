/*
 * 작성자 : 윤정도
 */

#include <JCore/Core.h>
#include <JCore/Utils/Console.h>

#include <Windows.h>

#include "JCore/Threading/Thread.h"

NS_JC_BEGIN
	const char*      Console::VTForeColor[ConsoleColor::Max]{
    // 사이에 공백 없도록 주의!
    CSI_GRAPHIC_RENDITION(0;30),  // Black  
    CSI_GRAPHIC_RENDITION(0;34),  // Blue
    CSI_GRAPHIC_RENDITION(0;32),  // Green
    CSI_GRAPHIC_RENDITION(0;36),  // Cyan
    CSI_GRAPHIC_RENDITION(0;31),  // Red
    CSI_GRAPHIC_RENDITION(0;35),  // Magenta
    CSI_GRAPHIC_RENDITION(0;33),  // Yellow
    CSI_GRAPHIC_RENDITION(0;1),   // LightGray
    CSI_GRAPHIC_RENDITION(0;0),   // Gray
    CSI_GRAPHIC_RENDITION(0;94),  // LightBlue
    CSI_GRAPHIC_RENDITION(0;92),  // LightGreen
    CSI_GRAPHIC_RENDITION(0;96),  // LightCyan
    CSI_GRAPHIC_RENDITION(0;91),  // LightRed
    CSI_GRAPHIC_RENDITION(0;95),  // LightMagenta
    CSI_GRAPHIC_RENDITION(0;93),  // LightYellow
    CSI_GRAPHIC_RENDITION(0;97)   // White
};

const char*      Console::VTBackColor[ConsoleColor::Max]{
    CSI_GRAPHIC_RENDITION(0;30),  // Black  
    CSI_GRAPHIC_RENDITION(0;34),  // Blue
    CSI_GRAPHIC_RENDITION(0;32),  // Green
    CSI_GRAPHIC_RENDITION(0;36),  // Cyan
    CSI_GRAPHIC_RENDITION(0;31),  // Red
    CSI_GRAPHIC_RENDITION(0;35),  // Magenta
    CSI_GRAPHIC_RENDITION(0;33),  // Yellow
    CSI_GRAPHIC_RENDITION(0;1),   // LightGray
    CSI_GRAPHIC_RENDITION(0;0),   // Gray
    CSI_GRAPHIC_RENDITION(0;94),  // LightBlue
    CSI_GRAPHIC_RENDITION(0;92),  // LightGreen
    CSI_GRAPHIC_RENDITION(0;96),  // LightCyan
    CSI_GRAPHIC_RENDITION(0;91),  // LightRed
    CSI_GRAPHIC_RENDITION(0;95),  // LightMagenta
    CSI_GRAPHIC_RENDITION(0;93),  // LightYellow
    CSI_GRAPHIC_RENDITION(0;97)   // White
};

const char*     Console::VTForeToken[ConsoleColor::Max] {
	VT_FORE_COLOR_BLACK        ,
	VT_FORE_COLOR_BLUE         ,
	VT_FORE_COLOR_GREEN        ,
	VT_FORE_COLOR_CYAN         ,
	VT_FORE_COLOR_RED          ,
	VT_FORE_COLOR_MAGNETA      ,
	VT_FORE_COLOR_YELLOW       ,
	VT_FORE_COLOR_LIGHT_GRAY   ,
	VT_FORE_COLOR_GRAY         ,
	VT_FORE_COLOR_LIGHT_BLUE   ,
	VT_FORE_COLOR_LIGHT_GREEN  ,
	VT_FORE_COLOR_LIGHT_CYAN   ,
	VT_FORE_COLOR_LIGHT_RED    ,
	VT_FORE_COLOR_LIGHT_MAGNETA,
	VT_FORE_COLOR_LIGHT_YELLOW ,
	VT_FORE_COLOR_WHITE  
};

const char*     Console::VTBackToken[ConsoleColor::Max] {
    VT_BACK_COLOR_BLACK         ,
    VT_BACK_COLOR_BLUE          ,
    VT_BACK_COLOR_GREEN         ,
    VT_BACK_COLOR_CYAN          ,
    VT_BACK_COLOR_RED           ,
    VT_BACK_COLOR_MAGNETA       ,
    VT_BACK_COLOR_YELLOW        ,
    VT_BACK_COLOR_LIGHT_GRAY    ,
    VT_BACK_COLOR_GRAY          ,
    VT_BACK_COLOR_LIGHT_BLUE    ,
    VT_BACK_COLOR_LIGHT_GREEN   ,
    VT_BACK_COLOR_LIGHT_CYAN    ,
    VT_BACK_COLOR_LIGHT_RED     ,
    VT_BACK_COLOR_LIGHT_MAGNETA ,
    VT_BACK_COLOR_LIGHT_YELLOW  ,
    VT_BACK_COLOR_WHITE
};


bool Console::Init() {
    ms_hStdout = WinApi::GetStdoutHandle();
    ms_hStdin = WinApi::GetStdinHandle();

    if (ms_hStdout == WinApi::InvalidHandleValue || ms_hStdin == WinApi::InvalidHandleValue) {
        return false;
    }

    return true;
}

bool Console::SetSize(int width, int height) {
    HWND hConsole = GetConsoleWindow();

    if (hConsole == NULL)
        return false;

    RECT r;
    if (::GetWindowRect(hConsole, &r) == 0)
        return false;

    return ::MoveWindow(hConsole, r.left, r.top, width, height, TRUE) != 0;
}

void Console::SetColor(ConsoleColor color) {
    TLockGuard guard(ms_ConsoleLock);
    DebugAssertMsg(ms_hStdout != WinApi::InvalidHandleValue, "출력 핸들이 이상합니다.");
    ms_iDefaultColor = color;
    ::SetConsoleTextAttribute(ms_hStdout, static_cast<Int16>(color));
}

void Console::GetColor(ConsoleColor color) {
    TLockGuard guard(ms_ConsoleLock);
    DebugAssertMsg(ms_hStdout != WinApi::InvalidHandleValue, "출력 핸들이 이상합니다.");
    ms_iDefaultColor = color;
    ::SetConsoleTextAttribute(ms_hStdout, static_cast<Int16>(color));
}

ConsoleColor Console::GetColor() {
    TLockGuard guard(ms_ConsoleLock);
    return ms_iDefaultColor;
}

ConsoleColor Console::ConvertColorString(const String& colorString) {
    if (colorString == "black") { return Black; }
    if (colorString == "blue") { return Blue; }
    if (colorString == "green") { return Green; }
    if (colorString == "cyan") { return Cyan; }
    if (colorString == "red") { return Red; }
    if (colorString == "magenta") { return Magenta; }
    if (colorString == "yellow") { return Yellow; }
    if (colorString == "lightgray") { return LightGray; }
    if (colorString == "gray") { return Gray; }
    if (colorString == "lightblue") { return LightBlue; }
    if (colorString == "lightgreen") { return LightGreen; }
    if (colorString == "lightcyan") { return LightCyan; }
    if (colorString == "lightred") { return LightRed; }
    if (colorString == "lightmagenta") { return LightMagenta; }
    if (colorString == "lightyellow") { return LightYellow; }
    if (colorString == "white") { return White; }
    return Max;
}

String Console::ReadLine() {
    String s;
    char ch;

    while (std::cin.get(ch) && ch != '\n')
        s += ch;

    return s;
}

String Console::ReadLine(const char* msg) {
    Write("%s", msg);
    return ReadLine();
}

ConsoleKeyInfo Console::ReadKey(const char* msg) {

    // 멀티쓰레딩시 인풋 동기화를 위해 사용
    static NormalLock s_Lock;

    if (ms_hStdin == WinApi::InvalidHandleValue) {
        return {};
    }

    if (msg != nullptr)
		Write("%s", msg);

    INPUT_RECORD inputRecord;
    DWORD ulNumberOfeventRead;
    BOOL iResult = FALSE;
    char chKeyChar;
    VirtualKey eVirtualKey;

    JCORE_LOCK_GUARD(s_Lock);

    for (;;) {
        iResult = ReadConsoleInput(ms_hStdin, &inputRecord, 1, &ulNumberOfeventRead);

        // 샐패하는 경우는 파일 또는 파이프로 리다이렉트 되는 경우
        if (iResult == FALSE || ulNumberOfeventRead == 0) {
            return {};
        }

        // 키 입력없는 경우 쉬엄쉬엄
        if (inputRecord.EventType != KEY_EVENT || inputRecord.Event.KeyEvent.bKeyDown == FALSE) {
            Thread::Sleep(10);
            continue;
        }

        eVirtualKey = (VirtualKey)inputRecord.Event.KeyEvent.wVirtualKeyCode;
        chKeyChar = inputRecord.Event.KeyEvent.uChar.AsciiChar;

        if (chKeyChar == 0) {
            if (eVirtualKey == VirtualKey::Alt || eVirtualKey == VirtualKey::ShiftKey || eVirtualKey == VirtualKey::CapsLock || eVirtualKey == VirtualKey::NumLock || eVirtualKey == VirtualKey::Scroll)
                continue;
        }

        
        break;
    }

    return ConsoleKeyInfo{ (ConsoleKey)eVirtualKey, chKeyChar };
}

ConsoleKeyInfo Console::ReadKeyWhile(const char* msg, ConsoleKey key) {

    if (msg)
        Write("%s", msg);

    for (;;) {
        ConsoleKeyInfo info = ReadKey(nullptr);
        if (info && info.Key == key) { 
            return info;
        }
    }
}

void Console::Clear() {
    TLockGuard guard(ms_ConsoleLock);
    CRuntime::System("cls");
}

void Console::SetCursorPosition(int x, int y) {
    TLockGuard guard(ms_ConsoleLock);
    DebugAssert(ms_hStdout != WinApi::InvalidHandleValue);
    ms_iCursorPosX = x;
    ms_iCursorPosY = y;
    WinApi::SetConsoleCursorPosition(ms_hStdout, x, y);
}

Tuple<int, int> Console::GetCursorPosition() {
    DebugAssert(ms_hStdout != WinApi::InvalidHandleValue);
    int x;
    int y;
    if (WinApi::GetConsoleCursorPosition(ms_hStdout, x, y)) {
        return { x,  y };
    }
    return { -1, -1 };
}

bool Console::SetOutputCodePage(int codePage) {
    bool bRet = WinApi::SetConsoleOutputCodePage(codePage);
    // DebugAssertMessage(bRet, "인코딩 변경 실패");
    return bRet;
}

bool Console::SetEnableVTMode(bool enabled) {
    Int32UL dwMode = 0;
    if (!GetConsoleMode(ms_hStdout, &dwMode))
        return false;

    if (enabled)
        dwMode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING;    // 0x0004
    else
        dwMode &= ~ENABLE_VIRTUAL_TERMINAL_PROCESSING;

    if (!SetConsoleMode(ms_hStdout, dwMode))
        return false;

    return true;
}

int Console::GetOutputCodePage() {
    return WinApi::GetConsoleOutputCodePage();
}


NS_JC_END
