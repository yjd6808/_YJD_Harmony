/*
 * 작성자: 윤정도
 * 생성일: 1/7/2023 6:37:50 AM
 * =====================
 *
 */



#include <JCoreTest/CoreTest.h>
#include <JCore/Wrapper/CRuntime.h>

#if TEST_CRuntimeTest == ON

TEST(CRuntimeTest, FileIOTest) {
	IoHandle hHandle = CRuntime::FileOpen("a.bin", "wb+");

	EXPECT_EQ(CRuntime::FileTell(hHandle), 0);

	Byte wbuff[] {
		1, 2, 3, 4, 5, 6, 7, 8, 9, 10
	};

	// 4바이트 크기를 2번 쓴다.
	int size =  CRuntime::FileWrite(wbuff, 4, 2, hHandle);
	EXPECT_EQ(size, 2);
	EXPECT_EQ(CRuntime::FileTell(hHandle), 8);

	Byte rbuff[100];

	// wb+로 열어서 그런가 끝에서 읽으면 0바이트를 읽네
	// 커서위치를 한번도 안바꾼 상태면 끝자락에서 데이터가 읽어지고 읽은 만큼 0을 쓴다.
	EXPECT_EQ(CRuntime::FileRead(rbuff, 2, 10, hHandle), 10);
	EXPECT_EQ(CRuntime::FileTell(hHandle), 28);


	EXPECT_TRUE(CRuntime::FileHasError(hHandle) == false);
	EXPECT_TRUE(CRuntime::FileSeekCur(hHandle, 1));	// 
	EXPECT_TRUE(CRuntime::FileHasError(hHandle) == false);

	EXPECT_EQ(CRuntime::FileTell(hHandle), 29);
	EXPECT_TRUE(CRuntime::FileHasError(hHandle) == false);

	EXPECT_TRUE(CRuntime::FileSeekCur(hHandle, -1));
	EXPECT_EQ(CRuntime::FileTell(hHandle), 28);
	EXPECT_TRUE(CRuntime::FileHasError(hHandle) == false);

	EXPECT_TRUE(CRuntime::FileSeekCur(hHandle, -1));
	EXPECT_EQ(CRuntime::FileTell(hHandle), 27);
	EXPECT_TRUE(CRuntime::FileHasError(hHandle) == false);

	EXPECT_TRUE(CRuntime::FileSeekBegin(hHandle, 0));
	EXPECT_EQ(CRuntime::FileTell(hHandle), 0);
	EXPECT_TRUE(CRuntime::FileHasError(hHandle) == false);

	Memory::Set(rbuff, 100, 0);
	EXPECT_EQ(CRuntime::FileRead(rbuff, 1, 10, hHandle), 10);
	EXPECT_EQ(CRuntime::FileTell(hHandle), 10);

	EXPECT_EQ(CRuntime::FileRead(rbuff, 10, 1, hHandle), 1);
	EXPECT_EQ(CRuntime::FileTell(hHandle), 20);

	// 커서 위치가 움직인 이후부터는 커서를 끝자락에 두더라도 더이상 읽는걸로 쓰는게 불가능하다.
	Memory::Set(rbuff, 100, 0);
	EXPECT_TRUE(CRuntime::FileSeekEnd(hHandle, 0));
	EXPECT_EQ(CRuntime::FileTell(hHandle), 28);
	EXPECT_TRUE(CRuntime::FileHasError(hHandle) == false);
	EXPECT_TRUE(CRuntime::FileEOF(hHandle) == false);
	EXPECT_EQ(CRuntime::FileWrite(wbuff, 10, 1, hHandle), 1);
	EXPECT_EQ(CRuntime::FileTell(hHandle), 38);

	EXPECT_TRUE(CRuntime::FileSeekBegin(hHandle, 30));
	EXPECT_EQ(CRuntime::FileTell(hHandle), 30);
	EXPECT_EQ(CRuntime::FileRead(rbuff, 10, 1, hHandle), 0);	
	EXPECT_EQ(CRuntime::FileTell(hHandle), 38);	// 30위치에서 10바이트를 읽었지만 38위치가 된다.

	EXPECT_TRUE(CRuntime::FileClose(hHandle));
	


	hHandle = CRuntime::FileOpen("a.bin", "ab+");
	EXPECT_EQ(CRuntime::FileTell(hHandle), 0);		// append 모드로 열었더라도 한번이라도 Write 하기전까지는 0임.
	size = CRuntime::FileWrite(wbuff, 4, 2, hHandle);
	EXPECT_EQ(CRuntime::FileTell(hHandle), 46);

	CRuntime::FileClose(hHandle);

	EXPECT_TRUE(CRuntime::FileRename("a.bin", "b.bin"));

	EXPECT_TRUE(CRuntime::FileDelete("b.bin"));
}


#endif
