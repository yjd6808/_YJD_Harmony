#include "header.h"

int main() {
	GG g = Const::K;
	return Console::ReadKeyWhile("X키 입력시 종료", ConsoleKey::X) ? 0 : -1;
}




