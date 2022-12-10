/*
	작성자 : 윤정도
	오토 오브젝트 테스트
*/


#include <JCoreTest/CoreTest.h>
#include <JCore/Deletor.h>
#include <JCore/RAII/AutoRef.h>


using namespace std;

#if TEST_AutoRefTest == ON

namespace AutoRefTest {

int g_iValue = 100;

struct Increaser
{
	void operator()(int& val) const {
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




} // namespace AutoObject



#endif // TEST_AutoRefTest == ON
