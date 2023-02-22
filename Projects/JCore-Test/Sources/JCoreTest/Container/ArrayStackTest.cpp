/*
	작성자 : 윤정도
	배열기반 스택 테스트
*/


#include <JCoreTest/CoreTest.h>
#include <JCore/Random.h>
#include <JCore/Container/ArrayStack.h>


using namespace std;

#if TEST_ArrayStackTest == ON

TEST(ArrayStackTest, Regular) {
	LeakCheck;
	ArrayStack<int> stack;
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
TEST(ArrayStackTest, TotalTest) {
	Random rand;

	int testCount = 50;
	while (testCount--) {
		int dc = rand.GenerateInt(0, 100) + 1;
		LeakCheck;
		ArrayStack<int> stack;
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
			++fit;
		}

		while (stackFit->HasNext()) {
			EXPECT_TRUE(stackFit->Previous() == (*fit));
			++rit;
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

// 생성자 테스트
TEST(ArrayStackTest, ConstructorTest) {
	LeakCheck;

	// 이니셜라이저 테스트
	ArrayStack<int> a{ 1, 2, 3 };

	EXPECT_TRUE(a.Top() == 3); a.Pop();
	EXPECT_TRUE(a.Top() == 2); a.Pop();

	// 복사 생성자 테스트
	ArrayStack<int> b(a);
	for (int i = 0; i < 30; i++) {
		b.Push(i);
	}

	ArrayStack<int> c(a);
	EXPECT_TRUE(b.Top() == 29); b.Pop();
	EXPECT_TRUE(b.Top() == 28); b.Pop();
	EXPECT_TRUE(b.Size() == 29);
	EXPECT_TRUE(c.Size() == 1);

	// 이동 생성자 테스트
	ArrayStack<int> d(Move(c));
	EXPECT_TRUE(c.Size() == 0);
	EXPECT_TRUE(d.Size() == 1);
	EXPECT_TRUE(d.Top() == 1);
}

// 연산자 테스트
TEST(ArrayStackTest, OperatorTest) {
	LeakCheck;

	ArrayStack<int> s{ 1, 2, 3 };

	// 이니셜라이저 대입
	s = { 1 };
	EXPECT_TRUE(s.Top() == 1); s.Pop();

	ArrayStack<int> t{ 1, 2, 3, 4, 5 };
	s = t;

	EXPECT_TRUE(s.Size() == 5);
	EXPECT_TRUE(s.Top() == 5); s.Pop();
	EXPECT_TRUE(s.Top() == 4); s.Pop();
	EXPECT_TRUE(s.Top() == 3); s.Pop();
	EXPECT_TRUE(s.Top() == 2); s.Pop();
	EXPECT_TRUE(s.Top() == 1); s.Pop();
	EXPECT_TRUE(s.Size() == 0); 

	s.Push(5);
	s.Push(4);
	EXPECT_TRUE(s.Top() == 4);
	s.Clear();
	EXPECT_TRUE(s.Size() == 0);

	s = Move(t);
	EXPECT_TRUE(t.IsEmpty());
	EXPECT_TRUE(s.Size() == 5);
	EXPECT_TRUE(s.Top() == 5); s.Pop();
	EXPECT_TRUE(s.Top() == 4); s.Pop();
	EXPECT_TRUE(s.Top() == 3); s.Pop();
	EXPECT_TRUE(s.Top() == 2); s.Pop();
	EXPECT_TRUE(s.Top() == 1); s.Pop();
	EXPECT_TRUE(s.Size() == 0);
}



TEST(ArrayStackTest, InnerDestructorTest) {
	LeakCheck;

	ArrayStack<String> aq;
	for (int i = 0; i < 1024; i++) {
		if (i && i % 4 == 0) {
			aq.Pop();
		} else {
			aq.Push("ab");
		}
	}
}


TEST(ArrayStackTest, MemoryPool) {
	MemoryPoolLeakCheck;

	ArrayStack<String, DefaultArrayAllocator> q;
	for (int i = 0; i < 100'000; i++) q.Push("1");
}

#endif // TEST_ArrayStackTest == ON


