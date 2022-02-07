/*
	작성자 : 윤정도
	스마트 벡터 테스트
*/


#include <JCoreTest/CoreTest.h>
#include <JCore/Exception.h>
#include <JCore/SmartPointer.h>

using namespace JCore;
using namespace std;

#if TEST_SmartVectorImplTest == ON

struct Data
{
	int a;
	int b;
};

class SmartVector
{

};


template <typename T>
struct Over
{
	Over() {
		cout << "T" << "\n";
	}
	T regend;
};

template <typename T>
struct Over<T[]>
{
	Over() {
		cout << "T[]" << "\n";
	}
	~Over() {
		delete[] regend;
	}

	T* regend;
};

TEST(SmartVectorImplTest, ExceptionTest) {
	Over<int> a;
	Over<int[]> f;
	f.regend = new int[50] {};
}

#endif // TEST_SmartVectorImplTest == ON

