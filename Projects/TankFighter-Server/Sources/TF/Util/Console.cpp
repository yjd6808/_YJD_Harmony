/*
	작성자 : 윤정도
 */

#include <TF/PrecompiledHeader.h>


#pragma warning ( disable : 4244 )

#include <random>

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

#include <mutex>

#include <TF/Util/Console.h>

#define BUF_LEN	1024

using namespace JCore;

	Vec2Int Console::ms_CursorPosition;
	ConsoleColor Console::ms_iDefaultColor;

	void  Console:: SetConsoleSize(const int width, const int height) {
		char a[BUF_LEN];
		sprintf_s(a, " mode  con lines=%d   cols=%d ", height, width);
		system(a);
	}

	void Console::SetColor(ConsoleColor color) {
		ms_iDefaultColor = color;
		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), static_cast<WORD>(color));
	}

	void Console::Write(ConsoleColor color, const char *format, ...) {
		va_list ap;
		char buf[BUF_LEN];
		va_start(ap, format);
		vsprintf_s(buf, BUF_LEN, format, ap);
		va_end(ap);
		if (ms_ConsoleLock)
			ms_ConsoleLock->Lock();

		SetColor(color);
		fprintf(stdout, "%s", buf);
		SetColor(ms_iDefaultColor);

		if (ms_ConsoleLock)
			ms_ConsoleLock->Unlock();
	}

	void Console::Write(const char *format, ...) {
		va_list ap;
		char buf[BUF_LEN];
		va_start(ap, format);
		vsprintf_s(buf, BUF_LEN, format, ap);
		va_end(ap);

		if (ms_ConsoleLock)
			ms_ConsoleLock->Lock();
		fprintf(stdout, "%s", buf);
		if (ms_ConsoleLock)
			ms_ConsoleLock->Unlock();
	}

	void Console::WriteLine(ConsoleColor color, const char *format, ...) {
		va_list ap;
		char buf[BUF_LEN];
		va_start(ap, format);
		vsprintf_s(buf, BUF_LEN, format, ap);
		va_end(ap);
		if (ms_ConsoleLock)
			ms_ConsoleLock->Lock();

		SetColor(color);
		fprintf(stdout, "%s\n", buf);
		SetColor(ms_iDefaultColor);

		if (ms_ConsoleLock)
			ms_ConsoleLock->Unlock();
	}

	void Console::WriteLine(const char *format, ...) {
		va_list ap;
		char buf[BUF_LEN];
		va_start(ap, format);
		vsprintf_s(buf, BUF_LEN, format, ap);
		va_end(ap);

		if (ms_ConsoleLock)
			ms_ConsoleLock->Lock();

		fprintf(stdout, "%s\n", buf);

		if (ms_ConsoleLock)
			ms_ConsoleLock->Unlock();
	}

	void Console::Init() {
		CONSOLE_SCREEN_BUFFER_INFO a;

		GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &a);
		ms_CursorPosition = {a.dwCursorPosition.X, a.dwCursorPosition.Y };
	}

	void Console::Clear() {
		system("cls");
	}

	void Console::SetCursorPosition(const Vec2Int & Vec2Int) {
		ms_CursorPosition = Vec2Int;
		COORD p;
		p.X = ms_CursorPosition.X;
		p.Y = ms_CursorPosition.Y;
		SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), p);
	}

	const Vec2Int& Console::GetCursorPosition() {
		return ms_CursorPosition;
	}



