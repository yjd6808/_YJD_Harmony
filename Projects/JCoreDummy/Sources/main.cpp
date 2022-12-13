
#include "header.h"
#include "JCore/Pool/MemoryPoolManager.h"


#include "JCore/System/SingletonLifeCycle.h"


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

struct DefaultAllocator
{
	template <typename T>
	static T* Allocate() {
		return (T*)operator new(sizeof(T));
	}
};

template<typename T, typename TAllocator = DefaultAllocator, typename... Args>
T* Create(Args&&... args) {
	T* ptr = TAllocator::template Allocate<T>();
	::new (ptr) T{ Forward<Args>(args)... };
	return ptr;
}

struct A1 : SingletonLifeCycle<A1, 1, 11>
{
	A1() { Console::WriteLine("%d", GetConstructionPriority()); }
	~A1() { Console::WriteLine("%d", GetDestructionPriority()); }
};
struct A2 : SingletonLifeCycle<A2, 5, 16>
{
	A2() { Console::WriteLine("%d", GetConstructionPriority()); }
	~A2() { Console::WriteLine("%d", GetDestructionPriority()); }
};
struct A3 : SingletonLifeCycle<A3, 3, 13>
{
	A3() { Console::WriteLine("%d", GetConstructionPriority()); }
	~A3() { Console::WriteLine("%d", GetDestructionPriority()); }
};
struct A4 : SingletonLifeCycle<A4, 3, 18>
{
	A4() { Console::WriteLine("%d", GetConstructionPriority()); }
	~A4() { Console::WriteLine("%d", GetDestructionPriority()); }
};
struct A5 : SingletonLifeCycle<A5, 4, 13>
{
	A5() { Console::WriteLine("%d", GetConstructionPriority()); }
	~A5() { Console::WriteLine("%d", GetDestructionPriority()); }
};
struct A6 : SingletonLifeCycle<A6, 7, 14>
{
	A6() { Console::WriteLine("%d", GetConstructionPriority()); }
	~A6() { Console::WriteLine("%d", GetDestructionPriority()); }
};


extern int main();


int main() {
	JCoreSystem::Initialize();
	std::cout << "\n";
	//AutoMemoryLeakDetector detector;
	g_DefaultMemoryPoolManager.Register<eSingle, eBinarySearch, 0>(false);
	g_DefaultMemoryPoolManager.Initialize<eSingle, eBinarySearch, 0>
	({
		{1, 2048},
		{2, 2048},
		{4, 2048},
		{8, 2048},
		{16, 2048},
		{32, 2048},
		{64, 2048},
		{128, 2048},
		{256, 2048},
		{512, 2048},
		{1024, 2048},
	});

	DateTime::Now();
	void* a = g_DefaultMemoryPoolManager.Get<eSingle, eBinarySearch, 0>()->StaticPop<16>();
	g_DefaultMemoryPoolManager.Get<eSingle, eBinarySearch, 0>()->StaticPush<16>(a);
	Vector<BucketNode<int, int>> s;

	Vector<TestData> s1;
	Vector<IData*> s2;
	s2.PushBack(new TestData(1, 2));
	delete s2[0];


	Console::Init();
    Console::SetOutputCodePage(CodePage::UTF8);
	return 0;
}

