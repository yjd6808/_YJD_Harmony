/*
	작성자 : 윤정도
*/


#include <JCoreTest/CoreTest.h>
#include <JCoreTest/TestUtil/Object.h>
#include <JCore/Container/LinkedList.h>



using namespace JCore;
using namespace std;

#if TEST_LinkedListTest == ON

TEST(LinkedListTest, Regular) {
	MemoryLeakDetector detector;

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
	MemoryLeakDetector detector;

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

		EXPECT_TRUE(end->Previous());
		EXPECT_TRUE(end->HasPrevious() == false);
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


#endif // TEST_LinkedListTest == ON
