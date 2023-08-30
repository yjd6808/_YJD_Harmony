#include "header.h"
#include "Properties.h"



int main() {
	InitializeDefaultLogger();
	PropertyStatics::Initialize();
	{
		Properties properties;
		//PropertyStatics::EnableOperatorLog = false;
		constexpr int HP = 1;

		properties.AddProperty(HP, PropertyType::Int);
		properties[HP] += 20;
		properties[HP] += 20.0;
		properties[HP] /= 10.0;
		Console::WriteLine("%d", properties[HP].GetValue<Int>());

	}
	PropertyStatics::Finalize();

	FinalizeDefaultLogger();
	return Console::ReadKeyWhile("X키 입력시 종료", ConsoleKey::X) ? 0 : -1;
}
