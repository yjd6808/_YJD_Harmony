/*
	작성자 : 윤정도
	배열기반 스택 테스트
*/


#include <JCoreTest/CoreTest.h>
#include <JCoreTest/TestUtil/Object.h>

#include <JCore/Random.h>
#include <JCore/Container/ListStack.h>

using namespace JCore;
using namespace std;

#if TEST_ListStackTest == ON

TEST(ListStackTest, Regular) {
	MemoryLeakDetector detector;
	ListStack<int> stack;
	const int count = 320;

	for (int i = 0; i < count; i++) {
		stack.Push(i);
	}

	for (int i = count - 1; i >= 0; i--) {
		int a = stack.Top();
		EXPECT_TRUE(stack.Top() == i);
		stack.Pop();
	}

	EXPECT_TRUE(stack.Size() == 0);

	for (int i = 0; i < count; i++) {
		stack.Push(i);
	}

	stack.Clear();
	EXPECT_TRUE(stack.Size() == 0);
}

// 추가, 삭제, 반복자 테스트
TEST(ListStackTest, TotalTest) {
	Random rand;

	int testCount = 50;
	while (testCount--) {
		int dc = rand.GenerateInt(0, 100) + 1;
		MemoryLeakDetector detector;
		ListStack<int> stack;
		std::stack<int> stl;

		list<int> flist;
		list<int> rlist;

		for (int i = 0; i < dc; i++) {
			stack.Push(i);
			stl.push(i);
			flist.push_back(i);
			rlist.push_front(i);
		}

		auto fit = flist.begin();
		auto rit = rlist.begin();

		auto stackFit = stack.Begin();
		auto stackRit = stack.End();

		while (stackFit->HasNext()) {
			EXPECT_TRUE(stackFit->Next() == (*fit));
			fit++;
		}

		while (stackFit->HasNext()) {
			EXPECT_TRUE(stackFit->Previous() == (*fit));
			rit++;
		}

		EXPECT_TRUE(stack.Size() == dc);
		EXPECT_TRUE(stack.Size() == stl.size());

		for (int i = dc - 1; i >= 0; i--) {
			EXPECT_TRUE(stack.Top() == stl.top());
			stack.Pop();
			stl.pop();
		}

		EXPECT_TRUE(stack.Size() == 0);
		EXPECT_TRUE(stl.size() == 0);

		for (int i = 0; i < dc; i++) {
			stack.Push(i);
			stl.push(i);
		}

		EXPECT_TRUE(stack.Size() == dc);

		stack.Clear();

		EXPECT_TRUE(stack.Size() == 0);
	}
}

TEST(ListStackTest, ConstructorTest) {
	MemoryLeakDetector detector;

	ListStack<int> a{ 1, 2, 3 };

	EXPECT_TRUE(a.Top() == 3); a.Pop();
	EXPECT_TRUE(a.Top() == 2); a.Pop();

	ListStack<int> b(a);
	EXPECT_TRUE(b.Top() == 1); b.Pop();
	EXPECT_TRUE(b.Size() == 0);
	EXPECT_TRUE(a.Size() == 1);

	b.Push(3);
	b.Push(2);

	EXPECT_TRUE(b.Top() == 2);
	EXPECT_TRUE(b.Size() == 2);

	ListStack<int> c(Move(b));
	EXPECT_TRUE(b.Size() == 0);
	EXPECT_TRUE(c.Top() == 2); c.Pop();
	EXPECT_TRUE(c.Top() == 3); c.Pop();
	EXPECT_TRUE(c.Size() == 0);
}



TEST(ListStackTest, OperatorTest) {
	MemoryLeakDetector detector;

	ListStack<Model> a{ 1, 2, 3 };

	ListStack<Model> b{ 6, 5, 4, 3, 2, 1 };

	// 복사 대입
	b = a;

	int i = 3;
	while (!b.IsEmpty()) {
		EXPECT_TRUE(b.Top().a == i);
		b.Pop();
		i--;
	}

	ListStack<Model> c{ 1, 2, 3 };
	b.PushAll(c);
	EXPECT_TRUE(b.Size() == 3);
	EXPECT_TRUE(b.Top().a == 3); b.Pop();
	EXPECT_TRUE(b.Top().a == 2); b.Pop();
	EXPECT_TRUE(b.Size() == 1);


	// 이니셜라이저 복사 대입
	b = { 1, 3, 5, 6 };
	EXPECT_TRUE(b.Size() == 4);
	EXPECT_TRUE(b.Top().a == 6); b.Pop();
	EXPECT_TRUE(b.Top().a == 5); b.Pop();
	EXPECT_TRUE(b.Size() == 2);

	ListStack<Model> d;
	d = Move(b);
	EXPECT_TRUE(b.Size() == 0);
	EXPECT_TRUE(d.Size() == 2);

	auto it = d.Begin();
	EXPECT_TRUE(it->Next().a == 1);
	EXPECT_TRUE(it->Next().a == 3);
	EXPECT_TRUE(it->HasNext() == false);
}


#endif // TEST_ListStackTest == ON


