#include "header.h"

int main(int _argc, char** _argv) 
{
	InitializeJCore(_argc, _argv);
	InitializeDefaultLogger();

	{
		// Vector / ArrayStack / ArrayQueue
		Vector<int> vec;
		vec.PushBack(1);
		vec.PushBack(2);
		vec.PushBack(3);

		ArrayStack<int> arrStack;
		arrStack.Push(10);
		arrStack.Push(20);
		arrStack.Push(30);

		ArrayQueue<int> arrQueue;
		arrQueue.Enqueue(100);
		arrQueue.Enqueue(200);
		arrQueue.Enqueue(300);

		// HashMap / HashSet
		HashMap<String, int> hmap;
		hmap.Insert(String("one"), 1);
		hmap.Insert(String("two"), 2);
		hmap.Insert(String("three"), 3);

		HashSet<String> hset;
		hset.Insert(String("apple"));
		hset.Insert(String("banana"));
		hset.Insert(String("cherry"));

		// LinkedList / ListQueue / ListStack
		LinkedList<int> list;
		list.PushBack(1);
		list.PushBack(2);
		list.PushBack(3);

		ListQueue<int> listQueue;
		listQueue.Enqueue(11);
		listQueue.Enqueue(22);
		listQueue.Enqueue(33);

		ListStack<int> listStack;
		listStack.Push(111);
		listStack.Push(222);
		listStack.Push(333);

		// TreeMap / TreeSet
		TreeMap<int, String> treeMap;
		treeMap.Insert(1, String("one"));
		treeMap.Insert(2, String("two"));
		treeMap.Insert(3, String("three"));

		TreeSet<int> treeSet;
		treeSet.Insert(5);
		treeSet.Insert(1);
		treeSet.Insert(9);

		// Tuple / Pair
		Tuple<int, String> tup{ 42, String("answer") };
		Pair<String, int> pair{ String("key"), 777 };

		// SharedPtr / WeakPtr
		SharedPtr<int> sp = MakeShared<int>(1234);
		SharedPtr<int> sp2;
		WeakPtr<int> wp = sp;
		WeakPtr<int> wp2;
		UniquePtr<int> up = MakeUnique<int>(5678);

		// String
		String s("Hello Natvis");
		s.Append(" Test");

		// Time / DateTime / Date / TimeSpan
		DateTime now = DateTime::Now();
		Date date = now.ToDate();
		Time time = now.ToTime();
		TimeSpan ts = TimeSpan::FromSecond(5);

		jc::CMessage msg1(32, 5);
		msg1.WriteU32(120);
		msg1.WriteU32(120);
		msg1.WriteU32(120);
		msg1.WriteU32(120);
		msg1.WriteString("abcdefg1");
		msg1.WriteString("abcdefg2");
		msg1.WriteString("abcdefg3");
		msg1.WriteString("abcdefg4");

		_u32 a1 = msg1.ReadU32();
		_u32 a2 = msg1.ReadU32();
		_u32 a3 = msg1.ReadU32();
		_u32 a4 = msg1.ReadU32();
		String str1 = msg1.ReadString();
		String str2 = msg1.ReadString();
		String str3 = msg1.ReadString();
		String str4 = msg1.ReadString();

		// 브레이크포인트 찍어서 각 변수들 natvis 확인
		(void)vec;
		(void)arrStack;
		(void)arrQueue;
		(void)hmap;
		(void)hset;
		(void)list;
		(void)listQueue;
		(void)listStack;
		(void)treeMap;
		(void)treeSet;
		(void)tup;
		(void)pair;
		(void)sp;
		(void)sp2;
		(void)wp;
		(void)wp2;
		(void)up;
		(void)s;
		(void)now;
		(void)date;
		(void)time;
		(void)ts;

	}

	Console::Write("%s", "x키 입력시 종료");
	//Console::ReadKeyWhile("X키 입력시 종료", ConsoleKey::X);
	FinalizeDefaultLogger();
	FinalizeJCore();
	return 0;
}
