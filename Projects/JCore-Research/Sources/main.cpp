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

#define CASE_1(a)			CASE_1_(1, 2, a)
#define CASE_1_(a, b, c)	a + b + c


#define CASE_2(a)			CASE_2_(1, a)
#define CASE_2_(...)		CASE_2__(__VA_ARGS__, 3)
#define CASE_2__(a, b, c)	a + b + c

#define CASE_3_ELEMS(...)	__VA_ARGS__, 1, 2
#define CASE_3(...)			CASE_2_(CASE_3_ELEMS(__VA_ARGS__))
// #define CASE_3_(a, b, c)	a + b + c
#define CASE_3_(a)	a


struct S
{
	int a;
	int b;
};


#define CASE_4_ELEMENTS(a, b)	{ a, b }
#define CASE_4(a, b)			CASE_4_(CASE_4_ELEMENTS(a, b))
#define CASE_4_(a)				a

constexpr S case4 CASE_4(1, 2);		// constexpr S case4{1, 2};로 확장됨

// CASE4에서 발생한 마이크로소프트 전처리기만의 고유 문제를 확장 매크로를 활용해서 해결
#define CASE_5_ELEMENTS(a, b)	{ a, b }
#define CASE_5(a, b)			CASE_5_(CASE_5_ELEMENTS(a, b))
#define CASE_5_(...)			JCORE_EXPAND_1(CASE_5__(__VA_ARGS__))		// 확장 매크로 수행
#define CASE_5__(a, b)			a, b

constexpr S case5 CASE_5(1, 2);		// constexpr S case5{1, 2};로 확장됨

#define CASE_6_ELEMENTS			1, 2
#define CASE_6					JCORE_EXPAND_1(CASE_6_(CASE_6_ELEMENTS))
#define CASE_6_					



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
