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

// ���̵� Ÿ���� ������ �׽�Ʈ
TEST(PointerObserverTest, VoidOwner_VoidWatcher) {
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
	Model() { PrintFormat("����Ʈ ������ ȣ��\n"); }
	Model(int da) { a = da; PrintFormat("�Ϲ� ������ ȣ��\n"); }
	~Model() { PrintFormat("�Ҹ��� ȣ��\n"); }

	int a = 3;
	int b = 3;
};



// Ÿ���� ������ ����
TEST(PointerObserverTest, Owner_Watcher) {
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


#endif // TEST_PointerObserverTest == ON


