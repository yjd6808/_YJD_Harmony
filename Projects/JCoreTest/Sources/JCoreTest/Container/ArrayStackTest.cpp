/*
	�ۼ��� : ������
	�迭��� ���� �׽�Ʈ
*/


#include <JCoreTest/CoreTest.h>
#include <JCore/Random.h>
#include <JCore/Container/ArrayStack.h>

using namespace JCore;
using namespace std;

#if TEST_ArrayStackTest == ON

TEST(ArrayStackTest, Regular) {
	MemoryLeakDetector detector;
	ArrayStack<int> stack;
	int count = 320;

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

// �߰�, ����, �ݺ��� �׽�Ʈ
TEST(ArrayStackTest, TotalTest) {
	Random rand;

	int testCount = 50;
	while (testCount--) {
		int dc = rand.GenerateInt(0, 100) + 1;
		MemoryLeakDetector detector;
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

// ������ �׽�Ʈ
TEST(ArrayStackTest, ConstructorTest) {
	MemoryLeakDetector detector;

	// �̴ϼȶ����� �׽�Ʈ
	ArrayStack<int> a{ 1, 2, 3 };

	EXPECT_TRUE(a.Top() == 3); a.Pop();
	EXPECT_TRUE(a.Top() == 2); a.Pop();

	// ���� ������ �׽�Ʈ
	ArrayStack<int> b(a);
	for (int i = 0; i < 30; i++) {
		b.Push(i);
	}

	ArrayStack<int> c(a);
	EXPECT_TRUE(b.Top() == 29); b.Pop();
	EXPECT_TRUE(b.Top() == 28); b.Pop();
	EXPECT_TRUE(b.Size() == 29);
	EXPECT_TRUE(c.Size() == 1);

	// �̵� ������ �׽�Ʈ
	ArrayStack<int> d(Move(c));
	EXPECT_TRUE(c.Size() == 0);
	EXPECT_TRUE(d.Size() == 1);
	EXPECT_TRUE(d.Top() == 1);
}

// ������ �׽�Ʈ
TEST(ArrayStackTest, OperatorTest) {
	MemoryLeakDetector detector;

	ArrayStack<int> s{ 1, 2, 3 };

	// �̴ϼȶ����� ����
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

#endif // TEST_ArrayStackTest == ON

