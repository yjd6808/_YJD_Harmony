/*
	�ۼ��� : ������
	���� ������Ʈ �׽�Ʈ
*/


#include <JCoreTest/CoreTest.h>
#include <JCoreTest/TestUtil/Object.h>

#include <JCore/Deletor.h>
#include <JCore/AutoObject.h>

using namespace JCore;
using namespace std;

#if TEST_AutoObjectTest == ON

namespace AutoObject {

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
	// �����Ǹ鼭 g_iValue ���� 1�ö󰣴�.
	EXPECT_TRUE(g_iValue == 101);
}


TEST(AutoObjectTest, AutoPointer) {
	{
		Model* ptr = new Model();
		AutoPointer<Model, Deletor<Model>> ref(ptr);
	}
}


} // namespace AutoObject



#endif // TEST_AutoObjectTest == ON
