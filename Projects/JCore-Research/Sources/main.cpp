#include "header.h"

struct A
{
	A() = default;
	A(const A&) { cout << "copy\n"; }
	A(A&&) { cout << "move\n"; }
	~A() { cout << "del\n"; }
	int a;
};

#pragma warning(push, 3)

struct Example0
{
	int b[0];
};

struct Example1
{
	int b[];
};

struct Example2
{
	int a;
	int b[0];

	static constexpr bool ConstructCountableElement = true;

	static Example2* Construct(void* p, int count) {


	}

};

struct Example3
{
	int a;
	int b[];
};

struct Example4
{
	int b[1];
};

struct Example5
{
};

struct Example6 : Example5
{
	char dummy;
};

struct Example7
{
	char dummy;
};

#pragma warning(pop)

// Flexible-Array가 존재하는 이유
// C/C++에서는 크기 0인 배열(Zero Size Array)을
// 사용하는 이유는 가변길이

int main() {
	InitializeJCore();
	InitializeDefaultLogger();

	Example5 ex_array[2];

	std::cout
		<< std::boolalpha
		<< "ex[0]와 ex[1]의 주소가 다를까?: "
		<< ((&ex_array[0] != &ex_array[1]) ? "네 달라요" : "아뇨 같아요.")
		<< "\n";

	Example2 _2;
	Example3 _3;

	constexpr int K22 = sizeof _2.b;
	constexpr int K33 = sizeof _3.b;

	Example6 ex;
	void* a = &ex;
	void* b = &ex.dummy;
	if (a == b)
		std::cout << "aa\n";

	Console::WriteLine("%d", sizeof(Example0));
	Console::WriteLine("%d", sizeof(Example1));
	Console::WriteLine("%d", sizeof(Example2));
	Console::WriteLine("%d", sizeof(Example3));
	Console::WriteLine("%d", sizeof(Example4));
	Console::WriteLine("%d", sizeof(Example5));
	Console::WriteLine("%d", sizeof(Example6));
	Console::WriteLine("%d", sizeof(Example7));

	Vector v{ 5, 2, 7, 3, 8, 1 };	v.ForEach([](int val) { std::cout << val << " "; }); std::cout << "\n";
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
