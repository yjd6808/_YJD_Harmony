
#include <JCoreTest/CoreTest.h>
#include <JCore/Memory.h>



#if TEST_MemoryTest == ON

// 어셈블리어 약간 복습
TEST(MemoryTest, AssemblyTest) {

	int k = 5;

#if defined(_X64)
	// mov 연산자 테스트
	// 3가지 방법으로 바이트단위 복사를 해봄
	int a = 0x62636465;
	char c[4];
	__asm {
		mov		al,				byte ptr[a]
		mov		byte ptr[c],	al					; c[0] = 0x65

		mov		al,				byte ptr[a + 1]
		mov		byte ptr[c + 1],al					; c[1] = 0x64

		lea		eax,			[a]					; eax = &a
		mov		bl,				byte ptr[eax + 2]
		mov		byte ptr[c + 2],bl					; c[2] = 0x63

		add		eax,			3					; eax = eax + 3
		mov		bl,				byte ptr[eax]		; bl = *eax
		mov		byte ptr[c + 3],bl					; c[3] = 0x62
	}


	EXPECT_TRUE(c[0] == 0x65);
	EXPECT_TRUE(c[1] == 0x64);
	EXPECT_TRUE(c[2] == 0x63);
	EXPECT_TRUE(c[3] == 0x62);


	/* =============================================================== */

	// add 연산자 테스트
	char* d = (char*) & a;
	__asm {
		add		d,				1					; d = d + 1;
	}
	EXPECT_TRUE(*d == 0x64);
	__asm {
		add		d,				1
	}
	EXPECT_TRUE(*d == 0x63);
#endif
}


TEST(MemoryTest, MemoryCopy) {

	int* c = new int[4];
	const int d[4] = { 0, 1, 2, 3};

	Memory::Copy(c, sizeof(int) * 4, d, sizeof(d));

	EXPECT_TRUE(c[0] == 0);
	EXPECT_TRUE(c[1] == 1);
	EXPECT_TRUE(c[2] == 2);
	EXPECT_TRUE(c[3] == 3);
}

struct TestStruct {
	int a;
	int b;
	int c;

	int Val() { return a + b + c; }
	TestStruct() :
		  a(5)
		, b(5)
		, c(5) {
	}

	TestStruct(int d) :
		a(d),
		b(d),
		c(d) {
	}
};


TEST(MemoryTest, MemorySet) {
	const int kiArraySize = 32;
	TestStruct* c = new TestStruct[kiArraySize];
	TestStruct d(10);

	Memory::Set(c, sizeof(TestStruct) * kiArraySize, 0);

	for (int i = 0; i < kiArraySize; i++) {
		EXPECT_TRUE(c[i].Val() == 0);
	}
}


TEST(MemoryTest, MemorySetT) {

	constexpr int kiArraySize = 32;
	TestStruct* c = new TestStruct[kiArraySize];
	const TestStruct d(10);

	Memory::Set<TestStruct>(c, kiArraySize, d);

	for (int i = 0; i < kiArraySize; i++) {
		EXPECT_TRUE(c[i].Val() == 30);
	}
}




#endif // TEST_MemoryTest == ON