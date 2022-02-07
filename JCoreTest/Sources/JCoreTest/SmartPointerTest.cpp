/*
	�ۼ��� : ������
	����Ʈ ������ �����׽�Ʈ
*/


#include <JCoreTest/CoreTest.h>
#include <JCore/SmartPointer.h>

using namespace JCore;
using namespace std;

#if TEST_SmartPointerTest == ON

// ����ũ ������ �׽�Ʈ
TEST(SmartPointerTest, UniquePointer) {
	UniquePointer<int> g1 = MakeUnique<int>();
	UniquePointer<int> g2 = MakeUnique<int>();
	*g1 = 30;
	*g2 = 40;

	EXPECT_TRUE(*g1 == 30);
	EXPECT_TRUE(*g2 == 40);

	// g1 = g2; ������ ����
}

struct ModelA
{
#if Print == ON
	ModelA() {  cout << "model A constructor called\n";  }
	~ModelA() {  cout << "model A destructor called\n";  }
#endif
};

// ����� ������ �׽�Ʈ
TEST(SmartPointerTest, SharedPointer) {
	SharedPointer<int> g1 = MakeShared<int>(2);
	SharedPointer<int> g2 = MakeShared<int>(3);

	*g1 = 30;

	// ���� g1 �Ҹ�
	g2 = g1;
	EXPECT_TRUE(g2.RefCount() == 2);
	EXPECT_TRUE(g1.RefCount() == 2);

	{
		SharedPointer<int> g3(g2);
		EXPECT_TRUE(g1.RefCount() == 3);
	}

	EXPECT_TRUE(g1.RefCount() == 2);

	SharedPointer<int> g3;
	g3 = g2;

	EXPECT_TRUE(g1.RefCount() == 3);

	SharedPointer<int> g4(Move(g3));	// �̵������� ȣ��
	EXPECT_TRUE(g1.RefCount() == 3);	// ī��Ʈ ���� �Ǿ����


	SharedPointer<int> g5(g4);			// ��������� ȣ��
	EXPECT_TRUE(g1.RefCount() == 4);	// �ö�


	SharedPointer<ModelA> m1 = MakeShared<ModelA>();

	{
		SharedPointer<ModelA> m2(Move(m1));
	}

	// move �ϰ� �� �ڶ� �������
	EXPECT_TRUE(m1.Exist() == false);
	EXPECT_TRUE(m1.Get() == nullptr);

	{
		m1 = MakeShared<ModelA>(); // operator=(&&) ȣ��
	}
	EXPECT_TRUE(m1.Exist());
	EXPECT_TRUE(m1.Get() != nullptr);
	EXPECT_TRUE(m1.RefCount() == 1);
	EXPECT_TRUE(m1.WeakCount() == 1);
}




// ��ũ ������ �׽�Ʈ
TEST(SmartPointerTest, WeakPointer) {

	PrintFormat("[�⺻ �׽�Ʈ]\n");
	{
		WeakPointer<ModelA> w1;
		SharedPointer<ModelA> s1;

		EXPECT_TRUE(w1.RefCount() == 0);
		EXPECT_TRUE(s1.RefCount() == 0);
		EXPECT_TRUE(w1.Exist() == false);
		EXPECT_TRUE(s1.Exist() == false);

		w1 = s1;	// operator(&)

		EXPECT_TRUE(w1.RefCount() == 0);
		EXPECT_TRUE(s1.RefCount() == 0);
		EXPECT_TRUE(w1.Exist() == false);
		EXPECT_TRUE(s1.Exist() == false);

		s1 = MakeShared<ModelA>();	// SharedPointer::operator=(&&) ȣ��
		w1 = s1;

		EXPECT_TRUE(w1.RefCount() == 1);
		EXPECT_TRUE(w1.WeakCount() == 2);
	}

	PrintFormat("===========================================\n");
	PrintFormat("[��ȯ ���� �߻�����]\n");
	// ��ȯ ������ �Ͼ�� ����
	struct ModelC;
	struct ModelB
	{
	#if Print == ON
		ModelB() { cout << "model B constructor called\n"; }
		~ModelB() { cout << "model B destructor called\n"; }
	#endif
		SharedPointer<ModelC> C;
	};

	struct ModelC
	{
	#if Print == ON
		ModelC() { cout << "model C constructor called\n"; }
		~ModelC() { cout << "model C destructor called\n"; }
	#endif
		SharedPointer<ModelB> B;
	};



	// ��ȯ ������ �����ϴ� ����
	struct ModelE;
	struct ModelD
	{
	#if Print == ON
		ModelD() { cout << "model D constructor called\n"; }
		~ModelD() { cout << "model D destructor called\n"; }
	#endif
		WeakPointer<ModelE> E;
	};

	struct ModelE
	{
	#if Print == ON
		ModelE() { cout << "model E constructor called\n"; }
		~ModelE() { cout << "model E destructor called\n"; }
	#endif
		WeakPointer<ModelD> D;
	};


	SharedPointer<ModelB> b1 = MakeShared<ModelB>();
	SharedPointer<ModelC> c1 = MakeShared<ModelC>();

	// ������ ������ �����ϱ� ���� �Ϻη� Move�� ����
	{
		SharedPointer<ModelB> b2(Move(b1));
		SharedPointer<ModelC> c2(Move(c1));

		b2->C = c2;
		c2->B = b2;
	}

	// ��ȯ������ �������� �ʴ´�.
	EXPECT_TRUE(b1.RefCount() == c1.RefCount() == 1);

	PrintFormat("===========================================\n");
	PrintFormat("[��ȯ ���� �����ذ�]\n");
	SharedPointer<ModelD> d1 = MakeShared<ModelD>();
	SharedPointer<ModelE> e1 = MakeShared<ModelE>();
	{
		SharedPointer<ModelD> d2(Move(d1));
		SharedPointer<ModelE> e2(Move(e1));

		d2->E = e2;
		e2->D = d2;
	}

	EXPECT_TRUE(d1.RefCount() == 0);
	EXPECT_TRUE(e1.RefCount() == 0);
	EXPECT_TRUE(d1.Exist() == false);
	EXPECT_TRUE(e1.Exist() == false);

	
}




#endif // TEST_SmartPointerTest == ON
