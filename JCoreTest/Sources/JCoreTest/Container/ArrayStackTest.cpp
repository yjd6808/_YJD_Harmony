/*
	작성자 : 윤정도
	배열기반 스택 테스트
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

	EXPECT_TRUE(stack.GetSize() == 0);

	for (int i = 0; i < count; i++) {
		stack.Push(i);
	}

	stack.Clear();
	EXPECT_TRUE(stack.GetSize() == 0);
}

// 추가, 삭제, 반복자 테스트
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

		while (stackFit->HasValue()) {
			EXPECT_TRUE(stackFit->Next() == (*fit));
			fit++;
		}

		while (stackFit->HasValue()) {
			EXPECT_TRUE(stackFit->Previous() == (*fit));
			rit++;
		}

		EXPECT_TRUE(stack.GetSize() == dc);
		EXPECT_TRUE(stack.GetSize() == stl.size());

		for (int i = dc - 1; i >= 0; i--) {
			EXPECT_TRUE(stack.Top() == stl.top());
			stack.Pop();
			stl.pop();
		}

		EXPECT_TRUE(stack.GetSize() == 0);
		EXPECT_TRUE(stl.size() == 0);

		for (int i = 0; i < dc; i++) {
			stack.Push(i);
			stl.push(i);
		}

		EXPECT_TRUE(stack.GetSize() == dc);

		stack.Clear();

		EXPECT_TRUE(stack.GetSize() == 0);
	}
}



#endif // TEST_ArrayStackTest == ON


