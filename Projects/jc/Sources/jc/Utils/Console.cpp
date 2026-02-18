/*
 * 작성자 : 윤정도
 */

#include <jc/Core.h>
#include <jc/Utils/Console.h>

#include <Windows.h>

#include "jc/Threading/Thread.h"

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

//////////////////////////////////////////////////////////////////////////////////////////
bool Console::Init()
{
    ms_hStdout = WinApi::GetStdoutHandle();
    ms_hStdin = WinApi::GetStdinHandle();

    if (ms_hStdout == WinApi::InvalidHandleValue || ms_hStdin == WinApi::InvalidHandleValue)
    {
        return false;
    }

    return true;
}

//////////////////////////////////////////////////////////////////////////////////////////
bool Console::SetSize(int _width, int _height)
{
    HWND hConsole = GetConsoleWindow();

    if (hConsole == NULL)
        return false;

    RECT r;
    if (::GetWindowRect(hConsole, &r) == 0)
        return false;

    return ::MoveWindow(hConsole, r.left, r.top, _width, _height, TRUE) != 0;
}

//////////////////////////////////////////////////////////////////////////////////////////
void Console::SetColor(ConsoleColor _color)
{
    TLockGuard guard(ms_ConsoleLock);
    jc_assert_msg(ms_hStdout != WinApi::InvalidHandleValue, "출력 핸들이 이상합니다.");
    ms_iDefaultColor = _color;
    ::SetConsoleTextAttribute(ms_hStdout, static_cast<Int16>(_color));
}

//////////////////////////////////////////////////////////////////////////////////////////
void Console::GetColor(ConsoleColor _color)
{
    TLockGuard guard(ms_ConsoleLock);
    jc_assert_msg(ms_hStdout != WinApi::InvalidHandleValue, "출력 핸들이 이상합니다.");
    ms_iDefaultColor = _color;
    ::SetConsoleTextAttribute(ms_hStdout, static_cast<Int16>(_color));
}

//////////////////////////////////////////////////////////////////////////////////////////
ConsoleColor Console::GetColor()
{
    TLockGuard guard(ms_ConsoleLock);
    return ms_iDefaultColor;
}

//////////////////////////////////////////////////////////////////////////////////////////
ConsoleColor Console::ConvertColorString(const String& _colorString)
{
    if (_colorString == "black") { return Black; }
    if (_colorString == "blue") { return Blue; }
    if (_colorString == "green") { return Green; }
    if (_colorString == "cyan") { return Cyan; }
    if (_colorString == "red") { return Red; }
    if (_colorString == "magenta") { return Magenta; }
    if (_colorString == "yellow") { return Yellow; }
    if (_colorString == "lightgray") { return LightGray; }
    if (_colorString == "gray") { return Gray; }
    if (_colorString == "lightblue") { return LightBlue; }
    if (_colorString == "lightgreen") { return LightGreen; }
    if (_colorString == "lightcyan") { return LightCyan; }
    if (_colorString == "lightred") { return LightRed; }
    if (_colorString == "lightmagenta") { return LightMagenta; }
    if (_colorString == "lightyellow") { return LightYellow; }
    if (_colorString == "white") { return White; }
    return Max;
}

//////////////////////////////////////////////////////////////////////////////////////////
String Console::ReadLine()
{
    String line;
    char keyChar;

    while (std::cin.get(keyChar) && keyChar != '\n')
        line += keyChar;

    return line;
}

//////////////////////////////////////////////////////////////////////////////////////////
String Console::ReadLine(const char* _pMsg)
{
    Write("%s", _pMsg);
    return ReadLine();
}

//////////////////////////////////////////////////////////////////////////////////////////
int Console::ReadLineBuffered(const char* _pMsg, char* _pBuff, int _capacity)
{
    int index = 0;
    char keyChar;

    Write("%s", _pMsg);

    while (std::cin.get(keyChar) && index < _capacity && keyChar != '\n')
        _pBuff[index++] = keyChar;

    _pBuff[index] = NULL;
    return index;
}

//////////////////////////////////////////////////////////////////////////////////////////
ConsoleKeyInfo Console::ReadKey(const char* _pMsg)
{
    // 멀티쓰레딩시 인풋 동기화를 위해 사용
    static NormalLock s_Lock;

    if (ms_hStdin == WinApi::InvalidHandleValue)
    {
        return {};
    }

    if (_pMsg != nullptr)
		Write("%s", _pMsg);

    INPUT_RECORD inputRecord;
    DWORD eventsRead;
    BOOL result = FALSE;
    char keyChar;
    VirtualKey virtualKey;

    JC_LOCK_GUARD(s_Lock);

    for (;;)
    {
        result = ReadConsoleInput(ms_hStdin, &inputRecord, 1, &eventsRead);

        // 샐패하는 경우는 파일 또는 파이프로 리다이렉트 되는 경우
        if (result == FALSE || eventsRead == 0)
        {
            return {};
        }

        // 키 입력없는 경우 쉬엄쉬엄
        if (inputRecord.EventType != KEY_EVENT || inputRecord.Event.KeyEvent.bKeyDown == FALSE)
        {
            Thread::Sleep(10);
            continue;
        }

        virtualKey = (VirtualKey)inputRecord.Event.KeyEvent.wVirtualKeyCode;
        keyChar = inputRecord.Event.KeyEvent.uChar.AsciiChar;

        if (keyChar == 0)
        {
            if (virtualKey == VirtualKey::Alt || virtualKey == VirtualKey::ShiftKey || virtualKey == VirtualKey::CapsLock || virtualKey == VirtualKey::NumLock || virtualKey == VirtualKey::Scroll)
                continue;
        }

        break;
    }

    return ConsoleKeyInfo{ (ConsoleKey)virtualKey, keyChar };
}

//////////////////////////////////////////////////////////////////////////////////////////
ConsoleKeyInfo Console::ReadKeyWhile(const char* _pMsg, ConsoleKey _key)
{
    if (_pMsg)
        Write("%s", _pMsg);

    for (;;)
    {
        ConsoleKeyInfo info = ReadKey(nullptr);
        if (info && info.Key == _key)
        {
            return info;
        }
    }
}

//////////////////////////////////////////////////////////////////////////////////////////
void Console::Clear()
{
    TLockGuard guard(ms_ConsoleLock);
    CRuntime::System("cls");
}

//////////////////////////////////////////////////////////////////////////////////////////
void Console::SetCursorPosition(int _x, int _y)
{
    TLockGuard guard(ms_ConsoleLock);
    jc_assert(ms_hStdout != WinApi::InvalidHandleValue);
    ms_iCursorPosX = _x;
    ms_iCursorPosY = _y;
    WinApi::SetConsoleCursorPosition(ms_hStdout, _x, _y);
}

//////////////////////////////////////////////////////////////////////////////////////////
Tuple<int, int> Console::GetCursorPosition()
{
    jc_assert(ms_hStdout != WinApi::InvalidHandleValue);
    int cursorX;
    int cursorY;
    if (WinApi::GetConsoleCursorPosition(ms_hStdout, cursorX, cursorY))
    {
        return { cursorX,  cursorY };
    }
    return { -1, -1 };
}

//////////////////////////////////////////////////////////////////////////////////////////
bool Console::SetOutputCodePage(int _codePage)
{
    bool result = WinApi::SetConsoleOutputCodePage(_codePage);
    // DebugAssertMessage(bRet, "인코딩 변경 실패");
    return result;
}

//////////////////////////////////////////////////////////////////////////////////////////
bool Console::SetEnableVTMode(bool _enabled)
{
    Int32UL mode = 0;
    if (!GetConsoleMode(ms_hStdout, &mode))
        return false;

    if (_enabled)
        mode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING;    // 0x0004
    else
        mode &= ~ENABLE_VIRTUAL_TERMINAL_PROCESSING;

    if (!SetConsoleMode(ms_hStdout, mode))
        return false;

    return true;
}

//////////////////////////////////////////////////////////////////////////////////////////
int Console::GetOutputCodePage()
{
    return WinApi::GetConsoleOutputCodePage();
}

NS_END

