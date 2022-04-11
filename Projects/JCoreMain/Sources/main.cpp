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
	LinkedList<Tuple<int, String>> list;
	list.PushBack({ 1, "abfd" }); 
	list.EmplaceBack(1, "abfd"); 


	HashMap<int, String> nodes;

	Random  rand;
	int a = rand.GenerateInt(0, 300);
	for (int i = 0; i < a; i++) {
		String z = "abcd";
		nodes.Insert(i, z + i);
		nodes.Values().Extension().ForEach([](String& str) {
			std::cout << str << "\n";
			});
	}

	for (int i = 0; i < a; i++) {
		if (!nodes.Remove(i)) {
			DebugAssert(false, "fdsf");
		}
	}





	return 0;
}