/*
	작성자 : 윤정도
*/


#include <JCoreTest/CoreTest.h>
#include <JCoreTest/TestUtil/Object.h>
#include <JCore/Container/Vector.h>



using namespace JCore;
using namespace std;

#if TEST_VectorTest == ON

TEST(VectorTest, Regular) {
	MemoryLeakDetector detector;

	// PushBackAll 테스트
	{
		Vector<int> v;
		Vector<int> temp;

		for (int i = 0; i < 1024; i++) {
			temp.PushBack(i);
		}
		
		v.PushBackAll(temp);
		
		for (int i = 0; i < 1024; i++) {
			EXPECT_TRUE(v[i] == i);
		}
	}

	// Insert
	{
		Vector<int> v;
		v.Insert(0, 1);
		v.Insert(1, 2);

		EXPECT_TRUE(v[0] == 1);
		EXPECT_TRUE(v[1] == 2);

		v.Insert(0, 3);
		EXPECT_TRUE(v[0] == 3);
		EXPECT_TRUE(v[1] == 1);
		EXPECT_TRUE(v[2] == 2);
	}

	// EmplaceBack
	{
		Vector<Model> v;
		v.EmplaceBack(1);
		v.EmplaceBack(2);

		EXPECT_TRUE(v[0].a == 1);
		EXPECT_TRUE(v[1].a == 2);
	}

	// EmplaceInsert
	{
		Vector<Model> v;
		v.EmplaceInsert(0, 1);
		v.EmplaceInsert(0, 2);
		v.EmplaceInsert(0, 3);

		EXPECT_TRUE(v[0].a == 3);
		EXPECT_TRUE(v[1].a == 2);
		EXPECT_TRUE(v[2].a == 1);
	}

	// InsertAll
	// RemoveAt
	{
		Vector<int> v;
		v.PushBack(0);
		v.PushBack(0);
		v.PushBack(0);
		v.PushBack(0);

		Vector<int> temp;
		for (int i = 0; i < 3; i++) {
			temp.PushBack(i);
		}

		v.InsertAll(0, temp);

		for (int i = 0; i < 3; i++) {
			EXPECT_TRUE(v[i] == i);
		}

		for (int i = 0; i < 3; i++) {
			v.RemoveAt(0);
		}

		for (int i = 0; i < 4; i++) {
			EXPECT_TRUE(v[i] == 0);
		}

		v.InsertAll(1, temp);
		EXPECT_TRUE(v[0] == 0);
		for (int i = 1; i < 4; i++) {
			EXPECT_TRUE(v[i] == i - 1);
		}

		for (int i = 0; i < 3; i++) {
			v.RemoveAt(1);
		}

		for (int i = 0; i < 4; i++) {
			EXPECT_TRUE(v[i] == 0);
		}

		v.InsertAll(2, temp);
		EXPECT_TRUE(v[0] == 0);
		EXPECT_TRUE(v[1] == 0);
		for (int i = 2; i < 5; i++) {
			EXPECT_TRUE(v[i] == i - 2);
		}

		for (int i = 0; i < 3; i++) {
			v.RemoveAt(2);
		}

		for (int i = 0; i < 4; i++) {
			EXPECT_TRUE(v[i] == 0);
		}

		v.InsertAll(3, temp);
		EXPECT_TRUE(v[0] == 0);
		EXPECT_TRUE(v[1] == 0);
		EXPECT_TRUE(v[2] == 0);
		for (int i = 3; i < 6; i++) {
			EXPECT_TRUE(v[i] == i - 3);
		}

		for (int i = 0; i < 3; i++) {
			v.RemoveAt(3);
		}
	}



	// Remove / Find / Offset
	{
		Vector<int> v;
		for (int i = 0; i < 20; i++) {
			v.PushBack(i);
		}

		for (int i = 0; i < 20; i++) {
			EXPECT_TRUE(v.Find(i));
			EXPECT_TRUE(v.Offset(i) == i);
		}

		for (int i = 0; i < 20; i++) {
			EXPECT_TRUE(v.FindBinary(i));
			EXPECT_TRUE(v.OffsetBinary(i) == i);
		}

		for (int i = 0; i < 20; i++) {
			v.RemoveBinary(i);
		}

		EXPECT_TRUE(v.Size() == 0);
	}
}

TEST(VectorTest, Enumerator) {
	MemoryLeakDetector detector;

	// 데이터가 없는 경우
	{
		Vector<int> v;
		EXPECT_TRUE(v.Begin()->IsValid());
		EXPECT_TRUE(v.End()->IsValid());
		EXPECT_TRUE(v.Begin()->HasNext() == false);
		EXPECT_TRUE(v.End()->HasNext() == false);
	}

	// 데이터가 한개만 있는 경우
	{
		Vector<int> v;
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
		Vector<int> v;
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


#endif // TEST_VectorTest == ON
