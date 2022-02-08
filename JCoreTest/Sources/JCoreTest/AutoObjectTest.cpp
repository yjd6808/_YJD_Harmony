/*
	�ۼ��� : ������
	���� ������Ʈ �׽�Ʈ
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
	// �����Ǹ鼭 g_iValue ���� 1�ö󰣴�.
	EXPECT_TRUE(g_iValue == 101);
}




struct Model
{
	Model() { PrintFormat("����Ʈ ������ ȣ��\n"); }
	Model(int da) { a = da; PrintFormat("�Ϲ� ������ ȣ��\n"); }
	~Model() { PrintFormat("�Ҹ��� ȣ��\n"); }

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
