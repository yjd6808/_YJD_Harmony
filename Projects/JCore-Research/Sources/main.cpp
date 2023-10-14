#include <vector>

#include "header.h"

int main() {

	InitializeDefaultLogger();
	Properties s;
	PropertyStatics::EnableOperatorLog = true;

	s.Add(1, PropertyType::Bool);
	s.Add(2, PropertyType::Bool);
	s.Add(3, PropertyType::Int64);
	s.Add(4, PropertyType::Int64U);
	s.Add(5, PropertyType::Int32UL);


	s[1] = true;
	s[2] = false;
	s[3] = 1;
	s[4] = 0;
	s[5] = 100;


	const bool a1 = s[1] == s[2];	
	const bool a2 = s[1] != s[2];
	const bool a3 = s[1] == s[3];
	const bool a4 = s[1] != s[3];
	const bool a5 = s[1] == s[4];
	const bool a6 = s[1] != s[4];
	const bool a7 = s[1] == s[5];
	const bool a8 = s[1] != s[5];

	const bool a11 = s[1] == false;
	const bool a12 = s[1] != false;
	const bool a13 = s[1] == 1;
	const bool a14 = s[1] != 1;
	const bool a15 = s[1] == 0;
	const bool a16 = s[1] != 0;
	const bool a17 = s[1] == 100;
	const bool a18 = s[1] != 100;


	FinalizeDefaultLogger();
	return Console::ReadKeyWhile("X키 입력시 종료", ConsoleKey::X) ? 0 : -1;
}
