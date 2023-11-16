#include "header.h"

int main() {
	InitializeJCore();
	InitializeDefaultLogger();

	Vector v{ 7, 2, 3, 5, 1 };
	// Arrays::HeapSort(v.Source(), v.Size());
	Arrays::MakeHeap(v.Source(), v.Size());

	Arrays::PopHeap(v.Source(), v.Size());
	Console::WriteLine("%d", v.Back());
	v.PopBack();

	Arrays::PopHeap(v.Source(), v.Size());
	Console::WriteLine("%d", v.Back());
	v.PopBack();

	Arrays::PopHeap(v.Source(), v.Size());
	Console::WriteLine("%d", v.Back());
	v.PopBack();

	Console::WriteLine("%d", v.Back());
	v.PopBack();

	Arrays::PopHeap(v.Source(), v.Size());
	Console::WriteLine("%d", v.Back());
	v.PopBack();

	FinalizeDefaultLogger();
	FinalizeJCore();
	return Console::ReadKeyWhile("X키 입력시 종료", ConsoleKey::X) ? 0 : -1;
}
