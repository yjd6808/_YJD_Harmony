/*
	작성자 : 윤정도
 */

#pragma warning ( disable : 4244 )

#define _CRT_SECURE_NO_WARNINGS

#include <iostream>
#include <random>
#include <Windows.h>
#include <mutex>

#include <TF/Util/Console.h>

	Vec2Int Console::ms_CursorPosition;
	ConsoleColor Console::ms_iDefaultColor;

	void  Console:: SetConsoleSize(const int width, const int height)
	{
		char a[500];
		sprintf_s(a, " mode  con lines=%d   cols=%d ", height, width);
		system(a);
	}

	void Console::SetColor(ConsoleColor color)
	{
		ms_iDefaultColor = color;
		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), static_cast<WORD>(color));
	}

	void Console::Write(ConsoleColor color, const char *format, ...)
	{
		va_list ap;
		char buf[1024];
		va_start(ap, format);
		vsprintf(buf, format, ap);
		va_end(ap);
		SetColor(color);
		fprintf(stdout, "%s", buf);
		SetColor(ms_iDefaultColor);
	}

	void Console::Write(const char *format, ...)
	{
		va_list ap;
		char buf[1024];
		va_start(ap, format);
		vsprintf(buf, format, ap);
		va_end(ap);
		fprintf(stdout, "%s", buf);
	}

	void Console::WriteLine(ConsoleColor color, const char *format, ...)
	{
		va_list ap;
		char buf[1024];
		va_start(ap, format);
		vsprintf(buf, format, ap);
		va_end(ap);
		SetColor(color);
		fprintf(stdout, "%s\n", buf);
		SetColor(ms_iDefaultColor);
	}

	void Console::WriteLine(const char *format, ...)
	{
		va_list ap;
		char buf[1024];
		va_start(ap, format);
		vsprintf(buf, format, ap);
		va_end(ap);
		fprintf(stdout, "%s\n", buf);
	}



	void Console::ErrorWriteLine(const char *format, ...)
	{
		SetColor(ConsoleColor::LIGHTRED);
		va_list ap;
		char buf[1024];
		va_start(ap, format);
		vsprintf(buf, format, ap);
		va_end(ap);
		fprintf(stdout, "[Error] %s\n", buf);

	}

	void Console::InfoWriteLine(const char *format, ...)
	{
		SetColor(ConsoleColor::GREEN);
		va_list ap;
		char buf[1024];
		va_start(ap, format);
		vsprintf(buf, format, ap);
		va_end(ap);
		fprintf(stdout, "[Info] %s\n", buf);
	}

	void Console::DebugWriteLine(const char *format, ...)
	{
		SetColor(ConsoleColor::DARKGRAY);
		va_list ap;
		char buf[1024];
		va_start(ap, format);
		vsprintf(buf, format, ap);
		va_end(ap);
		fprintf(stdout, "[Debug] %s\n", buf);
	}


	void Console::Init()
	{
		CONSOLE_SCREEN_BUFFER_INFO a;

		GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &a);
		ms_CursorPosition = {a.dwCursorPosition.X, a.dwCursorPosition.Y };
	}

	void Console::Clear()
	{
		system("cls");
	}

	void Console::SetCursorPosition(const Vec2Int & Vec2Int)
	{
		ms_CursorPosition = Vec2Int;
		COORD p;
		p.X = ms_CursorPosition.X;
		p.Y = ms_CursorPosition.Y;
		SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), p);
	}

	const Vec2Int& Console::GetCursorPosition()
	{
		return ms_CursorPosition;
	}



