#include <vector>

#include "header.h"

int main() {

	return Console::ReadKeyWhile("X키 입력시 종료", ConsoleKey::X) ? 0 : -1;
}
