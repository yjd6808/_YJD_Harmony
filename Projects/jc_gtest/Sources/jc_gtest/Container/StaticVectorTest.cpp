/*
	작성자 : 윤정도
*/



#include "jc/Container/StaticVector.h"



using namespace std;

#if TEST_StaticVectorTest == ON

TEST(StaticVectorTest, Regular) {
	LeakCheck;

	// Add / Size
	{
		StaticVector<int, 4> v;
		EXPECT_TRUE(v.IsEmpty());
		EXPECT_TRUE(v.Size() == 0);

		EXPECT_TRUE(v.Add(1));
		EXPECT_TRUE(v.Add(2));
		EXPECT_TRUE(v.Add(3));
		EXPECT_TRUE(v.Add(4));
		EXPECT_TRUE(v.IsFull());
		EXPECT_TRUE(v.Size() == 4);

		// 가득 찼을 때 Add 실패
		EXPECT_FALSE(v.Add(5));
		EXPECT_TRUE(v.Size() == 4);
	}

	// operator[] / Source
	{
		StaticVector<int, 4> v;
		v.Add(10);
		v.Add(20);
		v.Add(30);

		EXPECT_TRUE(v[0] == 10);
		EXPECT_TRUE(v[1] == 20);
		EXPECT_TRUE(v[2] == 30);

		int* pSrc = v.Source();
		EXPECT_TRUE(pSrc[0] == 10);
		EXPECT_TRUE(pSrc[1] == 20);
		EXPECT_TRUE(pSrc[2] == 30);
	}

	// RemoveAt / Remove
	{
		StaticVector<int, 10> v;
		for (int i = 0; i < 5; i++)
			v.Add(i);

		v.RemoveAt(1);
		EXPECT_TRUE(v[0] == 0);
		EXPECT_TRUE(v[1] == 2);
		EXPECT_TRUE(v[2] == 3);
		EXPECT_TRUE(v[3] == 4);
		EXPECT_TRUE(v.Size() == 4);

		v.Remove(3);
		EXPECT_TRUE(v[0] == 0);
		EXPECT_TRUE(v[1] == 2);
		EXPECT_TRUE(v[2] == 4);
		EXPECT_TRUE(v.Size() == 3);

		// 범위 벗어난 RemoveAt
		EXPECT_FALSE(v.RemoveAt(5));
		EXPECT_FALSE(v.RemoveAt(-1));

		// 존재하지 않는 값 Remove
		EXPECT_FALSE(v.Remove(999));
	}

	// Clear
	{
		StaticVector<int, 10> v;
		v.Add(1);
		v.Add(2);
		v.Add(3);
		v.Clear();
		EXPECT_TRUE(v.IsEmpty());
		EXPECT_TRUE(v.Size() == 0);
	}

	// Exist / Offset
	{
		StaticVector<int, 20> v;
		for (int i = 0; i < 10; i++)
			v.Add(i * 2);

		for (int i = 0; i < 10; i++)
		{
			EXPECT_TRUE(v.Exist(i * 2));
			EXPECT_TRUE(v.Offset(i * 2) == i);
		}

		EXPECT_FALSE(v.Exist(1));
		EXPECT_FALSE(v.Exist(999));
		EXPECT_TRUE(v.Offset(1) == -1);
	}

	// ExistIf / FindIf
	{
		StaticVector<int, 10> v;
		v.Add(1);
		v.Add(3);
		v.Add(5);
		v.Add(7);

		EXPECT_TRUE(v.ExistIf([](int _val) { return _val == 5; }));
		EXPECT_FALSE(v.ExistIf([](int _val) { return _val == 2; }));

		int* pFound = v.FindIf([](int _val) { return _val > 4; });
		EXPECT_TRUE(pFound != nullptr);
		EXPECT_TRUE(*pFound == 5);

		const StaticVector<int, 10>& cref = v;
		const int* pConstFound = cref.FindIf([](int _val) { return _val > 4; });
		EXPECT_TRUE(pConstFound != nullptr);
		EXPECT_TRUE(*pConstFound == 5);

		int* pNotFound = v.FindIf([](int _val) { return _val > 100; });
		EXPECT_TRUE(pNotFound == nullptr);
	}

	// GetCapacity
	{
		StaticVector<int, 8> v;
		EXPECT_TRUE(v.GetCapacity() == 8);
	}
}

TEST(StaticVectorTest, ConstructorTest) {
	LeakCheck;

	// 이니셜라이저 생성자
	{
		StaticVector<int, 5> v{ 1, 2, 3 };
		EXPECT_TRUE(v.Size() == 3);
		EXPECT_TRUE(v[0] == 1);
		EXPECT_TRUE(v[1] == 2);
		EXPECT_TRUE(v[2] == 3);
	}

	// 복사 생성자
	{
		StaticVector<int, 5> a{ 1, 2, 3 };
		StaticVector<int, 5> b(a);
		EXPECT_TRUE(b.Size() == 3);
		EXPECT_TRUE(b[0] == 1);
		EXPECT_TRUE(b[1] == 2);
		EXPECT_TRUE(b[2] == 3);

		// 독립적인 복사 확인
		a[0] = 999;
		EXPECT_TRUE(b[0] == 1);
	}

	// 이동 생성자
	{
		StaticVector<int, 5> a{ 1, 2, 3 };
		StaticVector<int, 5> b(Move(a));
		EXPECT_TRUE(b.Size() == 3);
		EXPECT_TRUE(b[0] == 1);
		EXPECT_TRUE(b[1] == 2);
		EXPECT_TRUE(b[2] == 3);
		EXPECT_TRUE(a.IsEmpty());
	}
}

TEST(StaticVectorTest, OperatorTest) {
	LeakCheck;

	// 대입 연산자
	{
		StaticVector<int, 5> a{ 1, 2, 3 };
		StaticVector<int, 5> b;

		b = a;
		EXPECT_TRUE(b.Size() == 3);
		EXPECT_TRUE(b[0] == 1);
		EXPECT_TRUE(b[1] == 2);
		EXPECT_TRUE(b[2] == 3);

		a[0] = 999;
		EXPECT_TRUE(b[0] == 1);
	}

	// 이동 대입
	{
		StaticVector<int, 5> a{ 1, 2, 3 };
		StaticVector<int, 5> b;

		b = Move(a);
		EXPECT_TRUE(b.Size() == 3);
		EXPECT_TRUE(b[0] == 1);
		EXPECT_TRUE(a.IsEmpty());
	}

	// 이니셜라이저 대입
	{
		StaticVector<int, 5> v;
		v = { 10, 20, 30 };
		EXPECT_TRUE(v.Size() == 3);
		EXPECT_TRUE(v[0] == 10);
		EXPECT_TRUE(v[1] == 20);
		EXPECT_TRUE(v[2] == 30);
	}
}

TEST(StaticVectorTest, ObjectTest) {
	LeakCheck;

	// 일반 객체 (non-pointer) 테스트
	{
		StaticVector<Model, 5> v;
		v.Add(Model(1));
		v.Add(Model(2));
		v.Add(Model(3));

		EXPECT_TRUE(v[0].a == 1);
		EXPECT_TRUE(v[1].a == 2);
		EXPECT_TRUE(v[2].a == 3);

		v.RemoveAt(1);
		EXPECT_TRUE(v[0].a == 1);
		EXPECT_TRUE(v[1].a == 3);
		EXPECT_TRUE(v.Size() == 2);

		v.Clear();
		EXPECT_TRUE(v.IsEmpty());
	}

	// 포인터 타입 테스트
	{
		StaticVector<Model*, 5> v;
		Model* p1 = new Model(10);
		Model* p2 = new Model(20);

		v.Add(p1);
		v.Add(p2);
		EXPECT_TRUE(v.Size() == 2);
		EXPECT_TRUE(v[0]->a == 10);
		EXPECT_TRUE(v[1]->a == 20);

		v.RemoveAt(0);
		EXPECT_TRUE(v[0]->a == 20);
		EXPECT_TRUE(v.Size() == 1);

		delete p1;
		delete p2;
	}
}

TEST(StaticVectorTest, Iterator) {
	LeakCheck;

	// 범위 기반 for
	{
		StaticVector<int, 5> v{ 1, 2, 3 };

		int sum = 0;
		for (int val : v)
			sum += val;
		EXPECT_TRUE(sum == 6);
	}

	// const 범위 기반 for
	{
		const StaticVector<int, 5> v{ 1, 2, 3 };

		int sum = 0;
		for (int val : v)
			sum += val;
		EXPECT_TRUE(sum == 6);
	}

	// 빈 컨테이너 iterate
	{
		StaticVector<int, 5> v;
		int count = 0;
		for (int val : v)
			count++;
		EXPECT_TRUE(count == 0);
	}
}

TEST(StaticVectorTest, EdgeCase) {
	LeakCheck;

	// 용량 0
	{
		StaticVector<int, 0> v;
		EXPECT_TRUE(v.IsEmpty());
		EXPECT_TRUE(v.IsFull());
		EXPECT_TRUE(v.GetCapacity() == 0);
		EXPECT_FALSE(v.Add(1));
	}

	// 용량 1
	{
		StaticVector<int, 1> v;
		EXPECT_TRUE(v.Add(1));
		EXPECT_TRUE(v.IsFull());
		EXPECT_FALSE(v.Add(2));

		v.RemoveAt(0);
		EXPECT_TRUE(v.IsEmpty());
		EXPECT_TRUE(v.Add(2));
	}

	// RemoveAt 마지막 요소
	{
		StaticVector<int, 5> v{ 1, 2, 3 };
		v.RemoveAt(2);
		EXPECT_TRUE(v.Size() == 2);
		EXPECT_TRUE(v[0] == 1);
		EXPECT_TRUE(v[1] == 2);
	}

	// RemoveAt 첫 요소
	{
		StaticVector<int, 5> v{ 1, 2, 3 };
		v.RemoveAt(0);
		EXPECT_TRUE(v.Size() == 2);
		EXPECT_TRUE(v[0] == 2);
		EXPECT_TRUE(v[1] == 3);
	}
}

#endif // TEST_StaticVectorTest == ON
