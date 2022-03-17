/*
	�ۼ��� : ������
	Arrays Ŭ������ �׽�Ʈ�մϴ�.
*/


#include <JCoreTest/CoreTest.h>
#include <JCore/Core.h>
#include <JCore/Container/Arrays.h>
#include <JCore/Random.h>

using namespace JCore;
using namespace std;

#if TEST_ArraysTest == ON

// Arrays::Sort �Լ� �׽�Ʈ
TEST(ArraysTest, Sort) {
	vector<int> normal;
	Random random;
	for (int i = 0; i < 500; i++) {
		int r = random.GenerateInt(1, 10);
		int* myArray = new int[r];
		for (int j = 0; j < r; j++) {
			int gen = random.GenerateInt(0, 50);
			normal.push_back(gen);
			myArray[j] = gen;
		}
		std::sort(normal.begin(), normal.end());
		Arrays::Sort(myArray, r);

		for (int j = 0; j < r; j++) {
			if (normal[j] != myArray[j]) {
				goto FAILED;
			}

		}
		/*
		for (int j = 0; j < r; j++) {
			//Print("%d ", normal[j]);
		}
		PrintFormat();
		for (int j = 0; j < r; j++) {
			Print("%d ", myArray[j]);
		}
		PrintFormat();
		
		std::cout << r << "���� ������ ���İ�� ��ġ\n";
		*/
		normal.clear();
	}
	return;
FAILED:
	EXPECT_TRUE(false);
}

TEST(ArraysTest, BinarySearch) {
	std::vector<int> vec;

	// �����Ͱ� 1���� �ִ� ���
	{
		vec.push_back(1);
		EXPECT_TRUE(Arrays::BinarySearch(&vec[0], vec.size(), 1) == 0);
		EXPECT_TRUE(Arrays::BinarySearch(&vec[0], vec.size(), 2) == -1);
		EXPECT_TRUE(Arrays::BinarySearch(&vec[0], vec.size(), 0) == -1);
	}
	
	// �����Ͱ� 2���� �ִ� ���
	{
		vec.clear();
		vec.push_back(1);
		vec.push_back(2);

		EXPECT_TRUE(Arrays::BinarySearch(&vec[0], vec.size(), 1) == 0);
		EXPECT_TRUE(Arrays::BinarySearch(&vec[0], vec.size(), 2) == 1);
		EXPECT_TRUE(Arrays::BinarySearch(&vec[0], vec.size(), 0) == -1);
		EXPECT_TRUE(Arrays::BinarySearch(&vec[0], vec.size(), 3) == -1);
	}

	// �����Ͱ� 3���� �ִ� ���
	{
		vec.clear();
		vec.push_back(1);
		vec.push_back(2);
		vec.push_back(3);

		EXPECT_TRUE(Arrays::BinarySearch(&vec[0], vec.size(), 1) == 0);
		EXPECT_TRUE(Arrays::BinarySearch(&vec[0], vec.size(), 2) == 1);
		EXPECT_TRUE(Arrays::BinarySearch(&vec[0], vec.size(), 3) == 2);
		EXPECT_TRUE(Arrays::BinarySearch(&vec[0], vec.size(), 0) == -1);
		EXPECT_TRUE(Arrays::BinarySearch(&vec[0], vec.size(), 4) == -1);
	}

	// ��ġ�� ���ڰ� ���� ���
	for (int i = 0; i < 50; i++) {
		vec.clear();
		Random rand;
		int dataCount = rand.GenerateInt(1, 3000);
		while (dataCount--) {
			vec.push_back(rand.GenerateInt(1, 10));
		}

		std::sort(vec.begin(), vec.end());

		for (int i = 0; i < vec.size(); i++) {

			int search = Arrays::BinarySearch(&vec[0], vec.size(), vec[i]);
			EXPECT_TRUE(search >= 0);
		}
	}

	// ��ġ�� ���ڰ� ���� ���� ���
	for (int i = 0; i < 50; i++) {
		vec.clear();
		Random rand;
		int dataCount = rand.GenerateInt(1, 3000);
		while (dataCount--) {
			vec.push_back(rand.GenerateInt(1, 10000000));
		}

		std::sort(vec.begin(), vec.end());

		for (int i = 0; i < vec.size(); i++) {

			int search = Arrays::BinarySearch(&vec[0], vec.size(), vec[i]);
			EXPECT_TRUE(search >= 0);
		}
	}
}

TEST(ArraysTest, LowerBound) {
	std::vector<int> vec;

	// �����Ͱ� 1���� �ִ� ���
	{
		vec.push_back(1);

		EXPECT_TRUE(Arrays::LowerBound(&vec[0], vec.size(), 1) == 0);
		EXPECT_TRUE(Arrays::LowerBound(&vec[0], vec.size(), 2) == 1);
		EXPECT_TRUE(Arrays::LowerBound(&vec[0], vec.size(), 0) == 0);
	}

	// �����Ͱ� 2���� �ִ� ���
	{
		vec.clear();
		vec.push_back(1);
		vec.push_back(2);

		EXPECT_TRUE(Arrays::LowerBound(&vec[0], vec.size(), 1) == 0);
		EXPECT_TRUE(Arrays::LowerBound(&vec[0], vec.size(), 2) == 1);
		EXPECT_TRUE(Arrays::LowerBound(&vec[0], vec.size(), 0) == 0);
		EXPECT_TRUE(Arrays::LowerBound(&vec[0], vec.size(), 3) == 2);
	}

	// �����Ͱ� 3���� �ִ� ���
	{
		vec.clear();
		vec.push_back(1);
		vec.push_back(2);
		vec.push_back(3);

		EXPECT_TRUE(Arrays::LowerBound(&vec[0], vec.size(), 1) == 0);
		EXPECT_TRUE(Arrays::LowerBound(&vec[0], vec.size(), 2) == 1);
		EXPECT_TRUE(Arrays::LowerBound(&vec[0], vec.size(), 3) == 2);
		EXPECT_TRUE(Arrays::LowerBound(&vec[0], vec.size(), 0) == 0);
		EXPECT_TRUE(Arrays::LowerBound(&vec[0], vec.size(), 4) == 3);
	}
}

#endif // TEST_ArraysTest == ON


