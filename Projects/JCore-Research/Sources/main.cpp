#include "header.h"

int main() {
	InitializeJCore();
	InitializeDefaultLogger();


	FinalizeDefaultLogger();
	FinalizeJCore();
	return Console::ReadKeyWhile("X키 입력시 종료", ConsoleKey::X) ? 0 : -1;
}
