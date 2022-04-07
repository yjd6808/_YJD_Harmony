#define _CRT_SECURE_NO_WARNINGS

#include "header.h"


template <typename T>
struct Setter;

template <Int32U Size>
struct Setter<char[Size]>
{
	void operator()(const char* a) {

	}
};


template <>
struct Setter<JCore::DateTime>
{
	void operator()(const DateTime& dt) {
	}
};

template <>
struct Setter<int&>
{
	void operator()(const int val) {
	}
};

template <>
struct Setter<JCore::String>
{
	void operator()(const String& dt) {

	}
};



template <typename T>
void Arg(T&& val) {
	Setter<NaturalType_t<T>>()(val);
}


struct B
{
	int a;
	int b;
	int c;
};

struct A
{
	int z;
	B f[0];
};


int main() {
	AutoMemoryLeakDetector detector;

	HashMap<int, String> nodes;
	nodes.Insert( 0, "abcd" );


	LinkedList<Tuple<int, String>> list;
	list.PushBack({ 1, "abfd" });		// 메모리릭 32bytes
	list.EmplaceBack(1, "abfd");		// 메모리릭 없음





	return 0;
}