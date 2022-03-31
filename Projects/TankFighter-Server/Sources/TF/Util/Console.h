/*
	작성자 : 윤정도
 */

#pragma once

#pragma warning ( disable : 4244 )

#include <Common/Vec2Template.h>
#include <TF/Util/ConsoleColor.h>

class Console
{
	static Vec2Int		ms_CursorPosition;
	static ConsoleColor	ms_iDefaultColor;
public:
	static void SetConsoleSize(const int width, const int height);
	static void SetColor(ConsoleColor color);
	static void Write(ConsoleColor color, const char *format, ...);
	static void Write(const char *format, ...);
	static void WriteLine(ConsoleColor color, const char *format, ...);
	static void WriteLine(const char *format, ...);
	static void ErrorWriteLine(const char *format, ...);
	static void InfoWriteLine(const char *format, ...);
	static void DebugWriteLine(const char *format, ...);
	static void Init();
	static void Clear();
	static void SetCursorPosition(const Vec2Int & Vec2Int);

	static const Vec2Int& GetCursorPosition();
};

