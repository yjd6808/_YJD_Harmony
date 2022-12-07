/*
	작성자 : 윤정도
	Arrays 클래스를 테스트합니다.
*/


#include <JCoreTest/CoreTest.h>
#include <JCore/Core.h>
#include <JCore/Container/Arrays.h>
#include <JCore/Random.h>


using namespace std;

#if TEST_ArraysTest == ON

// Arrays::Sort 함수 테스트
TEST(ArraysTest, Sort) {
	vector<int> normal;
	Random random;
	for (int i = 0; i < 500; i++) {
		const int r = random.GenerateInt(1, 10);
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
		
		std::cout << r << "개의 데이터 정렬결과 일치\n";
		*/
		normal.clear();
	}
	return;
FAILED:
	EXPECT_TRUE(false);
}

TEST(ArraysTest, BinarySearch) {
	std::vector<int> vec;

	// 데이터가 1개만 있는 경우
	{
		vec.push_back(1);
		EXPECT_TRUE(Arrays::BinarySearch(&vec[0], vec.size(), 1) == 0);
		EXPECT_TRUE(Arrays::BinarySearch(&vec[0], vec.size(), 2) == -1);
		EXPECT_TRUE(Arrays::BinarySearch(&vec[0], vec.size(), 0) == -1);
	}
	
	// 데이터가 2개만 있는 경우
	{
		vec.clear();
		vec.push_back(1);
		vec.push_back(2);

		EXPECT_TRUE(Arrays::BinarySearch(&vec[0], vec.size(), 1) == 0);
		EXPECT_TRUE(Arrays::BinarySearch(&vec[0], vec.size(), 2) == 1);
		EXPECT_TRUE(Arrays::BinarySearch(&vec[0], vec.size(), 0) == -1);
		EXPECT_TRUE(Arrays::BinarySearch(&vec[0], vec.size(), 3) == -1);
	}

	// 데이터가 3개만 있는 경우
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

	// 겹치는 숫자가 많은 경우
	for (int i = 0; i < 50; i++) {
		vec.clear();
		Random rand;
		int dataCount = rand.GenerateInt(1, 3000);
		while (dataCount--) {
			vec.push_back(rand.GenerateInt(1, 10));
		}

		std::sort(vec.begin(), vec.end());

		for (int i = 0; i < vec.size(); i++) {

			const int search = Arrays::BinarySearch(&vec[0], vec.size(), vec[i]);
			EXPECT_TRUE(search >= 0);
		}
	}

	// 겹치는 숫자가 거의 없는 경우
	for (int i = 0; i < 50; i++) {
		vec.clear();
		Random rand;
		int dataCount = rand.GenerateInt(1, 3000);
		while (dataCount--) {
			vec.push_back(rand.GenerateInt(1, 10000000));
		}

		std::sort(vec.begin(), vec.end());

		for (int i = 0; i < vec.size(); i++) {

			const int search = Arrays::BinarySearch(&vec[0], vec.size(), vec[i]);
			EXPECT_TRUE(search >= 0);
		}
	}
}

TEST(ArraysTest, LowerBound) {
	std::vector<int> vec;

	// 데이터가 1개만 있는 경우
	{
		vec.push_back(1);

		EXPECT_TRUE(Arrays::LowerBound(&vec[0], vec.size(), 1) == 0);
		EXPECT_TRUE(Arrays::LowerBound(&vec[0], vec.size(), 2) == 1);
		EXPECT_TRUE(Arrays::LowerBound(&vec[0], vec.size(), 0) == 0);
	}

	// 데이터가 2개만 있는 경우
	{
		vec.clear();
		vec.push_back(1);
		vec.push_back(2);

		EXPECT_TRUE(Arrays::LowerBound(&vec[0], vec.size(), 1) == 0);
		EXPECT_TRUE(Arrays::LowerBound(&vec[0], vec.size(), 2) == 1);
		EXPECT_TRUE(Arrays::LowerBound(&vec[0], vec.size(), 0) == 0);
		EXPECT_TRUE(Arrays::LowerBound(&vec[0], vec.size(), 3) == 2);
	}

	// 데이터가 3개만 있는 경우
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


