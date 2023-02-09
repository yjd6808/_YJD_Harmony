/*
 * 작성자: 윤정도
 * 생성일: 12/8/2022 5:32:07 PM
 * =====================
 * 원자성 테스트는 다른 곳에서 많이 하기땜에 굳이 여기서 안해도 됨
 */



#include <JCoreTest/CoreTest.h>
#include <JCore/Primitives/Atomic.h>

using namespace JCore;

#if TEST_AtomicTest == ON

TEST(AtomicTest, Integer) {

    {
        Atomic<int> s;
        s.Store(100);
        EXPECT_TRUE(s.Load() == 100);
        if (s.TryCompareExchange(100, 2000))
            EXPECT_TRUE(s == 2000);

        EXPECT_TRUE(s.Exchange(3000) == 2000);
        EXPECT_TRUE(s == 3000);
        EXPECT_TRUE(s.Add(2500) == 5500);
        EXPECT_TRUE(s.Add(-2500) == 3000);
    }

    {
        Atomic<int> s;
        EXPECT_TRUE(s == 0);
        EXPECT_TRUE(s++ == 0);
        EXPECT_TRUE(s == 1);
    }

    {
        Atomic<int> s;
        EXPECT_TRUE(s == 0);
        EXPECT_TRUE(++s == 1);
        EXPECT_TRUE(s == 1);
    }

    {
        Atomic<int> s = 1;
        EXPECT_TRUE(s == 1);
        EXPECT_TRUE(--s == 0);
        EXPECT_TRUE(s == 0);
    }

    {
        Atomic<Int32U> k = 1;
        Atomic<Int> s = k;
        EXPECT_TRUE(s == 1);
        EXPECT_TRUE(s-- == 1);
        EXPECT_TRUE(s == 0);
    }

    {
        Atomic<Int> s = 0;
        EXPECT_TRUE(s += 100 == 100);
        EXPECT_TRUE(s -= 100 == 0);
    }

    {
        Atomic<Int> s = 0;
        EXPECT_TRUE((s |= 0b0101) == 0b0101);
        EXPECT_TRUE((s |= 0b0010) == 0b0111);
        EXPECT_TRUE((s |= 0b1000) == 0b1111);

        EXPECT_TRUE((s & 0b1010) == 0b1010);
        EXPECT_TRUE((s | 0b1000) == 0b1111);
        EXPECT_TRUE((s ^ 0b1010) == 0b0101);

        s = 0b1111;
        EXPECT_TRUE((s &= 0b1110) == 0b1110);
        EXPECT_TRUE((s &= 0b1101) == 0b1100);
        EXPECT_TRUE((s &= 0b1011) == 0b1000);

        s = 0b1111;
        EXPECT_TRUE((s ^= 0b1010) == 0b0101);
        EXPECT_TRUE((s ^= 0b1010) == 0b1111);
    }
}

TEST(AtomicTest, bool) {
    Atomic<bool> b;
    bool k = b.Load();
    EXPECT_FALSE(k);

    b = true;
    k = b;

    EXPECT_TRUE(k);

    b.Exchange(false);
    EXPECT_FALSE(b.Load());

    b.Exchange(true);
    EXPECT_TRUE(b.Load());

    b = false;
    b.Store(true);
    EXPECT_TRUE(b.Load());

    bool expected = false;
    EXPECT_FALSE(b.CompareExchange(expected, true)); // 교환 실패
    EXPECT_TRUE(expected);  // 실패하면 기존값 true가 들어와야함

    EXPECT_TRUE(b.CompareExchange(expected, false)); // 교환 성공
    EXPECT_FALSE(b.Load());  //false

    EXPECT_TRUE(b == false);
    EXPECT_TRUE(b != true);
    EXPECT_TRUE(!b == true);
}


TEST(AtomicTest, Pointer) {
    int* orignal = new int{5};
    void* base = orignal;

    Atomic<void*> a1(orignal);

    EXPECT_TRUE(a1 == base);
    EXPECT_TRUE(a1 == orignal);
    EXPECT_TRUE(a1.Load() == orignal);
    EXPECT_TRUE(a1.Load() == orignal);

    EXPECT_FALSE(a1 != base);
    EXPECT_FALSE(a1 != orignal);
    EXPECT_FALSE(a1.Load() != orignal);
    EXPECT_FALSE(a1.Load() != orignal);

    a1.Exchange(orignal + 4);
    EXPECT_TRUE(a1 == orignal + 4);

    a1 = nullptr;
    EXPECT_TRUE(a1 == nullptr);
    EXPECT_TRUE(a1.Load() == nullptr);

    a1 = orignal;
    EXPECT_TRUE(a1 == base);

    a1.Store(nullptr);
    EXPECT_TRUE(a1 == nullptr);

    delete orignal;
}


#endif