#include "header.h"

int main() {
	auto t = DateTime::Now();
	JCore::TreeMap<JCore::DateTime, int> s;
	s.Insert(t, 100);

	s.Remove(t);

	return Console::ReadKeyWhile("X키 입력시 종료", ConsoleKey::X) ? 0 : -1;
}




