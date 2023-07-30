#include "header.h"


struct Nice : RefCountObject
{
	Nice(int a) : A(a) {}

	
	void ReleaseAction() override { delete this; }

	int A;
};

struct King : Nice
{
	King() : Nice(1) {}
	King(const King& other) = delete;
};


int main() {
	Console::SetSize(800, 600);
	Random::EngineInitialize();

	RefCountObjectPtr<King> s1{ dbg_new King, false };
	RefCountObjectPtr<Nice> s2{ dbg_new King, false };

	{
		JCORE_REF_COUNT_GUARD(s1);
	}

	return 0;
}

