
#include "header.h"



using Console = NormalConsole;

struct IData
{
	virtual void A() = 0;
};

struct TestData : IData{
	TestData(){}
	TestData(int a, int b) {}

	void A() override {
		
	}
	int a;
	int b;
};

//struct A1 : PrioritySingleton<A1, 1, 11>
//{
//	A1() { Console::WriteLine("%d", GetConstructionPriority()); }
//	~A1() { Console::WriteLine("%d", GetDestructionPriority()); }
//};
//struct A2 : PrioritySingleton<A2, 5, 16>
//{
//	A2() { Console::WriteLine("%d", GetConstructionPriority()); }
//	~A2() { Console::WriteLine("%d", GetDestructionPriority()); }
//};
//struct A3 : PrioritySingleton<A3, 3, 13>
//{
//	A3() { Console::WriteLine("%d", GetConstructionPriority()); }
//	~A3() { Console::WriteLine("%d", GetDestructionPriority()); }
//};
//struct A4 : PrioritySingleton<A4, 3, 18>
//{
//	A4() { Console::WriteLine("%d", GetConstructionPriority()); }
//	~A4() { Console::WriteLine("%d", GetDestructionPriority()); }
//};
//struct A5 : PrioritySingleton<A5, 4, 13>
//{
//	A5() { Console::WriteLine("%d", GetConstructionPriority()); }
//	~A5() { Console::WriteLine("%d", GetDestructionPriority()); }
//};
//struct A6 : PrioritySingleton<A6, 7, 14>
//{
//	A6() { Console::WriteLine("%d", GetConstructionPriority()); }
//	~A6() { Console::WriteLine("%d", GetDestructionPriority()); }
//};


extern int main();


int main() {
	JCore::Initialize();
	JCore::NormalConsole::Init();
	JCore::NormalConsole::SetOutputCodePage(CodePage::UTF8);



	Vector<BucketNode<int, int>> s;
	Vector<TestData> s1;
	Vector<IData*> s2;
	s2.PushBack(new TestData(1, 2));
	delete s2[0];


	
	return 0;
}

