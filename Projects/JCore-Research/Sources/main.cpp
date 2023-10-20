#include "header.h"

int main() {
	new char;
	InitializeDefaultLogger();

	Properties a;

	a.Add(1, PropertyType::String);
	a[1] = "abcd";


	auto prop = a.Get(1);
	if (prop == nullptr)
		return -1;
	String s1 = prop->As<String>();
	const String& s2 = a.Get(1)->CRef<String>();
	String& s3 = a.Get(1)->Ref<String>();
	s3 = "abcde";

	FinalizeDefaultLogger();
	return Console::ReadKeyWhile("X키 입력시 종료", ConsoleKey::X) ? 0 : -1;
}
