/*
 * 작성자: 윤정도
 * 생성일: 8/15/2023 7:40:18 AM
 * =====================
 * TreeSet 클래스를 테스트합니다.
 */


#include "jc/Container/TreeSet.h"
#include "jc/Random.h"

USING_NS_STD;

#if TEST_TreeSetTest == ON

TEST(TreeSetTest, Regular) {
	LeakCheck;
	TreeSet<int> testSet;

	// Insert 테스트
	// Key, Value 개별삽입
	for (int i = 0; i < 100; ++i) {
		EXPECT_TRUE(testSet.Insert(i));
	}
	for (int i = 0; i < 100; ++i) {
		EXPECT_FALSE(testSet.Insert(i));
	}
	EXPECT_TRUE(testSet.Size() == 100);

	// Remove 테스트
	for (int i = 0; i < 100; ++i) {
		EXPECT_TRUE(testSet.Remove(i));
	}
	EXPECT_TRUE(testSet.Size() == 0);
	EXPECT_TRUE(testSet.IsEmpty());

	// Exist 테스트
	for (int i = 0; i < 100; ++i) {
		EXPECT_TRUE(testSet.Insert(i));
	}
	for (int i = 0; i < 100; ++i) {
		EXPECT_TRUE(testSet.Exist(i));
	}

	// Clear 테스트
	testSet.Clear();
	EXPECT_TRUE(testSet.Size() == 0);
	EXPECT_TRUE(testSet.Insert(2));	// 삭제 후 데이터 정상 삽입 되는지
	EXPECT_TRUE(testSet.Insert(3));
	EXPECT_TRUE(testSet.Insert(4));
	EXPECT_TRUE(testSet.Size() == 3);
}

TEST(TreeSetTest, EnumeratorTest) {
	TreeSet<int> testSet;
	for (int i = 0; i < 10; i++) {
		testSet.Insert(i);
	}

	// 정방향 반복자 테스트
	auto mapIter = testSet.Begin();
	for (int i = 0; mapIter.HasNext(); i++) {
		int& key = mapIter.Next();
		EXPECT_TRUE(testSet.Exist(key));
		EXPECT_TRUE(testSet.Exist(i));
	}

	// 반대방향 반복자 테스트
	auto mapRIter = testSet.End();
	for (int i = 9; mapRIter.HasPrevious(); i--) {
		int& key = mapRIter.Previous();
		EXPECT_TRUE(testSet.Exist(key));
		EXPECT_TRUE(testSet.Exist(key));
	}
}

TEST(TreeSetTest, ConstructorTest) {
	// 유니폼 이니셜라이저 생성자
	TreeSet testSet{ 1, 2, 3, 4 };

	EXPECT_TRUE(testSet.Exist(1));
	EXPECT_TRUE(testSet.Exist(2));
	EXPECT_TRUE(testSet.Exist(3));
	EXPECT_TRUE(testSet.Exist(4));
	EXPECT_TRUE(testSet.Size() == 4);

	// 복사 생성자
	TreeSet copy1(testSet);
	EXPECT_TRUE(copy1.Exist(1));
	EXPECT_TRUE(copy1.Exist(2));
	EXPECT_TRUE(copy1.Exist(3));
	EXPECT_TRUE(copy1.Exist(4));
	EXPECT_TRUE(copy1.Size() == 4);

	// 이동 생성자
	TreeSet move1(Move(testSet));
	EXPECT_TRUE(testSet.Size() == 0);
	EXPECT_TRUE(testSet.IsEmpty());
	EXPECT_TRUE(copy1.Exist(1));
	EXPECT_TRUE(copy1.Exist(2));
	EXPECT_TRUE(copy1.Exist(3));
	EXPECT_TRUE(copy1.Exist(4));
	EXPECT_TRUE(copy1.Size() == 4);
}

TEST(TreeSetTest, OperatorTest) {
	TreeSet testSet{ 1 };

	// 유니폼 이니셜라이저 대입 연산자
	testSet = { 1, 2, 3, 4 };

	EXPECT_TRUE(testSet.Exist(1));
	EXPECT_TRUE(testSet.Exist(2));
	EXPECT_TRUE(testSet.Exist(3));
	EXPECT_TRUE(testSet.Exist(4));
	EXPECT_TRUE(testSet.Size() == 4);

	// 복사 생성자
	TreeSet copy1{ {1} };

	copy1 = testSet;
	EXPECT_TRUE(copy1.Exist(1));
	EXPECT_TRUE(copy1.Exist(2));
	EXPECT_TRUE(copy1.Exist(3));
	EXPECT_TRUE(copy1.Exist(4));
	EXPECT_TRUE(copy1.Size() == 4);

	// 이동 생성자
	TreeSet move1{ {1} };

	move1 = Move(testSet);
	EXPECT_TRUE(testSet.Size() == 0);
	EXPECT_TRUE(testSet.IsEmpty());
	EXPECT_TRUE(move1.Exist(1));
	EXPECT_TRUE(move1.Exist(2));
	EXPECT_TRUE(move1.Exist(3));
	EXPECT_TRUE(move1.Exist(4));
	EXPECT_TRUE(move1.Size() == 4);
}

TEST(TreeSetTest, NonFundamental) {
	LeakCheck;

	TreeSet<String> testSet;
	EXPECT_TRUE(testSet.Insert("abcd"));
	EXPECT_TRUE(testSet.Insert("abce"));

	EXPECT_TRUE(testSet.Size() == 2);

	EXPECT_TRUE(testSet.Exist("abcd"));
	EXPECT_TRUE(testSet.Exist("abce"));

	EXPECT_TRUE(testSet.Remove("abcd"));
	EXPECT_TRUE(testSet.Remove("abce"));

	EXPECT_TRUE(testSet.Size() == 0);

	testSet = { "abcd", "abcde", "abcdef" };
	EXPECT_TRUE(testSet.Exist("abcd"));
	EXPECT_TRUE(testSet.Exist("abcde"));
	EXPECT_TRUE(testSet.Exist("abcdef"));
	testSet.Clear();
	EXPECT_TRUE(testSet.Size() == 0);
}


TEST(TreeSetTest, MemoryPool) {
	MemoryPoolLeakCheck
	TreeSet<String, Comparator<String>, CDefaultArrayAllocator> q;
	for (int i = 0; i < 1'000; i++) {
		q.Insert(StringUtil::Format("ss%d", i));
	}
}

TEST(TreeSetTest, Sort) {
	LeakCheck;

	Random r(true);
	TreeSet<int> testSet;
	bool check[100]{};		// 현재 set가 unique여부를 보장하지 않는다는 가정하에 많든 채
	for (int i = 0; i < 100; ++i) {
		while (true) {
			int v = r.GenerateInt(0, 100);
			if (!check[v]) {
				check[v] = true;
				testSet.Insert(v);
			}

			bool bAllChecked = true;
			for (int j = 0; j < 100; ++j) {
				if (!check[j]) {
					bAllChecked = false;
					break;
				}
			}
			if (bAllChecked) break;
		}
	}

	auto it = testSet.Begin();
	for (int i = 0; i < 100; ++i) {
		int v = it.Current();

		if (it.HasNext())
			it.Next();
		EXPECT_TRUE(v == i);
	}

}

TEST(TreeSetTest, Pointer) {
	LeakCheck;	// 릭이 없어야함.
	int* ptr = new int;
	TreeSet<int*> testSet1;
	EXPECT_TRUE(testSet1.Insert(ptr));
	EXPECT_FALSE(testSet1.Insert(ptr));

	testSet1.Insert(new int);
	testSet1.Insert(new int);
	testSet1.Insert(new int);
	testSet1.Insert(new int);
	testSet1.ForEachDelete();
}

TEST(TreeSetTest, TryPop) {
	LeakCheck;
	TreeSet<int> testSet;

	testSet.Insert(1);
	testSet.Insert(2);
	testSet.Insert(3);
	EXPECT_EQ(testSet.Size(), 3);

	// 존재하는 키로 TryPop - 성공해야함
	int key = 0;
	EXPECT_TRUE(testSet.TryPop(1, &key));
	EXPECT_EQ(key, 1);
	EXPECT_EQ(testSet.Size(), 2);
	EXPECT_FALSE(testSet.Exist(1));

	// 이미 삭제된 키로 TryPop - 실패해야함
	EXPECT_FALSE(testSet.TryPop(1, &key));

	// 존재하지 않는 키로 TryPop - 실패해야함
	EXPECT_FALSE(testSet.TryPop(99, &key));

	// _pOut이 nullptr인 경우에도 정상 삭제
	EXPECT_TRUE(testSet.TryPop(2, nullptr));
	EXPECT_EQ(testSet.Size(), 1);
	EXPECT_FALSE(testSet.Exist(2));

	// 남은 마지막 원소 pop
	EXPECT_TRUE(testSet.TryPop(3, &key));
	EXPECT_EQ(key, 3);
	EXPECT_TRUE(testSet.IsEmpty());
}

TEST(TreeSetTest, BoundTest) {
	LeakCheck;

	// 홀수 키만 삽입: 1, 3, 5, 7, 9
	TreeSet<int> testSet;
	testSet.Insert(1);
	testSet.Insert(3);
	testSet.Insert(5);
	testSet.Insert(7);
	testSet.Insert(9);

	// ─────────────────────────────────────────
	// LowerBound : key 이상(>=)인 첫 번째 key 포인터
	// ─────────────────────────────────────────

	// 존재하는 키 → 해당 키
	EXPECT_NE(testSet.LowerBound(1), nullptr);
	EXPECT_EQ(*testSet.LowerBound(1), 1);

	EXPECT_NE(testSet.LowerBound(5), nullptr);
	EXPECT_EQ(*testSet.LowerBound(5), 5);

	EXPECT_NE(testSet.LowerBound(9), nullptr);
	EXPECT_EQ(*testSet.LowerBound(9), 9);

	// 존재하지 않는 키, 범위 내 → 다음 큰 키
	EXPECT_NE(testSet.LowerBound(2), nullptr);
	EXPECT_EQ(*testSet.LowerBound(2), 3);	// 2 없음 → 3

	EXPECT_NE(testSet.LowerBound(6), nullptr);
	EXPECT_EQ(*testSet.LowerBound(6), 7);	// 6 없음 → 7

	// 최소 키보다 작은 키 → 최솟값
	EXPECT_NE(testSet.LowerBound(0), nullptr);
	EXPECT_EQ(*testSet.LowerBound(0), 1);

	// 최대 키보다 큰 키 → nullptr
	EXPECT_EQ(testSet.LowerBound(10), nullptr);

	// ─────────────────────────────────────────
	// UpperBound : key 초과(>)인 첫 번째 key 포인터
	// ─────────────────────────────────────────

	// 존재하는 키 → 다음 키
	EXPECT_NE(testSet.UpperBound(1), nullptr);
	EXPECT_EQ(*testSet.UpperBound(1), 3);	// 1 초과 → 3

	EXPECT_NE(testSet.UpperBound(5), nullptr);
	EXPECT_EQ(*testSet.UpperBound(5), 7);	// 5 초과 → 7

	// 존재하지 않는 키, 범위 내 → 다음 큰 키
	EXPECT_NE(testSet.UpperBound(2), nullptr);
	EXPECT_EQ(*testSet.UpperBound(2), 3);	// 2 없음, 2 초과 → 3

	// 최소 키보다 작은 키 → 최솟값
	EXPECT_NE(testSet.UpperBound(0), nullptr);
	EXPECT_EQ(*testSet.UpperBound(0), 1);

	// 최대 키 이상 → nullptr
	EXPECT_EQ(testSet.UpperBound(9),  nullptr);
	EXPECT_EQ(testSet.UpperBound(10), nullptr);

	// ─────────────────────────────────────────
	// 빈 셋에서 모두 nullptr
	// ─────────────────────────────────────────
	TreeSet<int> empty;
	EXPECT_EQ(empty.LowerBound(1), nullptr);
	EXPECT_EQ(empty.UpperBound(1), nullptr);
}

#endif // TEST_TreeSetTest == ON


