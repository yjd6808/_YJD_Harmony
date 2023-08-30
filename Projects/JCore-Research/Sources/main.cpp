#include "header.h"
#include "Properties.h"

void k(const char* g) {
}

template <typename A>
void fn(const A& lhs) {
	using T = typename PropertyTypeDescription<A>::Ty;

	//Console::WriteLine("%s", *a);
}



int main() {
	InitializeDefaultLogger();
	PropertyStatics::Initialize();
	{
		Properties properties;
		properties.AddProperty(1, PropertyType::Int16);
		properties[1] += 200LL;
		properties[1] += 10.5f;

		Console::WriteLine("%d", properties[1].As<Int16>());

	}
	PropertyStatics::Finalize();

	FinalizeDefaultLogger();
	return Console::ReadKeyWhile("X키 입력시 종료", ConsoleKey::X) ? 0 : -1;
}
