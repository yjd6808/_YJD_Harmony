#include "header.h"

template <typename T, Int32U Size>
static void Fill(T(&arr)[Size], T val) {
	Console::WriteLine("%s", typeid(T).name());
	for (int i = 0; i < Size; ++i) arr[i] = val;
}

int main() {
	return Console::ReadKeyWhile("X키 입력시 종료", ConsoleKey::X) ? 0 : -1;
}




