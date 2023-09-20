#include "header.h"
#include "Properties.h"

void k(const char* g) {
}

template <typename A>
void fn(const A& lhs) {
	using T = typename PropertyTypeDescription<A>::Ty;

	//Console::WriteLine("%s", *a);
}

struct A{};
struct B : A {};

using APtr = SharedPtr<A>;
using BPtr = SharedPtr<B>;

using AStdPtr = std::shared_ptr<A>;
using BStdPtr = std::shared_ptr<B>;

void g1(const APtr& s1) {
}

void g2(const AStdPtr& s2) {
	
}

int main() {
	InitializeDefaultLogger();
	PropertyStatics::Initialize();

	APtr a = MakeShared<A>();
	BPtr b = MakeShared<B>();
	g1(a);
	g1(b);

	AStdPtr _a = std::make_shared<A>();
	BStdPtr _b = std::make_shared<B>();

	g2(_a);
	g2(_b);

	JCORE_PASS;

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
	PropertyStatics::Finalize();
	return Console::ReadKeyWhile("X키 입력시 종료", ConsoleKey::X) ? 0 : -1;
}
