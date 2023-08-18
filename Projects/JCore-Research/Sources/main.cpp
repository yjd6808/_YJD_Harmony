#include "header.h"

struct _Base
{
	virtual ~_Base() = default;
	virtual void foo() {
		Console::WriteLine("_base");
	}
};

struct _Derived : _Base
{
	void foo() override {
		Console::WriteLine("_derived");
	}
};

int main() {
	_Derived v;
	_Base* z = &v;
	z->foo();

	int a[100];
	for (int i = 0; int k : std::vector{ 1, 2, 3, 4, 5 }) {
		a[++i] = k;
	}


	return 0;
}




