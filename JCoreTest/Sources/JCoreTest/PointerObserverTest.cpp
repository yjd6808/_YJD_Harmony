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

namespace PointerObserverTest {


// 보이드 타입의 포인터 테스트
TEST(PointerObserverTest, VoidOwner_VoidWatcher) {
	MemoryLeakDetector detector;
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
	Model() { PrintFormat("디폴트 모델 1호 생성\n"); }
	Model(int da) { a = da; PrintFormat("모델 1호 생성\n"); }
	virtual ~Model() { PrintFormat("모델 1호 소멸\n"); }

	int a = 3;
	int b = 3;
};


struct SuperModel : Model
{
	SuperModel() : Model(1) {}
	~SuperModel() override { PrintFormat("슈퍼 모델 소멸\n"); }
};



// 타입을 가지는 오너
TEST(PointerObserverTest, Owner_Watcher) {
	MemoryLeakDetector detector;
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


struct Animal
{
	virtual int GetAge() const = 0;

	Animal() { PrintFormat("애니멀 생성\n"); }
	virtual ~Animal() { PrintFormat("애니멀 소멸\n"); }
};

struct Dog : Animal
{
	int GetAge() const override { return Age; };

	virtual ~Dog() { PrintFormat("개 소멸\n"); }

	int Age = 300;
};

struct Bird : Animal
{
	int GetAge() const override { return Age; };

	virtual ~Bird() { PrintFormat("날개 소멸\n"); }

	int Dummy = 0;
	int Age = 400;
};

// 다이나믹 캐스팅
TEST(PointerObserverTest, DynamicCastingTest) {
	MemoryLeakDetector detector;
	
	{
		PrintFormat("[Owner 이동 생성자 Test]\n");
		Owner<Animal> a1 = MakeOwner<Dog>();
		Owner<Dog> d1(Move(a1));				// Owner Constructor&& 

		EXPECT_TRUE(a1.Exist() == false);		// 기존꺼는 소멸
		EXPECT_TRUE(d1.Exist() == true);
		EXPECT_TRUE(d1.WatcherCount() == 1);

		PrintFormat("[Owner 이동 연산자 테스트]\n");
		Owner<Animal> a2 = MakeOwner<Bird>();
		Owner<Model> model = MakeOwner<SuperModel>();
		a2 = Move(d1);							//  Owner operaetor=&&
		//model = Move(d1);						// static_assert  dynamic casting 실패 [주석 해제시]

		EXPECT_TRUE(d1.Exist() == false);
		EXPECT_TRUE(a2.Exist() == true);		
		EXPECT_TRUE(a2.WatcherCount() == 1);	


		PrintFormat("[Watcher 복사 생성자 테스트]\n");
		Watcher<SuperModel> w1 = model;			// Watcher Constructor Owner&	+ 다운 캐스팅
		Watcher<Model>		w2 = w1;			// Watcher Constructor Watcher&	+ 업 캐스팅
		Watcher<Model>		w3 = w2;			// Watcher Constructor Watcher&	+ 동등 캐스팅
		

		EXPECT_TRUE(w3.WatcherCount() == 4);

		PrintFormat("[Watcher 이동 생성자 테스트]\n");
		Watcher<SuperModel>	w4 = Move(w3);		// Watcher Constructor Watcher&& + 다운 캐스팅
		Watcher<Model>	w5	   = Move(w4);		// Watcher Constructor Watcher&& + 업 캐스팅

		EXPECT_TRUE(w3.Exist() == false);
		EXPECT_TRUE(w4.Exist() == false);
		EXPECT_TRUE(w5.WatcherCount() == 4);


		PrintFormat("[Watcher 복사 연산자 테스트]\n");
		Watcher<Model> w6		= w5;			// Watcher Operator Watcher&	+ 동등 캐스팅
		Watcher<SuperModel> w7	= w6;			// Watcher Operator Watcher&	+ 다운 캐스팅
		Watcher<Model> w8  = w7;				// Watcher Operator Watcher&	+ 업 캐스팅
		EXPECT_TRUE(w5.WatcherCount() == 7);
		EXPECT_TRUE(w6.WatcherCount() == 7);
		EXPECT_TRUE(w7.WatcherCount() == 7);
		EXPECT_TRUE(w8.WatcherCount() == 7);

		Owner<Model> model2 = MakeOwner<SuperModel>();
		
		w8 = model2;							// Watcher Operator Owner&		+ 동등 캐스팅

		EXPECT_TRUE(w7.WatcherCount() == 6);
		EXPECT_TRUE(w8.WatcherCount() == 2);

		Owner<SuperModel> model3 = Move(model2);
		EXPECT_TRUE(model2.WatcherCount() == 0);
		EXPECT_TRUE(model2.Exist() == false);
		EXPECT_TRUE(w8.WatcherCount() == 2);
	}


	{
		PrintFormat("[다이나믹 캐스팅 실패 테스트]\n");
		Owner<Model> model = MakeOwner<Model>();

		// Owner 이동 생성자에서 다이나믹 캐스팅 실패시
		EXPECT_THROW(Owner<SuperModel>(Move(model)), NullPointerException);	// 애초에 Model인 녀석은 SuperModel로 다운 캐스팅 불가능

		Owner<Model> model2 = MakeOwner<Model>();
		Owner<SuperModel> supermodel = MakeOwner<SuperModel>();

		// Owner 이동 연산자에서 다이나믹 캐스팅 실패시
		try {
			supermodel = (Move(model2));
			FAIL();
		} catch (...) {
		}

		int a = 30;
	}
}


} // namespace PointerObserverTest

#endif // TEST_PointerObserverTest == ON


