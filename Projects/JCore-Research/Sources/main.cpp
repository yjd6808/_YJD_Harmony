#include "header.h"

template <int Type>
struct A { static_assert(Type == 0, "... Invalid Cmd Type"); };
int main() {
	Console::SetSize(800, 600);
	Random::EngineInitialize();

	return 0;
}

