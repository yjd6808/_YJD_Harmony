/*
	작성자 : 윤정도
	Arrays 클래스를 테스트합니다.
*/


#include <JCoreTest/CoreTest.h>

#include <JCore/Core.h>
#include <JCore/Container/HashMap.h>
#include <JCore/Random.h>


using namespace std;

#if TEST_HashMapTest == ON

TEST(HashMapTest, Regular) {
	LeakCheck;
	HashMap<int, Model> modelMap;
	
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

TEST(HashMapTest, EnumeratorTest) {
	HashMap<int, int> map;
	for (int i = 0; i < 10; i++) {
		map.Insert(i, i + 1);
	}

	// 정방향 반복자 테스트
	const auto mapIter = map.Begin();
	for (int i = 0; i < mapIter->HasNext(); i++) {
		Pair<int, int>& pair = mapIter->Next();
		EXPECT_TRUE(map.Exist(pair.Key));
		EXPECT_TRUE(map.Values().Extension().Exist(i + 1));	
	}

	const auto mapKeyIter = map.Keys().Begin();
	for (int i = 0; i < mapKeyIter->HasNext(); i++) {
		EXPECT_TRUE(map.Exist(mapKeyIter->Next()));
	}

	const auto mapValueIter = map.Values ().Begin();
	for (int i = 0; i < mapValueIter->HasNext(); i++) {
		EXPECT_TRUE(map.Values().Extension().Exist(i + 1));
	}

	// 반대방향 반복자 테스트
	const auto mapRIter = map.End();
	for (int i = 9; mapRIter->HasPrevious(); i--) {
		Pair<int, int>& pair = mapRIter->Previous();
		EXPECT_TRUE(map.Exist(pair.Key));
		EXPECT_TRUE(map.Values().Extension().Exist(pair.Value));
	}
}

TEST(HashMapTest, ConstructorTest) {
	// 유니폼 이니셜라이저 생성자
	HashMap<int, Model> modelMap
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
	HashMap<int, Model> copy1(modelMap);
	EXPECT_TRUE(copy1[1].a == 3);
	EXPECT_TRUE(copy1[2].a == 4);
	EXPECT_TRUE(copy1[3].a == 5);
	EXPECT_TRUE(copy1[4].a == 6);
	EXPECT_TRUE(copy1.Size() == 4);

	// 이동 생성자
	HashMap<int, Model> move1(Move(modelMap));
	EXPECT_TRUE(modelMap.Size() == 0);
	EXPECT_TRUE(move1[1].a == 3);
	EXPECT_TRUE(move1[2].a == 4);
	EXPECT_TRUE(move1[3].a == 5);
	EXPECT_TRUE(move1[4].a == 6);
	EXPECT_TRUE(move1.Size() == 4);
}



TEST(HashMapTest, OperatorTest) {
	
	HashMap<int, Model> modelMap
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
	HashMap<int, Model> copy1
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
	HashMap<int, Model> move1
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

TEST(HashMapTest, InnerDestructorTest) {
	LeakCheck;

	HashMap<int, String> aq;
	bool toggle = false;
	for (int i = 0; i < 1024; i++) {
		if (i && i % 4 == 0) {
			if ((toggle = !toggle))
				aq.Remove(*aq.Keys().Extension().First());
			else
				aq.Remove(*aq.Keys().Extension().Last());
		} else {
			aq.Insert(i, "fsefesfesfesf");
		}
	}
}

#endif // TEST_HashMapTest == ON


