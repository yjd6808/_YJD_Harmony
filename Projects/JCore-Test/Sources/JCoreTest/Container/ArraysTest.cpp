/*
	작성자 : 윤정도
	Arrays 클래스를 테스트합니다.
*/


#include <JCoreTest/CoreTest.h>
#include <JCore/Core.h>
#include <JCore/Container/Arrays.h>
#include <JCore/Random.h>

USING_NS_STD;

#if TEST_ArraysTest == ON

// Arrays::Sort 함수 테스트
TEST(ArraysTest, Sort) {

	static auto fnQuickSort = [](int* arr, int arrSize) { Arrays::Sort(arr, arrSize); };
	static auto fnInsertionSort = [](int* arr, int arrSize) { Arrays::InsertionSort(arr, arrSize); };
	static auto fnHeapSort = [](int* arr, int arrSize) { Arrays::HeapSort(arr, arrSize); };

	constexpr int SORT_ALGO_COUNT = 3;
	std::function<void(int*, int)> fnSortAlgorithm[]{
		fnQuickSort,
		fnInsertionSort,
		fnHeapSort
	};

	std::string szAlgorithm[]{
		"quick_sort",
		"insertion_sort",
		"heap_sort"
	};

	{
		vector<int> standard;	// STL 정렬 알고리즘 적용
		vector<int> target;		// 내가 구현한 정렬 알고리즘 적용
		std::string failed_flag;

		for (int k = 0; k < SORT_ALGO_COUNT; ++k) {
			for (int i = 0; i < 500; i++) {
				const int element_count = Random::GenerateInt(1, 10);
				standard.clear();
				target.clear();

				for (int j = 0; j < element_count; j++) {
					int new_val = Random::GenerateInt(0, 50);
					standard.push_back(new_val);
					target.push_back(new_val);
				}

				std::sort(standard.begin(), standard.end());
				fnSortAlgorithm[k](target.data(), target.size());

				for (int j = 0; j < element_count; j++) {
					if (standard[j] != target[j]) {
						failed_flag = szAlgorithm[k];
						goto FAILED;
					}
				}
			}
		FAILED:
			EXPECT_TRUE(failed_flag.length() == 0);
			failed_flag = "";
		}
	}

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

	// 인덱스 테스트
	{
		int arr[]
		{
			1 << 0, // 1
			1 << 1,	// 2
			1 << 2,	// 4
			1 << 3,	// 8 
			1 << 4,	// 16
			1 << 5,	// 32
			1 << 6,	// 64
			1 << 7,	// 128
			1 << 8,	// 256
			1 << 9	// 512
		};
		int max_size = sizeof(arr) / sizeof(int); // 10

		int idx = Arrays::LowerBound(arr, 56);
		EXPECT_TRUE(idx == 6);
		
		idx = Arrays::LowerBound(arr, 1 << 9);
		EXPECT_TRUE(idx == 9);

		idx = Arrays::LowerBound(arr, 1 << 9 + 1);
		EXPECT_TRUE(idx == max_size);

		idx = Arrays::LowerBound(arr, 0);
		EXPECT_TRUE(idx == 0);
	}
}


TEST(ArraysTest, Heapify) {
	std::vector<int> v;
	std::string flag;
	
	constexpr int HEAPIFY_TYPE = 2;
	static std::function<void(int*, int)> fnHeapify[]{
		[](int* arr, int arrSize) { Arrays::MakeHeapSiftDown(arr, arrSize, NaturalOrder{}); },
		[](int* arr, int arrSize) { Arrays::MakeHeapSiftUp(arr, arrSize, NaturalOrder{}); }
	};
	std::string szAlgorithm[]{
		"sift_down",
		"sift_up"
	};


	for (int k = 0; k < HEAPIFY_TYPE; ++k) {
		for (int i = 0; i < 300; ++i) {
			const int element_count = Random::GenerateInt(1, 10);
			v.clear();

			for (int j = 0; j < element_count; j++) {
				int new_val = Random::GenerateInt(0, 50);
				v.push_back(new_val);
			}

			// 최대 힙 만들기
			fnHeapify[k](v.data(), v.size());

			for (int j = element_count - 1; j >= 0; --j) {
				Arrays::PopHeap(v.data(), v.size(), NaturalOrder{});
				int val = v.back();
				v.pop_back();

				// 힙에서 꺼낸 데이터는 남아있는 모든 원소보다 크거나 같아야한다.
				for (int elem : v) {
					if (elem > val) {
						flag = szAlgorithm[k];
						goto FAILED;
					}
				}
			}
		}
	FAILED:
		EXPECT_TRUE(flag == "");
		flag = "";
	}
	


}

#endif // TEST_ArraysTest == ON


