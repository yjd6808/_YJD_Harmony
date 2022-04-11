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

struct G
{
	char a[512];
};

template <typename T>
struct ListNodeA
{

	using TListNodeA = ListNodeA<T>;

	template <typename... Args>
	ListNodeA(Args&&... args) {
		Construct(Forward<Args>(args)...);
	}
	virtual ~ListNodeA() noexcept {
		Value.~T();
	}

	template <typename... Args>
	void Construct(Args&&... args) {
		new (__builtin_addressof(Value)) T{ Forward<Args>(args)... };
	}

	void DeleteSelf() const {
		Value.~T();
		delete this;
	}

	union { T Value; };	// Lazy Instantiation
	TListNodeA* Next = nullptr;
	TListNodeA* Previous = nullptr;
};





int main() {
	AutoMemoryLeakDetector detector;
	LinkedList<Tuple<int, String>> list;
	list.PushBack({ 1, "abfd" }); 
	list.EmplaceBack(1, "abfd"); 


	ListNode<String>* z = new ListNode<String>("abcd");	//

	delete z;


	return 0;
}