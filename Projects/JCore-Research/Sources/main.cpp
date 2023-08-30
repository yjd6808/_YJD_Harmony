#include "header.h"
#include "Properties.h"

void k(const char* g) {
}

template <typename A>
void fn(const A& lhs) {
	using T = typename PropertyArgumentDescription<A>::Ty;

	//Console::WriteLine("%s", *a);
}



int main() {
	InitializeDefaultLogger();
	PropertyStatics::Initialize();
	{
		Properties properties;
		//PropertyStatics::EnableOperatorLog = false;
		constexpr int HP = 1;
		constexpr int NAME = 2;

		/*properties.AddProperty(HP, PropertyType::Int);
		properties[HP] += 20;
		properties[HP] += 20.0;
		properties[HP] /= 10.0;
		Console::WriteLine("%d", properties[HP].GetValue<Int>());*/

		
		const char* zz = "gg";
		properties.AddProperty(NAME, PropertyType::String);

		properties[NAME] += zz;
		properties[NAME] += "abcd";
		properties[NAME] += "efg";

	}
	PropertyStatics::Finalize();

	FinalizeDefaultLogger();
	return Console::ReadKeyWhile("X키 입력시 종료", ConsoleKey::X) ? 0 : -1;
}
