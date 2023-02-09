/*
	작성자 : 윤정도
*/


#include <JCoreTest/CoreTest.h>

#include <JCore/Container/LinkedList.h>




using namespace std;

#if TEST_LinkedListTest == ON

TEST(LinkedListTest, Regular) {
	LeakCheck;

	// PushBackAll 테스트
	{
		LinkedList<int> v;
		LinkedList<int> temp;

		for (int i = 0; i < 1024; i++) {
			temp.PushBack(i);
		}
		
		EXPECT_TRUE(temp.Front() == 0);
		EXPECT_TRUE(temp.Back() == 1023);
		EXPECT_TRUE(temp.Size() == 1024);

		v.PushBackAll(temp);
		EXPECT_TRUE(v.Front() == 0);
		EXPECT_TRUE(v.Back() == 1023);
		EXPECT_TRUE(v.Size() == 1024);
	}


}

TEST(LinkedListTest, Enumerator) {
	LeakCheck;

	// 데이터가 없는 경우
	{
		LinkedList<int> v;
		EXPECT_TRUE(v.Begin()->IsValid());
		EXPECT_TRUE(v.End()->IsValid());
		EXPECT_TRUE(v.Begin()->HasNext() == false);
		EXPECT_TRUE(v.End()->HasNext() == false);
	}

	// 데이터가 한개만 있는 경우
	{
		LinkedList<int> v;
		v.PushBack(1);

		auto begin = v.Begin();
		auto end = v.End();

		EXPECT_TRUE(begin->IsValid());
		EXPECT_TRUE(begin->HasNext());
		EXPECT_TRUE(end->IsValid());
		EXPECT_TRUE(end->HasPrevious());
		
		EXPECT_TRUE(begin->Next());
		EXPECT_TRUE(begin->HasNext() == false);

		// EXPECT_TRUE(end->Previous()); 한번 끝으로 이동하면 이제 더이상 복구불가능
		// EXPECT_TRUE(end->HasPrevious() == false);
	}

	// 데이터 5개
	{
		LinkedList<int> v;
		v.PushBack(1);
		v.PushBack(2);
		v.PushBack(3);
		v.PushBack(4);
		v.PushBack(5);

		auto begin = v.Begin();
		auto end = v.Begin();

		EXPECT_TRUE(begin->IsValid());
		EXPECT_TRUE(begin->HasNext());
		EXPECT_TRUE(end->IsValid());
		EXPECT_TRUE(end->HasNext());

		for (int i = 1; begin->HasNext(); i++) {
			EXPECT_TRUE(begin->Next() == i);
		}

		for (int i = 5; begin->HasPrevious(); i--) {
			EXPECT_TRUE(begin->Previous() == i);
		}
	}
}


TEST(LinkedListTest, ConstructorTest) {
	LeakCheck;

	LinkedList<int> a{ 1, 2, 3};

	EXPECT_TRUE(a.Front() == 1); a.PopFront();
	EXPECT_TRUE(a.Front() == 2); a.PopFront();

	LinkedList<int> b(a);
	EXPECT_TRUE(b.Front() == 3); b.PopFront();
	EXPECT_TRUE(b.Size() == 0);
	EXPECT_TRUE(a.Size() == 1);

	b.PushBack(1);
	b.PushFront(5);

	EXPECT_TRUE(b.Back() == 1);
	EXPECT_TRUE(b.Front() == 5);

	LinkedList<int> c(Move(b));
	EXPECT_TRUE(b.Size() == 0);
	EXPECT_TRUE(c.Back() == 1);
	EXPECT_TRUE(c.Front() == 5);
}



TEST(LinkedListTest, OperatorTest) {
	LeakCheck;

	LinkedList<Model> a{ 1, 2, 3 };

	LinkedList<Model> b{ 6, 5, 4, 3, 2, 1 };

	// 복사 대입
	b = a;

	int i = 1;
	while (!b.IsEmpty()) {
		EXPECT_TRUE(b.Front().a == i);
		b.PopFront();
		i++;
	}

	LinkedList<Model> c{ 1, 2, 3 };
	b.PushBackAll(c);
	EXPECT_TRUE(b.Size() == 3);
	EXPECT_TRUE(b.Back().a == 3);
	EXPECT_TRUE(b.Front().a == 1);

	// 이니셜라이저 복사 대입
	b = { 1, 3, 5, 6 };
	EXPECT_TRUE(b.Size() == 4);
	EXPECT_TRUE(b.Back().a == 6);
	EXPECT_TRUE(b.Front().a == 1);

	LinkedList<Model> d;
	d = Move(b);
	EXPECT_TRUE(d.Size() == 4);
	EXPECT_TRUE(d.Back().a == 6);
	EXPECT_TRUE(d.Front().a == 1);
}

TEST(LinkedListTest, InnerDestructorTest) {
	LeakCheck;

	LinkedList<String> aq;
	bool toggle = false;
	for (int i = 0; i < 1024; i++) {
		if (i && i % 4 == 0) {
			if ((toggle = !toggle))
				aq.Remove(*aq.Extension().First());
			else
				aq.Remove(*aq.Extension().Last());
		} else {
			aq.PushBack("fsefesfesfesf");
		}
	}
}

TEST(LinkedListTest, MemoryPool) {
	MemoryPoolLeakCheck;

	LinkedList<String, DefaultArrayAllocator> l;
	for (int i = 0; i < 100'000; i++) l.PushBack(1);
}

#endif // TEST_LinkedListTest == ON
