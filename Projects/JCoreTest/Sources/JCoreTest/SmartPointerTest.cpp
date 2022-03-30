/*
	작성자 : 윤정도
	스마트 포인터 동작테스트
*/


#include <JCoreTest/CoreTest.h>
#include <JCoreTest/TestUtil/Object.h>

#include <JCore/SmartPointer.h>


using namespace JCore;
using namespace std;

#if TEST_SmartPointerTest == ON

// 유니크 포인터 테스트
TEST(SmartPointerTest, UniquePointer) {
	MemoryLeakDetector leak;

	const UniquePointer<int> g1 = MakeUnique<int>();
	const UniquePointer<int> g2 = MakeUnique<int>();
	*g1 = 30;
	*g2 = 40;

	EXPECT_TRUE(*g1 == 30);
	EXPECT_TRUE(*g2 == 40);

	// g1 = g2; 컴파일 에러
}

struct ModelA
{
#if Print == ON
	ModelA() {  PrintFormat("model A constructor called\n");  }
	~ModelA() { PrintFormat("model A destructor called\n");  }
#endif
};

// 쉐어드 포인터 테스트
TEST(SmartPointerTest, SharedPointer) {
	

	SharedPointer<int> g1 = MakeShared<int>(2);
	SharedPointer<int> g2 = MakeShared<int>(3);

	*g1 = 30;

	// 기존 g1 소멸
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

	SharedPointer<int> g4(Move(g3));	// 이동생성자 호출
	EXPECT_TRUE(g1.RefCount() == 3);	// 카운트 유지 되어야함


	SharedPointer<int> g5(g4);			// 복사생성자 호출
	EXPECT_TRUE(g1.RefCount() == 4);	// 올라감


	SharedPointer<ModelA> m1 = MakeShared<ModelA>();

	{
		SharedPointer<ModelA> m2(Move(m1));
	}

	// move 하고 난 뒤라서 없어야함
	EXPECT_TRUE(m1.Exist() == false);
	EXPECT_TRUE(m1.Get() == nullptr);

	{
		m1 = MakeShared<ModelA>(); // operator=(&&) 호출
	}
	EXPECT_TRUE(m1.Exist());
	EXPECT_TRUE(m1.Get() != nullptr);
	EXPECT_TRUE(m1.RefCount() == 1);
	EXPECT_TRUE(m1.WeakCount() == 1);
}


// 워크 포인터 테스트
TEST(SmartPointerTest, WeakPointer) {
	PrintFormat("[기본 테스트]\n");
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

		s1 = MakeShared<ModelA>();	// SharedPointer::operator=(&&) 호출
		w1 = s1;

		EXPECT_TRUE(w1.RefCount() == 1);
		EXPECT_TRUE(w1.WeakCount() == 2);
	}

	PrintFormat("===========================================\n");
	PrintFormat("[순환 참조 발생예시]\n");
	// 순환 참조가 일어나는 구조
	struct ModelC;
	struct ModelB
	{
	#if Print == ON
		ModelB() { PrintFormat("model B constructor called\n"); }
		~ModelB() { PrintFormat("model B destructor called\n"); }
	#endif
		SharedPointer<ModelC> C;
	};

	struct ModelC
	{
	#if Print == ON
		ModelC() { PrintFormat("model C constructor called\n"); }
		~ModelC() { PrintFormat("model C destructor called\n"); }
	#endif
		SharedPointer<ModelB> B;
	};



	// 순환 참조를 방지하는 예시
	struct ModelE;
	struct ModelD
	{
	#if Print == ON
		ModelD() { PrintFormat("model D constructor called\n"); }
		~ModelD() { PrintFormat("model D destructor called\n"); }
	#endif
		WeakPointer<ModelE> E;
	};

	struct ModelE
	{
	#if Print == ON
		ModelE() { PrintFormat("model E constructor called\n"); }
		~ModelE() { PrintFormat("model E destructor called\n"); }
	#endif
		WeakPointer<ModelD> D;
	};


	SharedPointer<ModelB> b1 = MakeShared<ModelB>();
	SharedPointer<ModelC> c1 = MakeShared<ModelC>();

	// 포인터 변경을 관찰하기 위해 일부러 Move로 전달
	{
		SharedPointer<ModelB> b2(Move(b1));
		SharedPointer<ModelC> c2(Move(c1));

		b2->C = c2;
		c2->B = b2;
	}

	// 순환참조로 삭제되지 않는다.
	EXPECT_TRUE(b1.RefCount() == c1.RefCount() == 1);

	PrintFormat("===========================================\n");
	PrintFormat("[순환 참조 문제해결]\n");
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


// 다이나믹 캐스팅 테스트
TEST(SmartPointerTest, DynamicCastingTest) {
	MemoryLeakDetector leak;

	WeakPointer<Model> w1;
	{
		SharedPointer<SuperModel> s1 = MakeShared<SuperModel>();
		
		Model* z = s1.Get<Model*>();		// Get 다이나믹 캐스팅 지원
		SuperModel* z2 = s1.Get();			// 기본타입은 그냥 들고오면 됨

		SharedPointer<Model> s2 = s1;
		SharedPointer<SuperModel> s3 = s2;


		w1 = s1;
	}


}

// 인덱싱 테스트
// 배열처럼 동작가능한지.
TEST(SmartPointerTest, Indexing) {
	MemoryLeakDetector leak;

	constexpr int ArraySize = 3;
	constexpr int Value = 30;
	
	SharedPointer<int> a = MakeShared<int[]>(ArraySize);
	SharedPointer<int> b = MakeShared<int[ArraySize]>();

	UniquePointer<int> d = MakeUnique<int[]>(ArraySize);
	UniquePointer<int> e = MakeUnique<int[ArraySize]>();

	SharedPointer<Model> ma = MakeShared<SuperModel[]>(ArraySize);
	SharedPointer<Model> mb = MakeShared<SuperModel[ArraySize]>();

	UniquePointer<Model> md = MakeUnique<SuperModel[]>(ArraySize);
	UniquePointer<Model> me = MakeUnique<SuperModel[ArraySize]>();

	for (int i = 0; i < ArraySize; i++) {
		// 쉐어드
		a[i] = Value;
		b[i] = Value;

		EXPECT_TRUE(a[i] == Value);
		EXPECT_TRUE(b[i] == Value);

		// 유니크
		d[i] = Value;
		e[i] = Value;

		EXPECT_TRUE(d[i] == Value);
		EXPECT_TRUE(e[i] == Value);

		// 객체 쉐어드
		ma[i].a = Value; ma[i].b = Value;
		mb[i].a = Value; mb[i].b = Value;

		EXPECT_TRUE(ma[i].a == Value && ma[i].b == Value);
		EXPECT_TRUE(mb[i].a == Value && mb[i].b == Value);

		// 유니크 쉐어드
		md[i].a = Value; md[i].b = Value;
		me[i].a = Value; me[i].b = Value;

		EXPECT_TRUE(md[i].a == Value && md[i].b == Value);
		EXPECT_TRUE(me[i].a == Value && me[i].b == Value);
	}

	// 쉐어드
	EXPECT_THROW(a[-1], OutOfRangeException);
	EXPECT_THROW(b[-1], OutOfRangeException);
	EXPECT_THROW(a[ArraySize], OutOfRangeException);
	EXPECT_THROW(b[ArraySize], OutOfRangeException);
	EXPECT_TRUE(a.Length() == 3);
	EXPECT_TRUE(b.Length() == 3);

	// 유니크
	EXPECT_THROW(d[-1], OutOfRangeException);
	EXPECT_THROW(e[-1], OutOfRangeException);
	EXPECT_THROW(d[ArraySize], OutOfRangeException);
	EXPECT_THROW(e[ArraySize], OutOfRangeException);
	EXPECT_TRUE(d.Length() == 3);
	EXPECT_TRUE(e.Length() == 3);

	// 객체 쉐어드
	EXPECT_THROW(ma[-1], OutOfRangeException);
	EXPECT_THROW(mb[-1], OutOfRangeException);
	EXPECT_THROW(ma[ArraySize], OutOfRangeException);
	EXPECT_THROW(mb[ArraySize], OutOfRangeException);
	EXPECT_TRUE(ma.Length() == 3);
	EXPECT_TRUE(mb.Length() == 3);

	// 객체 유니크
	EXPECT_THROW(md[-1], OutOfRangeException);
	EXPECT_THROW(me[-1], OutOfRangeException);
	EXPECT_THROW(md[ArraySize], OutOfRangeException);
	EXPECT_THROW(me[ArraySize], OutOfRangeException);
	EXPECT_TRUE(md.Length() == 3);
	EXPECT_TRUE(me.Length() == 3);



	// 쉐어드 스칼라 인덱싱
	SharedPointer<int> c = MakeShared<int>();
	c[0] = Value;
	EXPECT_TRUE(c[0] == Value);
	EXPECT_TRUE(c.Length() == 1);
	EXPECT_THROW(c[-1], OutOfRangeException);
	EXPECT_THROW(c[-1], OutOfRangeException);
	EXPECT_THROW(c[1], OutOfRangeException);
	EXPECT_THROW(c[1], OutOfRangeException);
	

	// 유니크 스칼라 인덱싱
	UniquePointer<int> f = MakeUnique<int>();
	f[0] = Value;
	EXPECT_TRUE(f[0] == Value);
	EXPECT_TRUE(f.Length() == 1);
	EXPECT_THROW(f[-1], OutOfRangeException);
	EXPECT_THROW(f[-1], OutOfRangeException);
	EXPECT_THROW(f[1], OutOfRangeException);
	EXPECT_THROW(f[1], OutOfRangeException);


	// 객체 쉐어드 스칼라 인덱싱
	SharedPointer<Model> mc = MakeShared<SuperModel>();
	mc[0].a = Value; mc[0].b = Value;
	EXPECT_TRUE(mc[0].a == Value && mc[0].b == Value);
	EXPECT_TRUE(mc.Length() == 1);
	EXPECT_THROW(mc[-1], OutOfRangeException);
	EXPECT_THROW(mc[-1], OutOfRangeException);
	EXPECT_THROW(mc[1], OutOfRangeException);
	EXPECT_THROW(mc[1], OutOfRangeException);


	// 객체 유니크 스칼라 인덱싱
	UniquePointer<Model> mf = MakeUnique<SuperModel>();
	mf[0].a = Value; mf[0].b = Value;
	EXPECT_TRUE(mf[0].a == Value && mf[0].b == Value);
	EXPECT_TRUE(mf.Length() == 1);
	EXPECT_THROW(mf[-1], OutOfRangeException);
	EXPECT_THROW(mf[-1], OutOfRangeException);
	EXPECT_THROW(mf[1], OutOfRangeException);
	EXPECT_THROW(mf[1], OutOfRangeException);
}

#endif // TEST_SmartPointerTest == ON

