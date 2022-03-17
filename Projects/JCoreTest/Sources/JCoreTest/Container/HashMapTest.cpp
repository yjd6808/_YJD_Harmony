/*
	�ۼ��� : ������
	Arrays Ŭ������ �׽�Ʈ�մϴ�.
*/


#include <JCoreTest/CoreTest.h>
#include <JCoreTest/TestUtil/Object.h>
#include <JCore/Core.h>
#include <JCore/Container/HashMap.h>
#include <JCore/Random.h>

using namespace JCore;
using namespace std;

#if TEST_HashMapTest == ON

TEST(HashMapTest, Regular) {
	MemoryLeakDetector detector;
	HashMap<int, Model> modelMap;
	
	// Insert �׽�Ʈ
	// Key, Value ��������
	EXPECT_TRUE(modelMap.Insert(1, Model(3)));	
	EXPECT_FALSE(modelMap.Insert(1, Model(3)));	// �ߺ��� Ű�� ���� ����
	EXPECT_TRUE(modelMap.Insert(2, Model(4)));
	EXPECT_TRUE(modelMap.Insert(3, Model(5)));
	EXPECT_TRUE(modelMap.Insert(4, Model(6)));

	// KeyValuePair�� ����
	EXPECT_TRUE(modelMap.Insert(MakePair(5, Model(7))));
	EXPECT_TRUE(modelMap.Insert(MakePair(6, Model(8))));

	EXPECT_TRUE(modelMap.Size() == 6);

	// Remove �׽�Ʈ
	EXPECT_TRUE(modelMap.Remove(2));	// ���� ����
	EXPECT_FALSE(modelMap.Remove(2));	// ���� �����ؾ���
	EXPECT_TRUE(modelMap.Size() == 5);
	
	// operator[] �׽�Ʈ
	EXPECT_THROW(modelMap[2], InvalidArgumentException); // Ű���� 2�� �ش��ϴ� �����ʹ� ���������Ƿ� ���ܸ� ����
	EXPECT_TRUE(modelMap[3].a == 5);
	
	// Clear �׽�Ʈ
	modelMap.Clear();
	EXPECT_TRUE(modelMap.Size() == 0);
	EXPECT_TRUE(modelMap.Insert(2, Model(4)));	// ���� �� ������ ���� ���� �Ǵ���
	EXPECT_TRUE(modelMap.Insert(3, Model(5)));
	EXPECT_TRUE(modelMap.Insert(4, Model(6)));
	EXPECT_TRUE(modelMap.Size() == 3);	
}

TEST(HashMapTest, EnumeratorTest) {
	HashMap<int, int> map;
	for (int i = 0; i < 10; i++) {
		map.Insert(i, i + 1);
	}

	// ������ �ݺ��� �׽�Ʈ
	auto mapIter = map.Begin();
	for (int i = 0; i < mapIter->HasNext(); i++) {
		KeyValuePair<int, int>& pair = mapIter->Next();
		EXPECT_TRUE(map.Exist(pair.Key));
		EXPECT_TRUE(map.Values().Extension().Exist(i + 1));	
	}
	
	auto mapKeyIter = map.Keys().Begin();
	for (int i = 0; i < mapKeyIter->HasNext(); i++) {
		EXPECT_TRUE(map.Exist(mapKeyIter->Next()));
	}

	auto mapValueIter = map.Values ().Begin();
	for (int i = 0; i < mapValueIter->HasNext(); i++) {
		EXPECT_TRUE(map.Values().Extension().Exist(i + 1));
	}

	// �ݴ���� �ݺ��� �׽�Ʈ
	auto mapRIter = map.End();
	for (int i = 9; mapRIter->HasPrevious(); i--) {
		KeyValuePair<int, int>& pair = mapRIter->Previous();
		EXPECT_TRUE(map.Exist(pair.Key));
		EXPECT_TRUE(map.Values().Extension().Exist(pair.Value));
	}
}

TEST(HashMapTest, ConstructorTest) {
	// ������ �̴ϼȶ����� ������
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

	// ���� ������
	HashMap<int, Model> copy1(modelMap);
	EXPECT_TRUE(copy1[1].a == 3);
	EXPECT_TRUE(copy1[2].a == 4);
	EXPECT_TRUE(copy1[3].a == 5);
	EXPECT_TRUE(copy1[4].a == 6);
	EXPECT_TRUE(copy1.Size() == 4);

	// �̵� ������
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
	}; // �ʱ� �ƹ������ͳ� �־��

	// ������ �̴ϼȶ����� ���� ������
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

	// ���� ������
	HashMap<int, Model> copy1
	{
		{ 1, Model(1) }
	}; // �ʱ� �ƹ������ͳ� �־��

	copy1 = modelMap;
	EXPECT_TRUE(copy1[1].a == 3);
	EXPECT_TRUE(copy1[2].a == 4);
	EXPECT_TRUE(copy1[3].a == 5);
	EXPECT_TRUE(copy1[4].a == 6);
	EXPECT_TRUE(copy1.Size() == 4);

	// �̵� ������
	HashMap<int, Model> move1
	{ 
		{ 1, Model(1) }
	}; // �ʱ� �ƹ������ͳ� �־��

	move1 = Move(modelMap);
	EXPECT_TRUE(modelMap.Size() == 0);
	EXPECT_TRUE(move1[1].a == 3);
	EXPECT_TRUE(move1[2].a == 4);
	EXPECT_TRUE(move1[3].a == 5);
	EXPECT_TRUE(move1[4].a == 6);
	EXPECT_TRUE(move1.Size() == 4);
}


#endif // TEST_HashMapTest == ON


