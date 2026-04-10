#include "Core.h"

static auto fn_running_thread = []
{
	jc::Thread th([] (void* _param)
		{
		}, nullptr, "RunningThread", true);
	th.Abort();
	return 10;
}();

#pragma pack(push, 1)
struct check_point
{
	// callee saved registers
	_u64 rbx_ = 0;
	_u64 rbp_ = 0;
	_u64 rdi_ = 0;
	_u64 rsp_ = 0;
	_u64 r12_ = 0;
	_u64 r13_ = 0;
	_u64 r14_ = 0;
	_u64 r15_ = 0;
	char xmm_[16 * 10]{}; // xmm6 ~ xmm15
	bool saved_ = false;
} ck;
#pragma pack(pop)



int main(int _argc, char** _argv) 
{
	InitializeJCore(0, nullptr);
	InitializeDefaultLogger();

	while (true)
	{

		#define PRESS_ANY_KEY Console::WriteLine(ConsoleColor::Green, "아무 키나 눌러서 계속하세요..."); Console::ReadKey()
		Console::Clear();
		Console::WriteLine("========================================");
		Console::WriteLine("           학습 프로그램 메뉴");
		Console::WriteLine("========================================");
		Console::WriteLine("[1] ASM Study - 어셈블리 기초 학습");
		Console::WriteLine("[2] Heap Study - 힙 메모리 학습");
		Console::WriteLine("[3] RSP Game - 가위바위보 게임");
		Console::WriteLine("[0] 종료");
		Console::WriteLine("========================================");
		
		String input = Console::ReadLine("수행할 함수를 선택> ");
		if (input == "1")
		{
			call_01_AsmStudy(_argc, _argv);
			PRESS_ANY_KEY;
		}
		else if (input == "2")
		{
			call_02_HeapStudy(_argc, _argv);
			PRESS_ANY_KEY;
		}
		else if (input == "3")
		{
			call_03_RSPGame(_argc, _argv);
			PRESS_ANY_KEY;
		}
		else if (input == "0")
		{
			Console::WriteLine("\n프로그램을 종료합니다.");
			break;
		}
		else
		{
			Console::WriteLine("아무 키나 눌러서 계속하세요...");
			Console::ReadKey();
		}
	}

	FinalizeDefaultLogger();
	FinalizeJCore();
	return 0;
}


//constexpr int map[100]{};
////constexpr CTPlaceholderInfo ct = CTPlaceholderParse("{0} {1}       {2}");
////static_assert(ct.errorCode_ == 0);
////static_assert(ct.count_ == 3);
//
//static_assert(StringUtil::CTFind("a select b", "select") == 2);
//static_assert(StringUtil::CTCount("a select b", "e") == 2);
//
//{
//	// Vector / ArrayStack / ArrayQueue
//	Vector<int> vec;
//	vec.PushBack(1);
//	vec.PushBack(2);
//	vec.PushBack(3);
//
//	ArrayStack<int> arrStack;
//	arrStack.Push(10);
//	arrStack.Push(20);
//	arrStack.Push(30);
//
//	ArrayQueue<int> arrQueue;
//	arrQueue.Enqueue(100);
//	arrQueue.Enqueue(200);
//	arrQueue.Enqueue(300);
//
//	// HashMap / HashSet
//	HashMap<String, int> hmap;
//	hmap.Insert(String("one"), 1);
//	hmap.Insert(String("two"), 2);
//	hmap.Insert(String("three"), 3);
//
//	HashSet<String> hset;
//	hset.Insert(String("apple"));
//	hset.Insert(String("banana"));
//	hset.Insert(String("cherry"));
//
//	// LinkedList / ListQueue / ListStack
//	LinkedList<int> list;
//	list.PushBack(1);
//	list.PushBack(2);
//	list.PushBack(3);
//
//	ListQueue<int> listQueue;
//	listQueue.Enqueue(11);
//	listQueue.Enqueue(22);
//	listQueue.Enqueue(33);
//
//	ListStack<int> listStack;
//	listStack.Push(111);
//	listStack.Push(222);
//	listStack.Push(333);
//
//	// TreeMap / TreeSet
//	TreeMap<int, String> treeMap;
//	treeMap.Insert(1, String("one"));
//	treeMap.Insert(2, String("two"));
//	treeMap.Insert(3, String("three"));
//
//	TreeSet<int> treeSet;
//	treeSet.Insert(5);
//	treeSet.Insert(1);
//	treeSet.Insert(9);
//
//	// Tuple / Pair
//	Tuple<int, String> tup{ 42, String("answer") };
//	Pair<String, int> pair{ String("key"), 777 };
//
//	// SharedPtr / WeakPtr
//	SharedPtr<int> sp = MakeShared<int>(1234);
//	SharedPtr<int> sp2;
//	WeakPtr<int> wp = sp;
//	WeakPtr<int> wp2;
//	UniquePtr<int> up = MakeUnique<int>(5678);
//
//	// String
//	String s("Hello Natvis");
//	s.Append(" Test");
//
//	// Time / DateTime / Date / TimeSpan
//	DateTime now = DateTime::Now();
//	Date date = now.ToDate();
//	Time time = now.ToTime();
//	TimeSpan ts = TimeSpan::FromSecond(5);
//
//
//	{
//		jc::CMessage msg1(32, 5);
//		_u8* check = dbg_new _u8[32];
//		check[0] = 20;
//		check[4] = 20;
//		check[8] = 20;
//		check[16] = 20;
//		int* pfsef = dbg_new int();
//		_s8* pS8 = nullptr;
//		msg1.WriteS8(-50);
//		msg1.WriteS8(-100);
//		msg1.WriteU8(160);
//		msg1.WriteS16(-30000);
//		msg1.WriteU16(60000);
//		msg1.WriteS32(-2000000000);
//		msg1.WriteU32(4000000000);
//		msg1.WriteS64(-9000000000000000000LL);
//		msg1.WriteU64(18000000000000000000ULL);
//		msg1.WritePtr(pfsef);
//		msg1.WriteFloat(3.141592f);
//		msg1.WriteDouble(2.718281828459045);
//		msg1.WriteBinary(check, 32);
//		msg1.WriteBinary(check, 32);
//		msg1.WriteString("abcdefg1");
//		msg1.WriteString("abcdefg2");
//		msg1.WriteString("abcdefg3");
//		msg1.WriteString("abcdefg4");
//
//		auto a = msg1.GetCurrentVT();
//		msg1.ReadS8();
//		a = msg1.GetCurrentVT();
//		msg1.ReadS8();
//
//		a = msg1.ReadAny();
//
//		msg1.SetReadOffset(0);
//		a = msg1.ReadAny();
//
//
//		msg1.SetReadOffset(10);
//		_f32 fesf = msg1.ReadFloat();
//
//		msg1.SetReadOffset(8);
//		_u64 fsefesf = msg1.ReadU64();
//
//		msg1.SetReadOffset(14);
//		String sefse = msg1.ReadString();
//
//		msg1.SetWriteOffset(14);
//		msg1.SetReadOffset(14);
//		msg1.WriteString("aaa");
//		String fsef = msg1.ReadString();
//		Console::WriteLine(msg1.Dump().SafeSource());
//		delete pfsef;
//		delete[] check;
//
//		CDataMap map;
//		map.SetS32("intKey", -123);
//		map.SetString("abcd", "ffesfes");
//		map.SetString("abcd", "ffesfesfsefse");
//	}
//
//
//	// 브레이크포인트 찍어서 각 변수들 natvis 확인
//	(void)vec;
//	(void)arrStack;
//	(void)arrQueue;
//	(void)hmap;
//	(void)hset;
//	(void)list;
//	(void)listQueue;
//	(void)listStack;
//	(void)treeMap;
//	(void)treeSet;
//	(void)tup;
//	(void)pair;
//	(void)sp;
//	(void)sp2;
//	(void)wp;
//	(void)wp2;
//	(void)up;
//	(void)s;
//	(void)now;
//	(void)date;
//	(void)time;
//	(void)ts;
//
//}
//
//Console::Write("%s", "x키 입력시 종료");
////Console::ReadKeyWhile("X키 입력시 종료", ConsoleKey::X);