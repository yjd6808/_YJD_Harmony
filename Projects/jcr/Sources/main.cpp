#include "header.h"


constexpr int PLACEHOLDER_MAX = 100;
constexpr int PLACEHOLDER_ERROR_CAN_NOT_FIND_OPEN_BRACE		= -1;	
constexpr int PLACEHOLDER_ERROR_INVALID_FORMAT				= -2;			
constexpr int PLACEHOLDER_ERROR_CAN_NOT_FIND_CLOSE_BRACE	= -3;
constexpr int PLACEHOLDER_ERROR_TOO_LONG_CONTENT			= -4;
constexpr int PLACEHOLDER_ERROR_NON_NUMERIC_CONTENT			= -5;
constexpr int PLACEHOLDER_ERROR_INVALID_NUMBER				= -6;
constexpr int PLACEHOLDER_ERROR_MISSING_NUMBER				= -7;

struct CTPlaceholderInfo
{
	int errorCode_ = 0;
	int count_ = 0;
	int map_[PLACEHOLDER_MAX]{};

	static constexpr CTPlaceholderInfo Error(int _errorCode)
	{
		CTPlaceholderInfo r;
		r.errorCode_ = _errorCode;
		return r;
	}
};

constexpr CTPlaceholderInfo CTPlaceholderParse(const char* _pStr)
{
	
	// {0}, {1}, {2} ... 이런식으로 포맷팅할 때, 몇 개의 플레이스홀더가 있는지 세는 함수
	// 중간에 비는게 있으면 -1을 반환하여 올바르지 않은 플레이스 홀더를 나타내도록 한다.
	char temp[32]{};
	int i = 0;
	int len = StringUtil::CTLength(_pStr);
	int maxNum = -1;
	CTPlaceholderInfo r;
	do
	{
		i = StringUtil::CTFindChar(_pStr, '{', i);
		if (i <= -1)
			return CTPlaceholderInfo::Error(-1); // { 문자가 없는 경우
		if (i + 1 >= len)
			return CTPlaceholderInfo::Error(-2); // {이 마지막 문자인 경우
		int e = StringUtil::CTFindChar(_pStr, '}', i + 1);
		if (e <= -1)
			return CTPlaceholderInfo::Error(-3); // { 다음에 } 문자가 없는 경우
		int sz = e - i - 1;
		if (sz >= 32)
			return CTPlaceholderInfo::Error(-4); // 플레이스홀더 내용이 말도 안되는 경우
		StringUtil::CTCopy(temp, 32, _pStr + i + 1, sz);
		for (int k = 0; k < sz; ++k)
			if (temp[k] < '0' || temp[k] > '9')
				return CTPlaceholderInfo::Error(-5); // 플레이스홀더 내용이 숫자가 아닌 경우
		int num = StringUtil::CTToInt32(temp);
		if (num < 0 || num >= PLACEHOLDER_MAX)
			return CTPlaceholderInfo::Error(-6); // 플레이스홀더 번호가 음수이거나 최대 번호(100)를 넘는 경우
		++r.map_[num]; // 존재하는 플레이스홀더 표시
		if (num > maxNum)
			maxNum = num;
		StringUtil::CTZeroMemory(temp, 32);
		i = e + 1;
	} while (i < len);

	for (int j = 0; j <= maxNum; ++j)
		if (r.map_[j] == 0) // 플레이스홀더 번호가 중간에 비어있는 경우 {0}{1}{3}
			return CTPlaceholderInfo::Error(-7);
	int count = 0;
	for (int j = 0; j <= maxNum; ++j)
		count += r.map_[j];
	r.count_ = count;
	return r;
}



int main(int _argc, char** _argv) 
{
	InitializeJCore(_argc, _argv);
	InitializeDefaultLogger();

	//_CrtSetBreakAlloc(886);

	constexpr int map[100]{};
	constexpr CTPlaceholderInfo ct = CTPlaceholderParse("{0} {1}       {2}");
	static_assert(ct.errorCode_ == 0);
	static_assert(ct.count_ == 3);

	static_assert(StringUtil::CTFind("a select b", "select") == 2);
	static_assert(StringUtil::CTCount("a select b", "e") == 2);

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


		{
			jc::CMessage msg1(32, 5);
			_u8* check = dbg_new _u8[32];
			check[0] = 20;
			check[4] = 20;
			check[8] = 20;
			check[16] = 20;
			int* pfsef = dbg_new int();
			_s8* pS8 = nullptr;
			msg1.WriteS8(-50);
			msg1.WriteS8(-100);
			msg1.WriteU8(160);
			msg1.WriteS16(-30000);
			msg1.WriteU16(60000);
			msg1.WriteS32(-2000000000);
			msg1.WriteU32(4000000000);
			msg1.WriteS64(-9000000000000000000LL);
			msg1.WriteU64(18000000000000000000ULL);
			msg1.WritePtr(pfsef);
			msg1.WriteFloat(3.141592f);
			msg1.WriteDouble(2.718281828459045);
			msg1.WriteBinary(check, 32);
			msg1.WriteBinary(check, 32);
			msg1.WriteString("abcdefg1");
			msg1.WriteString("abcdefg2");
			msg1.WriteString("abcdefg3");
			msg1.WriteString("abcdefg4");

			auto a = msg1.GetCurrentVT();
			msg1.ReadS8();
			a = msg1.GetCurrentVT();
			msg1.ReadS8();

			a = msg1.ReadAny();

			msg1.SetReadOffset(0);
			a = msg1.ReadAny();


			msg1.SetReadOffset(10);
			_f32 fesf = msg1.ReadFloat();

			msg1.SetReadOffset(8);
			_u64 fsefesf = msg1.ReadU64();

			msg1.SetReadOffset(14);
			String sefse = msg1.ReadString();

			msg1.SetWriteOffset(14);
			msg1.SetReadOffset(14);
			msg1.WriteString("aaa");
			String fsef = msg1.ReadString();
			Console::WriteLine(msg1.Dump().SafeSource());
			delete pfsef;
			delete[] check;

			CDataMap map;
			map.SetS32("intKey", -123);
			map.SetString("abcd", "ffesfes");
			map.SetString("abcd", "ffesfesfsefse");
		}
		

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
