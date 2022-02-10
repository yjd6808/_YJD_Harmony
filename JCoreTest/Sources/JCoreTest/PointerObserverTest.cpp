/*
	�ۼ��� : ������
	������ ������ �׽�Ʈ
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


// ���̵� Ÿ���� ������ �׽�Ʈ
TEST(PointerObserverTest, VoidOwner_VoidWatcher) {
	MemoryLeakDetector detector;
	int* pNew = new int(1000);
	VoidOwner owner(pNew);

	*owner.Get<int>() = 300;			// �� ����
	void* pVoidNew = owner.GetRaw();

	EXPECT_TRUE(*pNew == 300);
	EXPECT_TRUE(pNew == pVoidNew);
	
	// ���ʴ� �������̱⵵ �ϴ�. ���� �ʱ� 1�� ����
	EXPECT_TRUE(owner.WatcherCount() == 1);

	VoidWatcher watch1(owner);
	VoidWatcher watch2;

	// watch1�� owner�� �����Ѵ�.
	EXPECT_TRUE(watch1.WatcherCount() == 2);

	// watch2�� �ƹ��͵� �����ϰ� ���� �ʴ´�.
	EXPECT_TRUE(watch2.WatcherCount() == 0);
	EXPECT_TRUE(watch2.Exist() == false);

	{
		// ������ �������� �ٸ� ���ʿ��� �����Ѵ�.
		VoidOwner tempOwner(Move(owner));	

		// ���� ���ʴ� ���� ��ȿ���� �ʴ�.
		EXPECT_TRUE(owner.Exist() == false);

		// watch1�� �������� ������ �༮(tempOwner)�� �����Ѵ�.
		// ���� �޶����� ����.
		EXPECT_TRUE(watch1.WatcherCount() == 2);
		
		watch2 = tempOwner;

		EXPECT_TRUE(watch2.WatcherCount() == 3);
	}

	// tempOwner�� ������������ �����ʿ����� �����Ǿ���.
	// ���� �̸� �������� watch1�� watch2�� ��� ��ȿȭ�ȴ�.
	// ���ʴ� ��������� ���ô� watch1, watch2�� ����ִ� ���� �����ϰ��ִ�.
	// �� �� �༮�� ����������� ���� ����� �����ϰ� �ִ´�.
	EXPECT_TRUE(watch1.WatcherCount() == 2);	
	EXPECT_TRUE(watch1.Exist() == false);
	EXPECT_TRUE(watch2.Exist() == false);

}


struct Model
{
	Model() { PrintFormat("����Ʈ �� 1ȣ ����\n"); }
	Model(int da) { a = da; PrintFormat("�� 1ȣ ����\n"); }
	virtual ~Model() { PrintFormat("�� 1ȣ �Ҹ�\n"); }

	int a = 3;
	int b = 3;
};


struct SuperModel : Model
{
	SuperModel() : Model(1) {}
	~SuperModel() override { PrintFormat("���� �� �Ҹ�\n"); }
};



// Ÿ���� ������ ����
TEST(PointerObserverTest, Owner_Watcher) {
	MemoryLeakDetector detector;
	Owner<Model> owner = MakeOwner<Model>(1000);

	// ���� ���� ������ �����ε�
	(*owner).a = 500;
	(*owner).b = 300;

	// ȭ��ǥ �����ε�
	EXPECT_TRUE(owner->a == 500 && owner->b == 300);

	// ���ʴ� �������̱⵵ �ϴ�. ���� �ʱ� 1�� ����
	EXPECT_TRUE(owner.WatcherCount() == 1);

	Watcher<Model> watch1(owner);
	Watcher<Model> watch2;

	// watch1�� owner�� �����Ѵ�.
	EXPECT_TRUE(watch1.WatcherCount() == 2);

	// watch2�� �ƹ��͵� �����ϰ� ���� �ʴ´�.
	EXPECT_TRUE(watch2.WatcherCount() == 0);
	EXPECT_TRUE(watch2.Exist() == false);

	{
		// ������ �������� �ٸ� ���ʿ��� �����Ѵ�.
		Owner<Model> tempOwner(Move(owner));

		// ���� ���ʴ� ���� ��ȿ���� �ʴ�.
		EXPECT_TRUE(owner.Exist() == false);

		// watch1�� �������� ������ �༮(tempOwner)�� �����Ѵ�.
		// ���� �޶����� ����.
		EXPECT_TRUE(watch1.WatcherCount() == 2);

		watch2 = tempOwner;

		EXPECT_TRUE(watch2.WatcherCount() == 3);
	}

	// tempOwner�� ������������ �����ʿ����� �����Ǿ���.
	// ���� �̸� �������� watch1�� watch2�� ��� ��ȿȭ�ȴ�.
	// ���ʴ� ��������� ���ô� watch1, watch2�� ����ִ� ���� �����ϰ��ִ�.
	// �� �� �༮�� ����������� ���� ����� �����ϰ� �ִ´�.
	EXPECT_TRUE(watch1.WatcherCount() == 2);
	EXPECT_TRUE(watch1.Exist() == false);
	EXPECT_TRUE(watch2.Exist() == false);


	// ������ Ÿ�����ε� ���� �� �ִ�.
	Owner<Model>* owner2 = MakeOwnerPointer<Model>(1000);
	EXPECT_TRUE(owner2->Get()->a == 1000);
	delete owner2;
}


struct Animal
{
	virtual int GetAge() const = 0;

	Animal() { PrintFormat("�ִϸ� ����\n"); }
	virtual ~Animal() { PrintFormat("�ִϸ� �Ҹ�\n"); }
};

struct Dog : Animal
{
	int GetAge() const override { return Age; };

	virtual ~Dog() { PrintFormat("�� �Ҹ�\n"); }

	int Age = 300;
};

struct Bird : Animal
{
	int GetAge() const override { return Age; };

	virtual ~Bird() { PrintFormat("���� �Ҹ�\n"); }

	int Dummy = 0;
	int Age = 400;
};

// ���̳��� ĳ����
TEST(PointerObserverTest, DynamicCastingTest) {
	MemoryLeakDetector detector;
	
	{
		PrintFormat("[Owner �̵� ������ Test]\n");
		Owner<Animal> a1 = MakeOwner<Dog>();
		Owner<Dog> d1(Move(a1));				// Owner Constructor&& 

		EXPECT_TRUE(a1.Exist() == false);		// �������� �Ҹ�
		EXPECT_TRUE(d1.Exist() == true);
		EXPECT_TRUE(d1.WatcherCount() == 1);

		PrintFormat("[Owner �̵� ������ �׽�Ʈ]\n");
		Owner<Animal> a2 = MakeOwner<Bird>();
		Owner<Model> model = MakeOwner<SuperModel>();
		a2 = Move(d1);							//  Owner operaetor=&&
		//model = Move(d1);						// static_assert  dynamic casting ���� [�ּ� ������]

		EXPECT_TRUE(d1.Exist() == false);
		EXPECT_TRUE(a2.Exist() == true);		
		EXPECT_TRUE(a2.WatcherCount() == 1);	


		PrintFormat("[Watcher ���� ������ �׽�Ʈ]\n");
		Watcher<SuperModel> w1 = model;			// Watcher Constructor Owner&	+ �ٿ� ĳ����
		Watcher<Model>		w2 = w1;			// Watcher Constructor Watcher&	+ �� ĳ����
		Watcher<Model>		w3 = w2;			// Watcher Constructor Watcher&	+ ���� ĳ����
		

		EXPECT_TRUE(w3.WatcherCount() == 4);

		PrintFormat("[Watcher �̵� ������ �׽�Ʈ]\n");
		Watcher<SuperModel>	w4 = Move(w3);		// Watcher Constructor Watcher&& + �ٿ� ĳ����
		Watcher<Model>	w5	   = Move(w4);		// Watcher Constructor Watcher&& + �� ĳ����

		EXPECT_TRUE(w3.Exist() == false);
		EXPECT_TRUE(w4.Exist() == false);
		EXPECT_TRUE(w5.WatcherCount() == 4);


		PrintFormat("[Watcher ���� ������ �׽�Ʈ]\n");
		Watcher<Model> w6		= w5;			// Watcher Operator Watcher&	+ ���� ĳ����
		Watcher<SuperModel> w7	= w6;			// Watcher Operator Watcher&	+ �ٿ� ĳ����
		Watcher<Model> w8  = w7;				// Watcher Operator Watcher&	+ �� ĳ����
		EXPECT_TRUE(w5.WatcherCount() == 7);
		EXPECT_TRUE(w6.WatcherCount() == 7);
		EXPECT_TRUE(w7.WatcherCount() == 7);
		EXPECT_TRUE(w8.WatcherCount() == 7);

		Owner<Model> model2 = MakeOwner<SuperModel>();
		
		w8 = model2;							// Watcher Operator Owner&		+ ���� ĳ����

		EXPECT_TRUE(w7.WatcherCount() == 6);
		EXPECT_TRUE(w8.WatcherCount() == 2);

		Owner<SuperModel> model3 = Move(model2);
		EXPECT_TRUE(model2.WatcherCount() == 0);
		EXPECT_TRUE(model2.Exist() == false);
		EXPECT_TRUE(w8.WatcherCount() == 2);
	}


	{
		PrintFormat("[���̳��� ĳ���� ���� �׽�Ʈ]\n");
		Owner<Model> model = MakeOwner<Model>();

		// Owner �̵� �����ڿ��� ���̳��� ĳ���� ���н�
		EXPECT_THROW(Owner<SuperModel>(Move(model)), NullPointerException);	// ���ʿ� Model�� �༮�� SuperModel�� �ٿ� ĳ���� �Ұ���

		Owner<Model> model2 = MakeOwner<Model>();
		Owner<SuperModel> supermodel = MakeOwner<SuperModel>();

		// Owner �̵� �����ڿ��� ���̳��� ĳ���� ���н�
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


