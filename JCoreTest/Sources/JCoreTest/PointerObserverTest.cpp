/*
	작성자 : 윤정도
	포인터 감시자 테스트
*/


#include <JCoreTest/CoreTest.h>
#include <JCore/String.h>
#include <JCore/Comparator.h>
#include <JCore/TypeTraits.h>
#include <JCore/PointerObserver.h>

using namespace JCore;
using namespace std;

#if TEST_PointerObserverTest == ON

// 보이드 타입의 포인터 테스트
TEST(PointerObserverTest, VoidOwner_VoidWatcher) {
	int* pNew = new int(1000);
	VoidOwner owner(pNew);

	*owner.Get<int>() = 300;			// 값 수정
	void* pVoidNew = owner.GetRaw();

	EXPECT_TRUE(*pNew == 300);
	EXPECT_TRUE(pNew == pVoidNew);
	
	// 오너는 감시자이기도 하다. 따라서 초기 1로 설정
	EXPECT_TRUE(owner.WatcherCount() == 1);

	VoidWatcher watch1(owner);
	VoidWatcher watch2;

	// watch1은 owner를 감시한다.
	EXPECT_TRUE(watch1.WatcherCount() == 2);

	// watch2는 아무것도 감시하고 있지 않는다.
	EXPECT_TRUE(watch2.WatcherCount() == 0);
	EXPECT_TRUE(watch2.Exist() == false);

	{
		// 오너의 소유권을 다른 오너에게 이전한다.
		VoidOwner tempOwner(Move(owner));	

		// 기존 오너는 이제 유효하지 않다.
		EXPECT_TRUE(owner.Exist() == false);

		// watch1은 소유권이 이전된 녀석(tempOwner)을 감시한다.
		// 따라서 달라진게 없다.
		EXPECT_TRUE(watch1.WatcherCount() == 2);
		
		watch2 = tempOwner;

		EXPECT_TRUE(watch2.WatcherCount() == 3);
	}

	// tempOwner가 스택프레임이 해제됨에따라 삭제되었다.
	// 이제 이를 감시중인 watch1과 watch2는 모두 무효화된다.
	// 오너는 사라졌지만 감시는 watch1, watch2가 비어있는 곳을 감시하고있다.
	// 이 두 녀석이 사라질때까지 제어 블록은 생존하고 있는다.
	EXPECT_TRUE(watch1.WatcherCount() == 2);	
	EXPECT_TRUE(watch1.Exist() == false);
	EXPECT_TRUE(watch2.Exist() == false);

}


struct Model
{
	Model() { PrintFormat("디폴트 생성자 호출\n"); }
	Model(int da) { a = da; PrintFormat("일반 생성자 호출\n"); }
	~Model() { PrintFormat("소멸자 호출\n"); }

	int a = 3;
	int b = 3;
};



// 타입을 가지는 오너
TEST(PointerObserverTest, Owner_Watcher) {
	Owner<Model> owner = MakeOwner<Model>(1000);

	// 간접 참조 연산자 오버로딩
	(*owner).a = 500;
	(*owner).b = 300;

	// 화살표 오버로딩
	EXPECT_TRUE(owner->a == 500 && owner->b == 300);

	// 오너는 감시자이기도 하다. 따라서 초기 1로 설정
	EXPECT_TRUE(owner.WatcherCount() == 1);

	Watcher<Model> watch1(owner);
	Watcher<Model> watch2;

	// watch1은 owner를 감시한다.
	EXPECT_TRUE(watch1.WatcherCount() == 2);

	// watch2는 아무것도 감시하고 있지 않는다.
	EXPECT_TRUE(watch2.WatcherCount() == 0);
	EXPECT_TRUE(watch2.Exist() == false);

	{
		// 오너의 소유권을 다른 오너에게 이전한다.
		Owner<Model> tempOwner(Move(owner));

		// 기존 오너는 이제 유효하지 않다.
		EXPECT_TRUE(owner.Exist() == false);

		// watch1은 소유권이 이전된 녀석(tempOwner)을 감시한다.
		// 따라서 달라진게 없다.
		EXPECT_TRUE(watch1.WatcherCount() == 2);

		watch2 = tempOwner;

		EXPECT_TRUE(watch2.WatcherCount() == 3);
	}

	// tempOwner가 스택프레임이 해제됨에따라 삭제되었다.
	// 이제 이를 감시중인 watch1과 watch2는 모두 무효화된다.
	// 오너는 사라졌지만 감시는 watch1, watch2가 비어있는 곳을 감시하고있다.
	// 이 두 녀석이 사라질때까지 제어 블록은 생존하고 있는다.
	EXPECT_TRUE(watch1.WatcherCount() == 2);
	EXPECT_TRUE(watch1.Exist() == false);
	EXPECT_TRUE(watch2.Exist() == false);


	// 포인터 타입으로도 만들 수 있다.
	Owner<Model>* owner2 = MakeOwnerPointer<Model>(1000);
	EXPECT_TRUE(owner2->Get()->a == 1000);
	delete owner2;
}


#endif // TEST_PointerObserverTest == ON


