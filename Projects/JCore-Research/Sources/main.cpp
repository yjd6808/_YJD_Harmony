#include "header.h"

int main() {
	InitializeJCore();
	InitializeDefaultLogger();

	Vector v{ 5, 2, 7, 3, 8, 1 };

	v.ForEach([](int val) { std::cout << val << " "; }); std::cout << "\n";
	Arrays::MakeHeap(v.Source(), v.Size());	// Max Heap으로 만든다.

	v.ForEach([](int val) { std::cout << val << " "; }); std::cout << "\n";
	Arrays::PopHeap(v.Source(), v.Size());
	v.ForEach([](int val) { std::cout << val << " "; }); std::cout << "\n";
	
	Arrays::PopHeap(v.Source(), v.Size() - 1);
	v.ForEach([](int val) { std::cout << val << " "; }); std::cout << "\n";

	Arrays::PopHeap(v.Source(), v.Size() - 2);
	v.ForEach([](int val) { std::cout << val << " "; }); std::cout << "\n";

	Arrays::PopHeap(v.Source(), v.Size() - 3);
	v.ForEach([](int val) { std::cout << val << " "; }); std::cout << "\n";

	Arrays::PopHeap(v.Source(), v.Size() - 4);
	v.ForEach([](int val) { std::cout << val << " "; }); std::cout << "\n";

	Arrays::PopHeap(v.Source(), v.Size() - 5);
	v.ForEach([](int val) { std::cout << val << " "; }); std::cout << "\n";

	FinalizeDefaultLogger();
	FinalizeJCore();
	return Console::ReadKeyWhile("X키 입력시 종료", ConsoleKey::X) ? 0 : -1;
}
