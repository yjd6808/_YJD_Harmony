/*
	작성자 : 윤정도
	오토 오브젝트 테스트
*/


#include <JCoreTest/CoreTest.h>
#include <JCore/Deletor.h>
#include <JCore/Primitives/AutoPtr.h>


using namespace std;

#if TEST_AutoPtrTest == ON

namespace AutoPtrTest {

	
TEST(AutoObjectTest, AutoPointer) {
	LeakCheck;
	Model* ptr = new Model();
	AutoPtr<Model, Deletor<Model>> _(ptr);
}



} // namespace AutoPtrTest
#endif // TEST_AutoObjectTest == ON
