/*
	작성자 : 윤정도
	HashSet 클래스를 테스트합니다.
*/


#include <JCoreTest/CoreTest.h>

#include <JCore/Core.h>
#include <JCore/Container/HashSet.h>
#include <JCore/Random.h>

USING_NS_STD;

#if TEST_HashSetTest == ON

TEST(HashSetTest, Regular) {
	LeakCheck;
	HashSet<int> testSet;
	
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

TEST(HashSetTest, EnumeratorTest) {
	HashSet<int> testSet;
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

TEST(HashSetTest, ConstructorTest) {
	// 유니폼 이니셜라이저 생성자
	HashSet testSet{ 1, 2, 3, 4 };

	EXPECT_TRUE(testSet.Exist(1));
	EXPECT_TRUE(testSet.Exist(2));
	EXPECT_TRUE(testSet.Exist(3));
	EXPECT_TRUE(testSet.Exist(4));
	EXPECT_TRUE(testSet.Size() == 4);

	// 복사 생성자
	HashSet copy1(testSet);
	EXPECT_TRUE(copy1.Exist(1));
	EXPECT_TRUE(copy1.Exist(2));
	EXPECT_TRUE(copy1.Exist(3));
	EXPECT_TRUE(copy1.Exist(4));
	EXPECT_TRUE(copy1.Size() == 4);

	// 이동 생성자
	HashSet move1(Move(testSet));
	EXPECT_TRUE(testSet.Size() == 0);
	EXPECT_TRUE(testSet.IsEmpty());
	EXPECT_TRUE(copy1.Exist(1));
	EXPECT_TRUE(copy1.Exist(2));
	EXPECT_TRUE(copy1.Exist(3));
	EXPECT_TRUE(copy1.Exist(4));
	EXPECT_TRUE(copy1.Size() == 4);
}

TEST(HashSetTest, OperatorTest) {
	HashSet testSet{ 1 };

	// 유니폼 이니셜라이저 대입 연산자
	testSet = { 1, 2, 3, 4 };

	EXPECT_TRUE(testSet.Exist(1));
	EXPECT_TRUE(testSet.Exist(2));
	EXPECT_TRUE(testSet.Exist(3));
	EXPECT_TRUE(testSet.Exist(4));
	EXPECT_TRUE(testSet.Size() == 4);

	// 복사 생성자
	HashSet copy1{ {1} };

	copy1 = testSet;
	EXPECT_TRUE(copy1.Exist(1));
	EXPECT_TRUE(copy1.Exist(2));
	EXPECT_TRUE(copy1.Exist(3));
	EXPECT_TRUE(copy1.Exist(4));
	EXPECT_TRUE(copy1.Size() == 4);

	// 이동 생성자
	HashSet move1{ {1} };

	move1 = Move(testSet);
	EXPECT_TRUE(testSet.Size() == 0);
	EXPECT_TRUE(testSet.IsEmpty());
	EXPECT_TRUE(move1.Exist(1));
	EXPECT_TRUE(move1.Exist(2));
	EXPECT_TRUE(move1.Exist(3));
	EXPECT_TRUE(move1.Exist(4));
	EXPECT_TRUE(move1.Size() == 4);
}

TEST(HashSetTest, NonFundamental) {
	LeakCheck;

	HashSet<String> testSet;
	EXPECT_TRUE(testSet.Insert("abcd"));
	EXPECT_TRUE(testSet.Insert("abce"));

	EXPECT_TRUE(testSet.Size() == 2);

	EXPECT_TRUE(testSet.Exist("abcd"));
	EXPECT_TRUE(testSet.Exist("abce"));

	EXPECT_TRUE(testSet.Remove("abcd"));
	EXPECT_TRUE(testSet.Remove("abce"));

	EXPECT_TRUE(testSet.Size() == 0);

	testSet = { "abcd", "abcde", "abcdef"};
	EXPECT_TRUE(testSet.Exist("abcd"));
	EXPECT_TRUE(testSet.Exist("abcde"));
	EXPECT_TRUE(testSet.Exist("abcdef"));
	testSet.Clear();
	EXPECT_TRUE(testSet.Size() == 0);
}


TEST(HashSetTest, MemoryPool) {
	MemoryPoolLeakCheck
	HashSet<String, DefaultArrayAllocator> q;
	for (int i = 0; i < 1'000; i++) {
		q.Insert(StringUtil::Format("ss%d", i));
	}
}


TEST(HashSetTest, ZeroCapacityTest) {
	LeakCheck;
	HashSet<int> testSet1(0);
	HashSet<int> testSet2(testSet1);
	HashSet<int> _(testSet1);
	HashSet<int> testSet3(Move(_));

	EXPECT_EQ(testSet1.BucketCount(), 0);
	EXPECT_EQ(testSet2.BucketCount(), 0);
	EXPECT_EQ(testSet3.BucketCount(), 0);

	testSet1.Insert(1);
	testSet2.Insert(1);
	testSet3.Insert(1);

	EXPECT_TRUE(testSet1.BucketCount() > 0);
	EXPECT_TRUE(testSet2.BucketCount() > 0);
	EXPECT_TRUE(testSet3.BucketCount() > 0);

	EXPECT_TRUE(testSet1.Exist(1));
	EXPECT_TRUE(testSet2.Exist(1));
	EXPECT_TRUE(testSet3.Exist(1));
}


TEST(HashSetTest, Pointer) {
	LeakCheck;	// 릭이 없어야함.
	int* ptr = new int;
	HashSet<int*> testSet1(0);
	EXPECT_TRUE(testSet1.Insert(ptr));
	EXPECT_FALSE(testSet1.Insert(ptr));

	testSet1.Insert(new int);
	testSet1.Insert(new int);
	testSet1.Insert(new int);
	testSet1.Insert(new int);
	testSet1.ForEachDelete();
}

#endif // TEST_HashSetTest == ON


