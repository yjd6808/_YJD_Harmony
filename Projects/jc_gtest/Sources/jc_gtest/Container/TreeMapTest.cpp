/*
	작성자 : 윤정도
*/



#include "jc/Container/TreeMap.h"
#include "jc/Random.h"


using namespace std;

#if TEST_TreeMapTest == ON

TEST(TreeMapTest, Regular) {
	LeakCheck;
	TreeMap<int, Model> modelMap;
	
	// Insert 테스트
	// Key, Value 개별삽입
	EXPECT_TRUE(modelMap.Insert(1, Model(3)));	
	EXPECT_FALSE(modelMap.Insert(1, Model(3)));	// 중복된 키값 삽입 실패
	EXPECT_TRUE(modelMap.Insert(2, Model(4)));
	EXPECT_TRUE(modelMap.Insert(3, Model(5)));
	EXPECT_TRUE(modelMap.Insert(4, Model(6)));

	// KeyValuePair로 삽입
	EXPECT_TRUE(modelMap.Insert(MakePair(5, Model(7))));
	EXPECT_TRUE(modelMap.Insert(MakePair(6, Model(8))));

	EXPECT_TRUE(modelMap.Size() == 6);

	// Remove 테스트
	EXPECT_TRUE(modelMap.Remove(2));	// 삭제 성공
	EXPECT_FALSE(modelMap.Remove(2));	// 삭제 실패해야함
	EXPECT_TRUE(modelMap.Size() == 5);
	
	// operator[] 테스트
	EXPECT_THROW(modelMap[2], InvalidArgumentException); // 키값이 2에 해당하는 데이터는 삭제했으므로 예외를 던짐
	EXPECT_TRUE(modelMap[3].a == 5);
	
	// Clear 테스트
	modelMap.Clear();
	EXPECT_TRUE(modelMap.Size() == 0);
	EXPECT_TRUE(modelMap.Insert(2, Model(4)));	// 삭제 후 데이터 정상 삽입 되는지
	EXPECT_TRUE(modelMap.Insert(3, Model(5)));
	EXPECT_TRUE(modelMap.Insert(4, Model(6)));
	EXPECT_TRUE(modelMap.Size() == 3);	
}

TEST(TreeMapTest, EnumeratorTest) {
	TreeMap<int, int> map;
	for (int i = 0; i < 10; i++) {
		map.Insert(i, i + 1);
	}

	for (int i = 0; i < 10; i++) {
		EXPECT_TRUE(map.Exist(i));
	}

	// 정방향 반복자 테스트
	auto mapIter = map.Begin();
	for (int i = 0; mapIter.HasNext(); i++) {
		Pair<int, int>& pair = mapIter.Next();
		EXPECT_TRUE(map.Exist(pair.key_));
		EXPECT_TRUE(map.Values().Exist(i + 1));	
	}

	auto mapKeyIter = map.Keys().Begin();
	for (int i = 0; mapKeyIter.HasNext(); i++) {
		int key = mapKeyIter.Next();
		EXPECT_TRUE(map.Exist(key));
	}

	auto mapValueIter = map.Values().Begin();
	for (int i = 0;  mapValueIter.HasNext(); i++) {
		int value = mapValueIter.Next();
		EXPECT_TRUE(map.Values().Exist(value));
	}

	// 반대방향 반복자 테스트
	auto mapRIter = map.End();
	for (int i = 9; mapRIter.HasPrevious(); i--) {
		Pair<int, int>& pair = mapRIter.Previous();
		EXPECT_TRUE(map.Exist(pair.key_));
		EXPECT_TRUE(map.Values().Exist(pair.value_));
	}
}

TEST(TreeMapTest, ConstructorTest) {
	// 유니폼 이니셜라이저 생성자
	TreeMap<int, Model> modelMap
	{
		{ 1, Model(3) },
		{ 2, Model(4) },
		{ 3, Model(5) },
		{ 4, Model(6) },
	};

	EXPECT_TRUE(modelMap[1].a == 3);
	EXPECT_TRUE(modelMap[2].a == 4);
	EXPECT_TRUE(modelMap[3].a == 5);
	EXPECT_TRUE(modelMap[4].a == 6);
	EXPECT_TRUE(modelMap.Size() == 4);

	// 복사 생성자
	TreeMap<int, Model> copy1(modelMap);
	EXPECT_TRUE(copy1[1].a == 3);
	EXPECT_TRUE(copy1[2].a == 4);
	EXPECT_TRUE(copy1[3].a == 5);
	EXPECT_TRUE(copy1[4].a == 6);
	EXPECT_TRUE(copy1.Size() == 4);

	// 이동 생성자
	TreeMap<int, Model> move1(Move(modelMap));
	EXPECT_TRUE(modelMap.Size() == 0);
	EXPECT_TRUE(move1[1].a == 3);
	EXPECT_TRUE(move1[2].a == 4);
	EXPECT_TRUE(move1[3].a == 5);
	EXPECT_TRUE(move1[4].a == 6);
	EXPECT_TRUE(move1.Size() == 4);
}



TEST(TreeMapTest, OperatorTest) {
	
	TreeMap<int, Model> modelMap
	{
		{ 1, Model(1) }
	}; // 초기 아무데이터나 넣어둠

	// 유니폼 이니셜라이저 대입 연산자
	modelMap = 
	{
		{ 1, Model(3) },
		{ 2, Model(4) },
		{ 3, Model(5) },
		{ 4, Model(6) },
	};

	EXPECT_TRUE(modelMap[1].a == 3);
	EXPECT_TRUE(modelMap[2].a == 4);
	EXPECT_TRUE(modelMap[3].a == 5);
	EXPECT_TRUE(modelMap[4].a == 6);
	EXPECT_TRUE(modelMap.Size() == 4);

	// 복사 생성자
	TreeMap<int, Model> copy1
	{
		{ 1, Model(1) }
	}; // 초기 아무데이터나 넣어둠

	copy1 = modelMap;
	EXPECT_TRUE(copy1[1].a == 3);
	EXPECT_TRUE(copy1[2].a == 4);
	EXPECT_TRUE(copy1[3].a == 5);
	EXPECT_TRUE(copy1[4].a == 6);
	EXPECT_TRUE(copy1.Size() == 4);

	// 이동 생성자
	TreeMap<int, Model> move1
	{ 
		{ 1, Model(1) }
	}; // 초기 아무데이터나 넣어둠

	move1 = Move(modelMap);
	EXPECT_TRUE(modelMap.Size() == 0);
	EXPECT_TRUE(move1[1].a == 3);
	EXPECT_TRUE(move1[2].a == 4);
	EXPECT_TRUE(move1[3].a == 5);
	EXPECT_TRUE(move1[4].a == 6);
	EXPECT_TRUE(move1.Size() == 4);
}

TEST(TreeMapTest, InnerDestructorTest) {
	LeakCheck;

	TreeMap<int, String> aq;
	bool toggle = false;
	for (int i = 0; i < 1024; i++) {
		if (i && i % 4 == 0) {
			if ((toggle = !toggle))
				aq.Remove(*aq.Keys().First());
			else
				aq.Remove(*aq.Keys().Last());
		} else {
			aq.Insert(i, "fsefesfesfesf");
		}
	}
}


TEST(TreeMapTest, MemoryPool) {
	MemoryPoolLeakCheck;

	TreeMap<String, String, Comparator<String>, CDefaultArrayAllocator> q;
	for (int i = 0; i < 100'000; i++) 
		q.Insert(StringUtil::Format("ss%d", i), StringUtil::Format("ss%d", i));
}

TEST(TreeMapTest, TryPop) {
	LeakCheck;
	TreeMap<int, int> map;

	map.Insert(1, 10);
	map.Insert(2, 20);
	map.Insert(3, 30);
	EXPECT_EQ(map.Size(), 3);

	// 존재하는 키로 TryPop - 성공해야함
	int value = 0;
	EXPECT_TRUE(map.TryPop(1, &value));
	EXPECT_EQ(value, 10);
	EXPECT_EQ(map.Size(), 2);
	EXPECT_FALSE(map.Exist(1));

	// 이미 삭제된 키로 TryPop - 실패해야함
	EXPECT_FALSE(map.TryPop(1, &value));

	// 존재하지 않는 키로 TryPop - 실패해야함
	EXPECT_FALSE(map.TryPop(99, &value));

	// _pOut이 nullptr인 경우에도 정상 삭제
	EXPECT_TRUE(map.TryPop(2, nullptr));
	EXPECT_EQ(map.Size(), 1);
	EXPECT_FALSE(map.Exist(2));

	// 남은 마지막 원소 pop
	EXPECT_TRUE(map.TryPop(3, &value));
	EXPECT_EQ(value, 30);
	EXPECT_TRUE(map.IsEmpty());
}

TEST(TreeMapTest, BoundTest) {
	LeakCheck;

	// 홀수 키만 삽입: 1→10, 3→30, 5→50, 7→70, 9→90
	TreeMap<int, int> map;
	map.Insert(1, 10);
	map.Insert(3, 30);
	map.Insert(5, 50);
	map.Insert(7, 70);
	map.Insert(9, 90);

	// ─────────────────────────────────────────
	// LowerBoundValue : key 이상(>=)인 첫 번째 value 포인터
	// ─────────────────────────────────────────

	// 존재하는 키 → 해당 value
	EXPECT_NE(map.LowerBoundValue(1), nullptr);
	EXPECT_EQ(*map.LowerBoundValue(1), 10);

	EXPECT_NE(map.LowerBoundValue(5), nullptr);
	EXPECT_EQ(*map.LowerBoundValue(5), 50);

	EXPECT_NE(map.LowerBoundValue(9), nullptr);
	EXPECT_EQ(*map.LowerBoundValue(9), 90);

	// 존재하지 않는 키, 범위 내 → 다음 큰 키의 value
	EXPECT_NE(map.LowerBoundValue(2), nullptr);
	EXPECT_EQ(*map.LowerBoundValue(2), 30);	// 2 없음 → 3의 value

	EXPECT_NE(map.LowerBoundValue(6), nullptr);
	EXPECT_EQ(*map.LowerBoundValue(6), 70);	// 6 없음 → 7의 value

	// 최소 키보다 작은 키 → 최솟값의 value
	EXPECT_NE(map.LowerBoundValue(0), nullptr);
	EXPECT_EQ(*map.LowerBoundValue(0), 10);

	// 최대 키보다 큰 키 → nullptr
	EXPECT_EQ(map.LowerBoundValue(10), nullptr);

	// ─────────────────────────────────────────
	// UpperBoundValue : key 초과(>)인 첫 번째 value 포인터
	// ─────────────────────────────────────────

	// 존재하는 키 → 다음 키의 value
	EXPECT_NE(map.UpperBoundValue(1), nullptr);
	EXPECT_EQ(*map.UpperBoundValue(1), 30);	// 1 초과 → 3의 value

	EXPECT_NE(map.UpperBoundValue(5), nullptr);
	EXPECT_EQ(*map.UpperBoundValue(5), 70);	// 5 초과 → 7의 value

	// 존재하지 않는 키, 범위 내 → 다음 큰 키의 value
	EXPECT_NE(map.UpperBoundValue(2), nullptr);
	EXPECT_EQ(*map.UpperBoundValue(2), 30);	// 2 없음, 2 초과 → 3의 value

	// 최소 키보다 작은 키 → 최솟값의 value
	EXPECT_NE(map.UpperBoundValue(0), nullptr);
	EXPECT_EQ(*map.UpperBoundValue(0), 10);

	// 최대 키 이상 → nullptr
	EXPECT_EQ(map.UpperBoundValue(9),  nullptr);
	EXPECT_EQ(map.UpperBoundValue(10), nullptr);

	// ─────────────────────────────────────────
	// LowerBoundPair : key 이상(>=)인 첫 번째 {key, value} 포인터
	// ─────────────────────────────────────────

	// 존재하는 키 → 해당 pair
	auto* pPair = map.LowerBoundPair(3);
	EXPECT_NE(pPair, nullptr);
	EXPECT_EQ(pPair->key_,   3);
	EXPECT_EQ(pPair->value_, 30);

	// 존재하지 않는 키 → 다음 큰 키의 pair
	pPair = map.LowerBoundPair(4);
	EXPECT_NE(pPair, nullptr);
	EXPECT_EQ(pPair->key_,   5);
	EXPECT_EQ(pPair->value_, 50);

	// 최대 키 초과 → nullptr
	EXPECT_EQ(map.LowerBoundPair(10), nullptr);

	// ─────────────────────────────────────────
	// UpperBoundKey : key 초과(>)인 첫 번째 key 포인터
	// ─────────────────────────────────────────

	// 존재하는 키 → 다음 키
	EXPECT_NE(map.UpperBoundKey(5), nullptr);
	EXPECT_EQ(*map.UpperBoundKey(5), 7);

	// 존재하지 않는 키 → 다음 큰 키
	EXPECT_NE(map.UpperBoundKey(6), nullptr);
	EXPECT_EQ(*map.UpperBoundKey(6), 7);

	// 최대 키 이상 → nullptr
	EXPECT_EQ(map.UpperBoundKey(9),  nullptr);
	EXPECT_EQ(map.UpperBoundKey(10), nullptr);

	// ─────────────────────────────────────────
	// 빈 맵에서 모두 nullptr
	// ─────────────────────────────────────────
	TreeMap<int, int> empty;
	EXPECT_EQ(empty.LowerBoundValue(1),  nullptr);
	EXPECT_EQ(empty.UpperBoundValue(1),  nullptr);
	EXPECT_EQ(empty.LowerBoundPair(1),   nullptr);
	EXPECT_EQ(empty.UpperBoundKey(1),    nullptr);
}

TEST(TreeMapTest, ConstIteration) {
	LeakCheck;

	const TreeMap<int, int> map{
		{ 1, 10 },
		{ 2, 20 },
		{ 3, 30 },
		{ 4, 40 },
	};

	// const 컨테이너의 값 이터레이터 순회
	int count = 0;
	auto it = map.Begin();
	while (it.HasNext()) {
		Pair<int, int>& pair = it.Next();
		EXPECT_TRUE(map.Exist(pair.key_));
		++count;
	}
	EXPECT_TRUE(count == 4);

	// const 컨테이너의 Keys()/Values() 순회
	auto keyIt = map.Keys().Begin();
	while (keyIt.HasNext()) {
		EXPECT_TRUE(map.Exist(keyIt.Next()));
	}

	auto valueIt = map.Values().Begin();
	int valueSum = 0;
	while (valueIt.HasNext()) {
		valueSum += valueIt.Next();
	}
	EXPECT_TRUE(valueSum == 100);

	EXPECT_TRUE(map.Values().Exist(30));
	EXPECT_TRUE(map.Keys().Exist(2));
}

#endif // TEST_TreeMapTest == ON


