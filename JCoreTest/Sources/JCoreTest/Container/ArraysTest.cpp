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



#endif // TEST_ArraysTest == ON


