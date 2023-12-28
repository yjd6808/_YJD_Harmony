/*
	작성자 : 윤정도
*/


#include <JCoreTest/CoreTest.h>

#include <JCore/Container/Vector.h>




using namespace std;

#if TEST_VectorTest == ON

TEST(VectorTest, Regular) {
	LeakCheck;

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



	// Remove / Exist / Offset
	{
		Vector<int> v;
		for (int i = 0; i < 20; i++) {
			v.PushBack(i);
		}

		for (int i = 0; i < 20; i++) {
			EXPECT_TRUE(v.Exist(i));
			EXPECT_TRUE(v.Offset(i) == i);
		}

		for (int i = 0; i < 20; i++) {
			EXPECT_TRUE(v.ExistBinary(i));
			EXPECT_TRUE(v.OffsetBinary(i) == i);
		}

		for (int i = 0; i < 20; i++) {
			v.RemoveBinary(i);
		}

		EXPECT_TRUE(v.Size() == 0);
	}
}

TEST(VectorTest, Enumerator) {
	LeakCheck;

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

// 생성자 테스트
TEST(VectorTest, ConstructorTest) {
	LeakCheck;

	// 이니셜라이저 테스트
	Vector<int> a{ 1, 2, 3 };

	EXPECT_TRUE(a[0] == 1); a.Remove(1);
	EXPECT_TRUE(a[0] == 2); a.Remove(2);

	// 복사 생성자 테스트
	Vector<int> b(a);
	for (int i = 0; i < 30; i++) {
		b.PushBack(i);
	}

	Vector<int> c(a);
	int lastIdx = b.Size() - 1;

	EXPECT_TRUE(b[lastIdx] == 29);		b.RemoveAt(lastIdx);
	EXPECT_TRUE(b[lastIdx - 1] == 28);	b.RemoveAt(lastIdx - 1);
	EXPECT_TRUE(b[0] == 3);
	EXPECT_TRUE(b.Size() == 29);
	EXPECT_TRUE(c.Size() == 1);
	EXPECT_TRUE(c[0] == 3);

	// 이동 생성자 테스트
	Vector<int> d(Move(c));
	EXPECT_TRUE(c.Size() == 0);
	EXPECT_TRUE(d.Size() == 1);
	EXPECT_TRUE(d[0] == 3);
}

// 연산자 테스트
TEST(VectorTest, OperatorTest) {
	LeakCheck;

	Vector<int> s{ 1, 2, 3 };

	// 이니셜라이저 대입
	s = { 1 };
	EXPECT_TRUE(s[0] == 1); s.RemoveAt(0);

	Vector<int> t{ 1, 2, 3, 4, 5 };
	s = t;

	EXPECT_TRUE(s.Size() == 5);
	EXPECT_TRUE(s[4] == 5); s.RemoveAt(4);
	EXPECT_TRUE(s[3] == 4); s.RemoveAt(3);
	EXPECT_TRUE(s[2] == 3); s.RemoveAt(2);
	EXPECT_TRUE(s[1] == 2); s.RemoveAt(1);
	EXPECT_TRUE(s[0] == 1); s.RemoveAt(0);
	EXPECT_TRUE(s.Size() == 0);

	s.PushBack(5);
	s.PushBack(4);
	EXPECT_TRUE(s[1] == 4);
	s.Clear();
	EXPECT_TRUE(s.Size() == 0);

	s = Move(t);
	EXPECT_TRUE(t.IsEmpty());
	EXPECT_TRUE(s.Size() == 5);
	EXPECT_TRUE(s[4] == 5); s.RemoveAt(4);
	EXPECT_TRUE(s[3] == 4); s.RemoveAt(3);
	EXPECT_TRUE(s[2] == 3); s.RemoveAt(2);
	EXPECT_TRUE(s[1] == 2); s.RemoveAt(1);
	EXPECT_TRUE(s[0] == 1); s.RemoveAt(0);
	EXPECT_TRUE(s.Size() == 0);
}

TEST(VectorTest, MemoryPool) {
	MemoryPoolLeakCheck;

	Vector<String, DefaultArrayAllocator> l;
	for (int i = 0; i < 100'000; i++) l.PushBack(1);
}

// 생성자 테스트
TEST(VectorTest, SizeInit) {
	LeakCheck;
	Vector<String> v(30, "a" );

	for (int i = 0; i < 30; ++i) {
		EXPECT_TRUE(v[i] == "a");
	}
}

TEST(VectorTest, BinarySearch) {
	Vector<Model> vModels;

	for (int i = 0; i <= 200; ++i) {
		vModels.PushBack({ 200 - i, i });
	}

	for (int i = 0; i <= 200; i++) {
		int ioffset = vModels.OffsetLowerBound(i, [](const Model& m, const int& v) { return m.b < v; });
		EXPECT_TRUE(vModels[i].b == i);
	}
}

TEST(VectorTest, ZeroCapacityTest) {
	LeakCheck;
	Vector<int> vector1(0);
	Vector<int> vector2(vector1);
	Vector<int> _(vector1);
	Vector<int> vector3(Move(_));

	EXPECT_EQ(vector1.Capacity(), 0);
	EXPECT_EQ(vector2.Capacity(), 0);
	EXPECT_EQ(vector3.Capacity(), 0);

	vector1.PushBack(1);
	vector2.PushBack(1);
	vector3.PushBack(1);

	EXPECT_TRUE(vector1.Capacity() > 0);
	EXPECT_TRUE(vector2.Capacity() > 0);
	EXPECT_TRUE(vector3.Capacity() > 0);
}

TEST(VectorTest, Pointer) {
	{
		LeakCheck;
		Vector vector({ new int, new int });

		delete vector[0];
		delete vector[1];
	}

	{
		LeakCheck;
		Vector<int*> vector(1);
		vector.PushBack(new int);
		vector.PushBack(new int);
		vector.PushBack(new int);
		vector.PushBack(new int);
		vector.ForEachDelete();
	}

}

TEST(VectorTest, Shrink) {
	LeakCheck;

	Vector v{ 1, 2, 3, 4, 5 };
	v.Shrink(3);
	EXPECT_TRUE(v[0] == 1);
	EXPECT_TRUE(v[1] == 2);
	EXPECT_TRUE(v[2] == 3);
	EXPECT_TRUE(v.Capacity() == 3);
	EXPECT_TRUE(v.Size() == 3);

	v.Shrink(6);
	EXPECT_TRUE(v.Capacity() == 3);
	EXPECT_TRUE(v.Size() == 3);

	v.Shrink(0);
	EXPECT_TRUE(v.Capacity() == 0);
	EXPECT_TRUE(v.Size() == 0);
	EXPECT_TRUE(v.Source() == nullptr);


	v.Reserve(20);
	EXPECT_TRUE(v.Size() == 0);
	EXPECT_TRUE(v.Capacity() == 20);
	EXPECT_TRUE(v.Source() != nullptr);

	for (int i = 0; i < 10; ++i) {
		v.PushBack(i);
	}
	EXPECT_TRUE(v.Size() == 10);
	EXPECT_TRUE(v.Capacity() == 20);
	v.ShrinkToFit();
	EXPECT_TRUE(v.Capacity() == 10);
}

TEST(VectorTest, Pop) {
	Vector v{ 1, 2, 3 };
	v.PopFront();
	EXPECT_TRUE(v[0] == 2);
	EXPECT_TRUE(v[1] == 3);
	EXPECT_TRUE(v.Size() == 2);

	v.PopFront();
	EXPECT_TRUE(v[0] == 3);
	EXPECT_TRUE(v.Size() == 1);

	v.PopFront();
	EXPECT_TRUE(v.Size() == 0);

	v.PushBack(1);
	EXPECT_TRUE(v[0] == 1);
	EXPECT_TRUE(v.Size() == 1);

	v = { 1, 2, 3 };
	v.PopFront(3);
	EXPECT_TRUE(v.Size() == 0);

	v = { 1, 2, 3 };
	v.PopFront(2);
	EXPECT_TRUE(v[0] == 3);
	EXPECT_TRUE(v.Size() == 1);
}

#endif // TEST_VectorTest == ON
