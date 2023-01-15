/*
 * 작성자: 윤정도
 * 생성일: 1/7/2023 8:02:23 AM
 * =====================
 *
 */



#include <JCoreTest/CoreTest.h>


#include <JCore/FileSystem/FileStream.h>

#if TEST_FileStreamTest == ON

TEST(FileStreamTest, BasicTest) {

	CRuntime::FileDelete("a.dat");
	CRuntime::FileDelete("b.dat");
	CRuntime::FileDelete("c.dat");

	Byte wbuff[] { 1, 2, 3, 4, 5 };
	Byte wbuff2[] {  6, 7, 8, 9, 10 };
	
	Byte rbuff[100]{};

	//  ===================== Write =========================================
	{
		EXPECT_THROW(FileStream("a.dat", FileAccess::eWrite, FileMode::eOpen), RuntimeException);		// 파일이 없으므로 오류
	}

	{
		
		FileStream stream("a.dat", FileAccess::eWrite, FileMode::eCreate);	// 정상 (파일 생성)
		stream.Write(wbuff, 0, 5);
		stream.Write(wbuff, 0, 5);
		stream.Write(wbuff, 0, 5);
		stream.Write(wbuff, 0, 5);
		EXPECT_EQ(stream.GetOffset(), stream.GetLength());
	}

	{
		FileStream stream("c.dat", FileAccess::eWrite, FileMode::eAppend);	// 정상 (파일 생성)
		stream.Write(wbuff, 0, 5);
		stream.Write(wbuff, 0, 5);
		stream.Write(wbuff, 0, 5);
		stream.Write(wbuff, 0, 5);
		EXPECT_EQ(stream.GetOffset(), stream.GetLength());
	}

	//  ===================== Read =========================================
	{
		Arrays::Fill(rbuff, (Byte)0);
		FileStream stream("a.dat", FileAccess::eRead, FileMode::eOpen);		// 정상 (a.dat을 위에서 만들었으므로)
		int offset = 0;
		EXPECT_EQ(stream.Read(rbuff, offset, 5), 5); offset += 5;

		// 제대로 읽었는지 확인
		for (int i = 1; i <= 5; i++) 
			EXPECT_EQ((int)rbuff[i - 1], i);

		EXPECT_EQ(stream.GetOffset(), offset);
		EXPECT_EQ(stream.Read(rbuff, offset, 5), 5); offset += 5;
		EXPECT_EQ(stream.GetOffset(), offset);
		EXPECT_EQ(stream.Read(rbuff, offset, 5), 5); offset += 5;
		EXPECT_EQ(stream.GetOffset(), offset);

		// 5바이트만 더 읽을 수 있는데 8바이트 읽기를 시도한 경우 5바이트만 읽는다.
		EXPECT_EQ(stream.Read(rbuff, offset, 8), 5);
		EXPECT_EQ(stream.GetOffset(), offset += 5);
		EXPECT_EQ(stream.GetOffset(), stream.GetLength());	// 끝자락에 도달
	}

	{
		EXPECT_THROW(FileStream("b.dat", FileAccess::eRead, FileMode::eOpen), RuntimeException);		// 파일이 없으므로 오류
	}


	//  ===================== ReadWrite =========================================


	{
		EXPECT_THROW(FileStream("b.dat", FileAccess::eReadWrite, FileMode::eOpen), RuntimeException);		// 파일이 없으므로 오류	
	}

	{
		Arrays::Fill(rbuff, (Byte)0);
		FileStream stream("a.dat", FileAccess::eReadWrite, FileMode::eOpen);		// 정상 (a.dat을 위에서 만들었으므로)
		EXPECT_EQ(stream.GetLength(), 20);	// 위에서 20바이트만큼 썼으므로
		EXPECT_EQ(stream.GetOffset(), 0);	// 아직 아무런 작업을 하지 않았으므로
		stream.Write(wbuff2, 0, 5);
		EXPECT_EQ(stream.GetOffset(), 5);

		stream.Seek(0);
		int offset = 0;
		EXPECT_EQ(stream.Read(rbuff, offset, 5), 5); offset += 5;
		EXPECT_EQ(stream.GetOffset(), 5);

		// 제대로 읽었는지 확인
		// 6 7 8 9 10 0 0 0 0 0
		// |   읽음   |
		for (int i = 0, j = 6; i < 5; i++, j++)
			EXPECT_EQ((int)rbuff[i], j);


		EXPECT_EQ(stream.Read(rbuff, offset, 5), 5); offset += 5;
		EXPECT_EQ(stream.GetOffset(), 10);

		// 6 7 8 9 10  1 2 3 4 5
		// |   읽음   |  읽음    |
		for (int i = 1; i <= 5; i++)
			EXPECT_EQ((int)rbuff[i + 4], i);

		// 마지막 위치로 이동
		stream.Seek(stream.GetLength());
		stream.Write(wbuff2, 0, 5);
		stream.Write(wbuff2, 0, 5);
		EXPECT_EQ(stream.GetOffset(), 30);
		EXPECT_EQ(stream.GetLength(), 30);
	}

	{
		FileStream stream("a.dat", FileAccess::eReadWrite, FileMode::eAppend);	// 새로 덮어쒸워서 생성했으므로 모두 0
		EXPECT_EQ(stream.GetLength(), 30);
		EXPECT_EQ(stream.GetOffset(), 30);

		stream.Write(wbuff2, 0, 5);
		stream.Write(wbuff2, 0, 5);

		EXPECT_EQ(stream.GetOffset(), 40);
		EXPECT_EQ(stream.GetLength(), 40);
	}

	{
		FileStream stream("a.dat", FileAccess::eReadWrite, FileMode::eCreate);	// 새로 덮어쒸워서 생성했으므로 모두 0
		EXPECT_EQ(stream.GetLength(), 0);
		EXPECT_EQ(stream.GetOffset(), 0);
		stream.Write(wbuff2, 0, 5);
		EXPECT_EQ(stream.GetOffset(), 5);

		stream.Close();
		EXPECT_TRUE(stream.IsClosed());

	}

	{
		FileStream stream("a.dat", FileAccess::eReadWrite, FileMode::eCreate);	// 새로 덮어쒸워서 생성했으므로 모두 0
		stream.WriteInt16(12345);
		stream.WriteInt16(23456);
		stream.WriteInt16(30102);
		stream.Seek(0);
		EXPECT_EQ(stream.ReadInt16(), 12345);
		EXPECT_EQ(stream.ReadInt16(), 23456);
		EXPECT_EQ(stream.ReadInt16(), 30102);
		stream.WriteString("abcdefg", true);
		stream.Close();
	}

	{
		FileStream stream("a.dat", FileAccess::eReadWrite, FileMode::eOpen);	
		stream.Seek(6);
		EXPECT_TRUE(stream.ReadString() == "abcdefg");
	}


	CRuntime::FileDelete("a.dat");
	CRuntime::FileDelete("b.dat");
}

#endif
