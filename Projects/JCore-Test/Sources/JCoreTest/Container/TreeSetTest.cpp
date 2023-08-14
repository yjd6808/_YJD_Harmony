/*
 * 작성자: 윤정도
 * 생성일: 8/15/2023 7:40:18 AM
 * =====================
 * TreeSet 클래스를 테스트합니다.
 */

#include <JCoreTest/CoreTest.h>

#include <JCore/Core.h>
#include <JCore/Container/TreeSet.h>
#include <JCore/Random.h>

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
	const auto mapIter = testSet.Begin();
	for (int i = 0; mapIter->HasNext(); i++) {
		int& key = mapIter->Next();
		EXPECT_TRUE(testSet.Exist(key));
		EXPECT_TRUE(testSet.Extension().Exist(i));
	}

	// 반대방향 반복자 테스트
	const auto mapRIter = testSet.End();
	for (int i = 9; mapRIter->HasPrevious(); i--) {
		int& key = mapRIter->Previous();
		EXPECT_TRUE(testSet.Exist(key));
		EXPECT_TRUE(testSet.Extension().Exist(key));
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
	TreeSet<String, Comparator<String>, DefaultArrayAllocator> q;
	for (int i = 0; i < 1'000; i++) {
		q.Insert(StringUtil::Format("ss%d", i));
	}
}

TEST(TreeSetTest, Sort) {
	LeakCheck;

	TreeSet<int> testSet;
	bool check[100]{};		// 현재 set가 unique여부를 보장하지 않는다는 가정하에 많든 채
	for (int i = 0; i < 100; ++i) {
		while (true) {
			int v = Random::GenerateInt(0, 100);
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

	TreeSet<int>::TEnumerator it = testSet.Begin();
	for (int i = 0; i < 100; ++i) {
		int v = it->Current();

		if (it->HasNext())
			it->Next();
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

#endif // TEST_TreeSetTest == ON


