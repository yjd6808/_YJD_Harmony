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
	return 0;
}




