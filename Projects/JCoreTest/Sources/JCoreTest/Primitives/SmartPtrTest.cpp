/*
	작성자 : 윤정도
	스마트 포인터 동작테스트
*/


#include <JCoreTest/CoreTest.h>


#include <JCore/Primitives/SmartPtr.h>



using namespace std;

#if TEST_SmartPtrTest == ON

// 유니크 포인터 테스트
TEST(SmartPointerTest, UniquePointer) {
	AutoMemoryLeakDetector leak;

	const UniquePtr<int> g1 = MakeUnique<int>();
	const UniquePtr<int> g2 = MakeUnique<int>();
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
    auto test = [](SharedPtr<int>& g1, SharedPtr<int>& g2) {
        *g1 = 30;

        // 기존 g1 소멸
        g2 = g1;
        EXPECT_TRUE(g2.RefCount() == 2);
        EXPECT_TRUE(g1.RefCount() == 2);

        {
            SharedPtr<int> g3(g2);
            EXPECT_TRUE(g1.RefCount() == 3);
        }

        EXPECT_TRUE(g1.RefCount() == 2);

        SharedPtr<int> g3;
        g3 = g2;

        EXPECT_TRUE(g1.RefCount() == 3);

        SharedPtr<int> g4(Move(g3));	// 이동생성자 호출
        EXPECT_TRUE(g1.RefCount() == 3);	// 카운트 유지 되어야함


        SharedPtr<int> g5(g4);			// 복사생성자 호출
        EXPECT_TRUE(g1.RefCount() == 4);	// 올라감


        SharedPtr<ModelA> m1 = MakeShared<ModelA>();

        {
            SharedPtr<ModelA> m2(Move(m1));
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
    };


    // 내부에서 만드는 경우
    {
        SharedPtr<int> g1 = MakeShared<int>(2);
        SharedPtr<int> g2 = MakeShared<int>(3);
        test(g1, g2);
    }

    // 외부에서 주입 해주는 경우
    {


        //SharedPtr<int> g1 = SharedPtr<int>(new int(2));
        //SharedPtr<int> g2 = SharedPtr<int>(new int(3));
        //test(g1, g2);
    }
}


// 워크 포인터 테스트
TEST(SmartPointerTest, WeakPointer) {
	PrintFormat("[기본 테스트]\n");
	{
		WeakPtr<ModelA> w1;
		SharedPtr<ModelA> s1;

		EXPECT_TRUE(w1.RefCount() == 0);
		EXPECT_TRUE(s1.RefCount() == 0);
		EXPECT_TRUE(w1.Exist() == false);
		EXPECT_TRUE(s1.Exist() == false);
		w1 = s1;	// operator(&)

		EXPECT_TRUE(w1.RefCount() == 0);
		EXPECT_TRUE(s1.RefCount() == 0);
		EXPECT_TRUE(w1.Exist() == false);
		EXPECT_TRUE(s1.Exist() == false);

        // 내부에서 생성하는 경우
	    {
            s1 = MakeShared<ModelA>();	// SharedPointer::operator=(&&) 호출
            w1 = s1;

            EXPECT_TRUE(w1.RefCount() == 1);
            EXPECT_TRUE(w1.WeakCount() == 2);
        }
        // 외부에서 주입 해주는 경우
		{
		    s1 = SharedPtr<ModelA>(new ModelA());
            w1 = s1;

            EXPECT_TRUE(w1.RefCount() == 1);
            EXPECT_TRUE(w1.WeakCount() == 2);
		}
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
		SharedPtr<ModelC> C;
	};

	struct ModelC
	{
	#if Print == ON
		ModelC() { PrintFormat("model C constructor called\n"); }
		~ModelC() { PrintFormat("model C destructor called\n"); }
	#endif
		SharedPtr<ModelB> B;
	};



	// 순환 참조를 방지하는 예시
	struct ModelE;
	struct ModelD
	{
	#if Print == ON
		ModelD() { PrintFormat("model D constructor called\n"); }
		~ModelD() { PrintFormat("model D destructor called\n"); }
	#endif
		WeakPtr<ModelE> E;
	};

	struct ModelE
	{
	#if Print == ON
		ModelE() { PrintFormat("model E constructor called\n"); }
		~ModelE() { PrintFormat("model E destructor called\n"); }
	#endif
		WeakPtr<ModelD> D;
	};


	SharedPtr<ModelB> b1 = MakeShared<ModelB>();
	SharedPtr<ModelC> c1 = MakeShared<ModelC>();

	// 포인터 변경을 관찰하기 위해 일부러 Move로 전달
	{
		SharedPtr<ModelB> b2(Move(b1));
		SharedPtr<ModelC> c2(Move(c1));

		b2->C = c2;
		c2->B = b2;
	}

	// 순환참조로 삭제되지 않는다.
	EXPECT_TRUE(b1.RefCount() == c1.RefCount() == 1);

	PrintFormat("===========================================\n");
	PrintFormat("[순환 참조 문제해결]\n");
	SharedPtr<ModelD> d1 = MakeShared<ModelD>();
	SharedPtr<ModelE> e1 = MakeShared<ModelE>();
	{
		SharedPtr<ModelD> d2(Move(d1));
		SharedPtr<ModelE> e2(Move(e1));

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
	AutoMemoryLeakDetector leak;

	WeakPtr<Model> w1;
	{

        // 내부에서 만드는 경우
        {
            SharedPtr<SuperModel> s1 = MakeShared<SuperModel>();

            Model* z = s1.Get<Model*>();		// Get 다이나믹 캐스팅 지원
            SuperModel* z2 = s1.Get();			// 기본타입은 그냥 들고오면 됨

            SharedPtr<Model> s2 = s1;
            SharedPtr<SuperModel> s3 = s2;
            w1 = s1;
        }


        // 외부에서 주입 해주는 경우
        {
            SharedPtr<SuperModel> s1 = SharedPtr(new SuperModel());

            Model* z = s1.Get<Model*>();		// Get 다이나믹 캐스팅 지원
            SuperModel* z2 = s1.Get();			// 기본타입은 그냥 들고오면 됨

            SharedPtr<Model> s2 = s1;
            SharedPtr<SuperModel> s3 = s2;
            w1 = s1;
        }
	}


}

// 인덱싱 테스트
// 배열처럼 동작가능한지.
TEST(SmartPointerTest, Indexing) {
	AutoMemoryLeakDetector leak;

	constexpr int ArraySize = 3;
	constexpr int Value = 30;
	
	SharedPtr<int> a = MakeShared<int[]>(ArraySize);
	SharedPtr<int> b = MakeShared<int[ArraySize]>();

	UniquePtr<int> d = MakeUnique<int[]>(ArraySize);
	UniquePtr<int> e = MakeUnique<int[ArraySize]>();

	SharedPtr<Model> ma = MakeShared<SuperModel[]>(ArraySize);
	SharedPtr<Model> mb = MakeShared<SuperModel[ArraySize]>();

	UniquePtr<Model> md = MakeUnique<SuperModel[]>(ArraySize);
	UniquePtr<Model> me = MakeUnique<SuperModel[ArraySize]>();

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
	SharedPtr<int> c = MakeShared<int>();
	c[0] = Value;
	EXPECT_TRUE(c[0] == Value);
	EXPECT_TRUE(c.Length() == 1);
	EXPECT_THROW(c[-1], OutOfRangeException);
	EXPECT_THROW(c[-1], OutOfRangeException);
	EXPECT_THROW(c[1], OutOfRangeException);
	EXPECT_THROW(c[1], OutOfRangeException);
	

	// 유니크 스칼라 인덱싱
	UniquePtr<int> f = MakeUnique<int>();
	f[0] = Value;
	EXPECT_TRUE(f[0] == Value);
	EXPECT_TRUE(f.Length() == 1);
	EXPECT_THROW(f[-1], OutOfRangeException);
	EXPECT_THROW(f[-1], OutOfRangeException);
	EXPECT_THROW(f[1], OutOfRangeException);
	EXPECT_THROW(f[1], OutOfRangeException);


	// 객체 쉐어드 스칼라 인덱싱
	SharedPtr<Model> mc = MakeShared<SuperModel>();
	mc[0].a = Value; mc[0].b = Value;
	EXPECT_TRUE(mc[0].a == Value && mc[0].b == Value);
	EXPECT_TRUE(mc.Length() == 1);
	EXPECT_THROW(mc[-1], OutOfRangeException);
	EXPECT_THROW(mc[-1], OutOfRangeException);
	EXPECT_THROW(mc[1], OutOfRangeException);
	EXPECT_THROW(mc[1], OutOfRangeException);


	// 객체 유니크 스칼라 인덱싱
	UniquePtr<Model> mf = MakeUnique<SuperModel>();
	mf[0].a = Value; mf[0].b = Value;
	EXPECT_TRUE(mf[0].a == Value && mf[0].b == Value);
	EXPECT_TRUE(mf.Length() == 1);
	EXPECT_THROW(mf[-1], OutOfRangeException);
	EXPECT_THROW(mf[-1], OutOfRangeException);
	EXPECT_THROW(mf[1], OutOfRangeException);
	EXPECT_THROW(mf[1], OutOfRangeException);
}

#endif // TEST_SmartPointerTest == ON

