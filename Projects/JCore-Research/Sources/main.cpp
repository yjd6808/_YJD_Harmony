#include "header.h"

int main() {
	InitializeDefaultLogger();


	{
		Properties properties;

		constexpr int HP = 1;
		constexpr int Name = 3;

		properties.AddProperty(HP, PropertyType::Int16);
		properties.AddProperty(Name, PropertyType::String);

		properties[HP] = 200;
		properties[HP] += 1;

		// 프로퍼티 연산
		properties[Name] = "윤정도";
		properties[Name] += "a";
		properties[Name] += "b";		// 성공
		properties[Name] /= "0";		// 실패

		properties[HP] = 10 + properties[HP];
		properties[HP] /= 2;

		// 비교연산
		if (properties[HP] == 105) {
			Console::WriteLine("HP == 105입니다.", properties[HP].As<Int>());
		}

		Console::WriteLine("HP: %d", properties[HP].As<Int>());
		Console::WriteLine("Name: %s", properties[Name].Ref<String>().Source());

	}

	return Console::ReadKeyWhile("X키 입력시 종료", ConsoleKey::X) ? 0 : -1;
}
