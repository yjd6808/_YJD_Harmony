/*
	작성자 : 윤정도
	오토 오브젝트 테스트
*/


#include <JCoreTest/CoreTest.h>
#include <JCore/Exception.h>
#include <JCore/Deletor.h>
#include <JCore/AutoObject.h>

using namespace JCore;
using namespace std;

#if TEST_AutoObjectTest == ON

namespace AutoObject {

int g_iValue = 100;

struct Increaser
{
	void operator()(int& val) {
		val++;
	}
};

TEST(AutoObjectTest, AutoRef) {

	{
		AutoRef<int, Increaser> ref(g_iValue);
	}
	// 삭제되면서 g_iValue 값이 1올라간다.
	EXPECT_TRUE(g_iValue == 101);
}




struct Model
{
	Model() { PrintFormat("디폴트 생성자 호출\n"); }
	Model(int da) { a = da; PrintFormat("일반 생성자 호출\n"); }
	~Model() { PrintFormat("소멸자 호출\n"); }

	int a = 3;
	int b = 3;
};

TEST(AutoObjectTest, AutoPointer) {
	{
		Model* ptr = new Model();
		AutoPointer<Model, Deletor<Model>> ref(ptr);
	}
}


} // namespace AutoObject



#endif // TEST_AutoObjectTest == ON
