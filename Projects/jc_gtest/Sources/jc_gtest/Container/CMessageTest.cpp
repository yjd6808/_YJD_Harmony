/*
	작성자 : 윤정도
	CMessage 클래스를 테스트합니다.
*/



#include "jc/Container/CMessage.h"

USING_NS_STD;
USING_NS_JC;

#if TEST_CMessageTest == ON

//////////////////////////////////////////////////////////////////////////////////////////
// 기본 생성자 및 초기화 테스트
//////////////////////////////////////////////////////////////////////////////////////////
TEST(CMessageTest, DefaultConstructor) {
	LeakCheck;
	CMessage msg;

	EXPECT_TRUE(msg.IsNull());
	EXPECT_TRUE(msg.IsEmpty());
	EXPECT_EQ(msg.GetMsgId(), 0);
	EXPECT_EQ(msg.GetTargetId(), 0);
	EXPECT_EQ(msg.GetWriteOffset(), 0);
	EXPECT_EQ(msg.GetReadOffset(), 0);
}

//////////////////////////////////////////////////////////////////////////////////////////
// 매개변수가 있는 생성자 테스트
//////////////////////////////////////////////////////////////////////////////////////////
TEST(CMessageTest, ParameterConstructor) {
	LeakCheck;
	CMessage msg(32, 256, 100, 200);

	EXPECT_FALSE(msg.IsNull());
	EXPECT_TRUE(msg.IsEmpty());
	EXPECT_EQ(msg.GetMsgId(), 100);
	EXPECT_EQ(msg.GetTargetId(), 200);
	EXPECT_EQ(msg.GetWriteOffset(), 0);
	EXPECT_EQ(msg.GetReadOffset(), 0);
}

//////////////////////////////////////////////////////////////////////////////////////////
// S8 쓰기 및 읽기 테스트
//////////////////////////////////////////////////////////////////////////////////////////
TEST(CMessageTest, WriteReadS8) {
	LeakCheck;
	CMessage msg(32, 256, 1, 0);

	// S8 값 쓰기
	msg.WriteS8(-128);
	msg.WriteS8(0);
	msg.WriteS8(127);

	// 오프셋 초기화 후 읽기
	msg.ResetReadOffset();

	EXPECT_EQ(msg.ReadS8(), -128);
	EXPECT_EQ(msg.ReadS8(), 0);
	EXPECT_EQ(msg.ReadS8(), 127);
}

//////////////////////////////////////////////////////////////////////////////////////////
// U8 쓰기 및 읽기 테스트
//////////////////////////////////////////////////////////////////////////////////////////
TEST(CMessageTest, WriteReadU8) {
	LeakCheck;
	CMessage msg(32, 256, 2, 0);

	msg.WriteU8(0);
	msg.WriteU8(128);
	msg.WriteU8(255);

	msg.ResetReadOffset();

	EXPECT_EQ(msg.ReadU8(), 0);
	EXPECT_EQ(msg.ReadU8(), 128);
	EXPECT_EQ(msg.ReadU8(), 255);
}

//////////////////////////////////////////////////////////////////////////////////////////
// S16 쓰기 및 읽기 테스트
//////////////////////////////////////////////////////////////////////////////////////////
TEST(CMessageTest, WriteReadS16) {
	LeakCheck;
	CMessage msg(32, 256, 3, 0);

	msg.WriteS16(-32768);
	msg.WriteS16(0);
	msg.WriteS16(32767);

	msg.ResetReadOffset();

	EXPECT_EQ(msg.ReadS16(), -32768);
	EXPECT_EQ(msg.ReadS16(), 0);
	EXPECT_EQ(msg.ReadS16(), 32767);
}

//////////////////////////////////////////////////////////////////////////////////////////
// U16 쓰기 및 읽기 테스트
//////////////////////////////////////////////////////////////////////////////////////////
TEST(CMessageTest, WriteReadU16) {
	LeakCheck;
	CMessage msg(32, 256, 4, 0);

	msg.WriteU16(0);
	msg.WriteU16(32768);
	msg.WriteU16(65535);

	msg.ResetReadOffset();

	EXPECT_EQ(msg.ReadU16(), 0);
	EXPECT_EQ(msg.ReadU16(), 32768);
	EXPECT_EQ(msg.ReadU16(), 65535);
}

//////////////////////////////////////////////////////////////////////////////////////////
// S32 쓰기 및 읽기 테스트
//////////////////////////////////////////////////////////////////////////////////////////
TEST(CMessageTest, WriteReadS32) {
	LeakCheck;
	CMessage msg(32, 256, 5, 0);

	msg.WriteS32(-2147483648);
	msg.WriteS32(0);
	msg.WriteS32(2147483647);

	msg.ResetReadOffset();

	EXPECT_EQ(msg.ReadS32(), -2147483648);
	EXPECT_EQ(msg.ReadS32(), 0);
	EXPECT_EQ(msg.ReadS32(), 2147483647);
}

//////////////////////////////////////////////////////////////////////////////////////////
// U32 쓰기 및 읽기 테스트
//////////////////////////////////////////////////////////////////////////////////////////
TEST(CMessageTest, WriteReadU32) {
	LeakCheck;
	CMessage msg(32, 256, 6, 0);

	msg.WriteU32(0);
	msg.WriteU32(2147483648);
	msg.WriteU32(4294967295);

	msg.ResetReadOffset();

	EXPECT_EQ(msg.ReadU32(), 0);
	EXPECT_EQ(msg.ReadU32(), 2147483648);
	EXPECT_EQ(msg.ReadU32(), 4294967295);
}

//////////////////////////////////////////////////////////////////////////////////////////
// S64 쓰기 및 읽기 테스트
//////////////////////////////////////////////////////////////////////////////////////////
TEST(CMessageTest, WriteReadS64) {
	LeakCheck;
	CMessage msg(32, 256, 7, 0);

	msg.WriteS64(-9223372036854775800LL);
	msg.WriteS64(0);
	msg.WriteS64(9223372036854775800LL);

	msg.ResetReadOffset();

	EXPECT_EQ(msg.ReadS64(), -9223372036854775800LL);
	EXPECT_EQ(msg.ReadS64(), 0);
	EXPECT_EQ(msg.ReadS64(), 9223372036854775800LL);
}

//////////////////////////////////////////////////////////////////////////////////////////
// U64 쓰기 및 읽기 테스트
//////////////////////////////////////////////////////////////////////////////////////////
TEST(CMessageTest, WriteReadU64) {
	LeakCheck;
	CMessage msg(32, 256, 8, 0);

	msg.WriteU64(0);
	msg.WriteU64(9223372036854775808ULL);
	msg.WriteU64(18446744073709551615ULL);

	msg.ResetReadOffset();

	EXPECT_EQ(msg.ReadU64(), 0);
	EXPECT_EQ(msg.ReadU64(), 9223372036854775808ULL);
	EXPECT_EQ(msg.ReadU64(), 18446744073709551615ULL);
}

//////////////////////////////////////////////////////////////////////////////////////////
// Float 쓰기 및 읽기 테스트
//////////////////////////////////////////////////////////////////////////////////////////
TEST(CMessageTest, WriteReadFloat) {
	LeakCheck;
	CMessage msg(32, 256, 9, 0);

	msg.WriteFloat(0.0f);
	msg.WriteFloat(3.141592f);
	msg.WriteFloat(-99.999f);

	msg.ResetReadOffset();

	EXPECT_FLOAT_EQ(msg.ReadFloat(), 0.0f);
	EXPECT_FLOAT_EQ(msg.ReadFloat(), 3.141592f);
	EXPECT_FLOAT_EQ(msg.ReadFloat(), -99.999f);
}

//////////////////////////////////////////////////////////////////////////////////////////
// Double 쓰기 및 읽기 테스트
//////////////////////////////////////////////////////////////////////////////////////////
TEST(CMessageTest, WriteReadDouble) {
	LeakCheck;
	CMessage msg(32, 256, 10, 0);

	msg.WriteDouble(0.0);
	msg.WriteDouble(2.718281828459045);
	msg.WriteDouble(-123.456789);

	msg.ResetReadOffset();

	EXPECT_DOUBLE_EQ(msg.ReadDouble(), 0.0);
	EXPECT_DOUBLE_EQ(msg.ReadDouble(), 2.718281828459045);
	EXPECT_DOUBLE_EQ(msg.ReadDouble(), -123.456789);
}

//////////////////////////////////////////////////////////////////////////////////////////
// Ptr 쓰기 및 읽기 테스트
//////////////////////////////////////////////////////////////////////////////////////////
TEST(CMessageTest, WriteReadPtr) {
	LeakCheck;
	CMessage msg(32, 256, 11, 0);

	int value1 = 100;
	int value2 = 200;
	int value3 = 300;

	msg.WritePtr(&value1);
	msg.WritePtr(&value2);
	msg.WritePtr(&value3);

	msg.ResetReadOffset();

	EXPECT_EQ(msg.ReadPtr(), &value1);
	EXPECT_EQ(msg.ReadPtr(), &value2);
	EXPECT_EQ(msg.ReadPtr(), &value3);
}

//////////////////////////////////////////////////////////////////////////////////////////
// String 쓰기 및 읽기 테스트
//////////////////////////////////////////////////////////////////////////////////////////
TEST(CMessageTest, WriteReadString) {
	LeakCheck;
	CMessage msg(32, 256, 12, 0);

	msg.WriteString(String("Hello"));
	msg.WriteString(String("World"));
	msg.WriteString(String(""));

	msg.ResetReadOffset();

	EXPECT_EQ(msg.ReadString(), String("Hello"));
	EXPECT_EQ(msg.ReadString(), String("World"));
	EXPECT_EQ(msg.ReadString(), String(""));
}

//////////////////////////////////////////////////////////////////////////////////////////
// Binary 쓰기 및 읽기 테스트
//////////////////////////////////////////////////////////////////////////////////////////
TEST(CMessageTest, WriteReadBinary) {
	LeakCheck;
	CMessage msg(32, 512, 13, 0);

	_u8 data1[] = { 0x01, 0x02, 0x03, 0x04, 0x05 };
	_u8 data2[] = { 0xAA, 0xBB, 0xCC };
	_u8 readBuffer[10];

	msg.WriteBinary(data1, 5);
	msg.WriteBinary(data2, 3);

	msg.ResetReadOffset();

	_u32 outLen = 0;
	EXPECT_TRUE(msg.ReadBinary(readBuffer, 10, outLen));
	EXPECT_EQ(outLen, 5);
	EXPECT_EQ(memcmp(readBuffer, data1, 5), 0);

	EXPECT_TRUE(msg.ReadBinary(readBuffer, 10, outLen));
	EXPECT_EQ(outLen, 3);
	EXPECT_EQ(memcmp(readBuffer, data2, 3), 0);
}

//////////////////////////////////////////////////////////////////////////////////////////
// 혼합 데이터 타입 쓰기 및 읽기 테스트
//////////////////////////////////////////////////////////////////////////////////////////
TEST(CMessageTest, MixedDataTypes) {
	LeakCheck;
	CMessage msg(32, 512, 20, 100);

	// 다양한 타입의 데이터 쓰기
	msg.WriteS32(42);
	msg.WriteFloat(3.14f);
	msg.WriteString(String("test"));
	msg.WriteU64(9999999999ULL);
	msg.WriteBool(true);

	msg.ResetReadOffset();

	EXPECT_EQ(msg.ReadS32(), 42);
	EXPECT_FLOAT_EQ(msg.ReadFloat(), 3.14f);
	EXPECT_EQ(msg.ReadString(), String("test"));
	EXPECT_EQ(msg.ReadU64(), 9999999999ULL);
	EXPECT_TRUE(msg.ReadBool());
}

//////////////////////////////////////////////////////////////////////////////////////////
// TryRead 메서드 테스트
//////////////////////////////////////////////////////////////////////////////////////////
TEST(CMessageTest, TryReadMethods) {
	LeakCheck;
	CMessage msg(32, 256, 21, 0);

	msg.WriteS32(100);
	msg.WriteFloat(2.5f);
	msg.WriteString(String("success"));

	msg.ResetReadOffset();

	_s32 s32Val;
	_f32 floatVal;
	String strVal;

	EXPECT_TRUE(msg.TryReadS32(s32Val));
	EXPECT_EQ(s32Val, 100);

	EXPECT_TRUE(msg.TryReadFloat(floatVal));
	EXPECT_FLOAT_EQ(floatVal, 2.5f);

	EXPECT_TRUE(msg.TryReadString(strVal));
	EXPECT_EQ(strVal, String("success"));
}

//////////////////////////////////////////////////////////////////////////////////////////
// 오프셋 제어 테스트
//////////////////////////////////////////////////////////////////////////////////////////
TEST(CMessageTest, OffsetControl) {
	LeakCheck;
	CMessage msg(32, 256, 22, 0);

	msg.WriteS32(1);
	msg.WriteS32(2);
	msg.WriteS32(3);

	EXPECT_EQ(msg.GetWriteOffset(), 3);

	// 읽기 오프셋 설정
	msg.ResetReadOffset();
	EXPECT_EQ(msg.GetReadOffset(), 0);

	msg.ReadS32(); // 1 읽음
	EXPECT_EQ(msg.GetReadOffset(), 1);

	msg.ReadS32(); // 2 읽음
	EXPECT_EQ(msg.GetReadOffset(), 2);

	// 읽기 오프셋 초기화
	msg.ResetReadOffset();
	EXPECT_EQ(msg.GetReadOffset(), 0);

	// 쓰기 오프셋 초기화
	msg.ResetWriteOffset();
	EXPECT_EQ(msg.GetWriteOffset(), 0);
}

//////////////////////////////////////////////////////////////////////////////////////////
// 메시지 ID 및 타겟 ID 설정/조회 테스트
//////////////////////////////////////////////////////////////////////////////////////////
TEST(CMessageTest, SetGetMsgIdTargetId) {
	LeakCheck;
	CMessage msg(32, 256);

	msg.SetMsgId(12345);
	msg.SetTargetId(67890);

	EXPECT_EQ(msg.GetMsgId(), 12345);
	EXPECT_EQ(msg.GetTargetId(), 67890);

	// 다시 설정
	msg.SetMsgId(11111);
	msg.SetTargetId(22222);

	EXPECT_EQ(msg.GetMsgId(), 11111);
	EXPECT_EQ(msg.GetTargetId(), 22222);
}

//////////////////////////////////////////////////////////////////////////////////////////
// 복사 생성자 테스트
//////////////////////////////////////////////////////////////////////////////////////////
TEST(CMessageTest, CopyConstructor) {
	LeakCheck;
	{
		CMessage msg1(32, 256, 100, 200);
		msg1.WriteS32(42);
		msg1.WriteString(String("original"));

		CMessage msg2(msg1);

		EXPECT_EQ(msg2.GetMsgId(), 100);
		EXPECT_EQ(msg2.GetTargetId(), 200);
		EXPECT_EQ(msg2.GetWriteOffset(), msg1.GetWriteOffset());

		msg2.ResetReadOffset();
		EXPECT_EQ(msg2.ReadS32(), 42);
		EXPECT_EQ(msg2.ReadString(), String("original"));
	}
}

//////////////////////////////////////////////////////////////////////////////////////////
// 이동 생성자 테스트
//////////////////////////////////////////////////////////////////////////////////////////
TEST(CMessageTest, MoveConstructor) {
	LeakCheck;
	{
		CMessage msg1(32, 256, 100, 200);
		msg1.WriteS32(42);
		msg1.WriteString(String("moved"));

		_u16 originalWriteOffset = msg1.GetWriteOffset();

		CMessage msg2(Move(msg1));

		EXPECT_EQ(msg2.GetMsgId(), 100);
		EXPECT_EQ(msg2.GetTargetId(), 200);
		EXPECT_EQ(msg2.GetWriteOffset(), originalWriteOffset);

		msg2.ResetReadOffset();
		EXPECT_EQ(msg2.ReadS32(), 42);
		EXPECT_EQ(msg2.ReadString(), String("moved"));
	}
}

//////////////////////////////////////////////////////////////////////////////////////////
// 복사 할당 연산자 테스트
//////////////////////////////////////////////////////////////////////////////////////////
TEST(CMessageTest, CopyAssignment) {
	LeakCheck;
	{
		CMessage msg1(32, 256, 100, 200);
		msg1.WriteS32(100);
		msg1.WriteFloat(1.5f);

		CMessage msg2(32, 256, 50, 60);
		msg2.WriteU32(999);

		msg2 = msg1;

		EXPECT_EQ(msg2.GetMsgId(), 100);
		EXPECT_EQ(msg2.GetTargetId(), 200);

		msg2.ResetReadOffset();
		EXPECT_EQ(msg2.ReadS32(), 100);
		EXPECT_FLOAT_EQ(msg2.ReadFloat(), 1.5f);
	}
}

//////////////////////////////////////////////////////////////////////////////////////////
// 이동 할당 연산자 테스트
//////////////////////////////////////////////////////////////////////////////////////////
TEST(CMessageTest, MoveAssignment) {
	LeakCheck;
	{
		CMessage msg1(32, 256, 100, 200);
		msg1.WriteS32(500);
		msg1.WriteString(String("test"));

		CMessage msg2(32, 256, 50, 60);
		msg2.WriteU32(111);

		msg2 = Move(msg1);

		EXPECT_EQ(msg2.GetMsgId(), 100);
		EXPECT_EQ(msg2.GetTargetId(), 200);

		msg2.ResetReadOffset();
		EXPECT_EQ(msg2.ReadS32(), 500);
		EXPECT_EQ(msg2.ReadString(), String("test"));
	}
}

//////////////////////////////////////////////////////////////////////////////////////////
// IsEmpty 테스트
//////////////////////////////////////////////////////////////////////////////////////////
TEST(CMessageTest, IsEmpty) {
	LeakCheck;
	CMessage msg(32, 256, 1, 0);

	EXPECT_TRUE(msg.IsEmpty());

	msg.WriteS32(42);
	EXPECT_FALSE(msg.IsEmpty());

	msg.ResetWriteOffset();
	EXPECT_TRUE(msg.IsEmpty());
}

//////////////////////////////////////////////////////////////////////////////////////////
// ReadAny 메서드 테스트 (variant 타입 조회)
//////////////////////////////////////////////////////////////////////////////////////////
TEST(CMessageTest, ReadAny) {
	LeakCheck;
	CMessage msg(32, 256, 30, 0);

	msg.WriteS8(10);
	msg.WriteU16(1000);
	msg.WriteString(String("test"));

	msg.ResetReadOffset();

	CMessage::VariantType vt1 = msg.ReadAny();
	EXPECT_EQ(vt1, CMessage::vt_s8);

	CMessage::VariantType vt2 = msg.ReadAny();
	EXPECT_EQ(vt2, CMessage::vt_u16);

	CMessage::VariantType vt3 = msg.ReadAny();
	EXPECT_EQ(vt3, CMessage::vt_string);
}

//////////////////////////////////////////////////////////////////////////////////////////
// WriteBinaryDummy 테스트
//////////////////////////////////////////////////////////////////////////////////////////
TEST(CMessageTest, WriteBinaryDummy) {
	LeakCheck;
	CMessage msg(32, 512, 31, 0);

	msg.WriteS32(111);
	msg.WriteBinaryDummy(100);  // 더미 바이너리 쓰기
	msg.WriteS32(222);

	msg.ResetReadOffset();

	EXPECT_EQ(msg.ReadS32(), 111);

	_u8 readBuffer[100];
	_u32 outLen = 0;
	EXPECT_TRUE(msg.ReadBinary(readBuffer, 100, outLen));
	EXPECT_EQ(outLen, 100);

	EXPECT_EQ(msg.ReadS32(), 222);
}

//////////////////////////////////////////////////////////////////////////////////////////
// GetCurrentVT 테스트
//////////////////////////////////////////////////////////////////////////////////////////
TEST(CMessageTest, GetCurrentVT) {
	LeakCheck;
	CMessage msg(32, 256, 32, 0);

	msg.WriteS32(42);
	msg.WriteFloat(3.14f);
	msg.WriteString(String("test"));

	msg.ResetReadOffset();

	EXPECT_EQ(msg.GetCurrentVT(), CMessage::vt_s32);
	msg.ReadS32();

	EXPECT_EQ(msg.GetCurrentVT(), CMessage::vt_f32);
	msg.ReadFloat();

	EXPECT_EQ(msg.GetCurrentVT(), CMessage::vt_string);
}

//////////////////////////////////////////////////////////////////////////////////////////
// GetVT 메서드 테스트
//////////////////////////////////////////////////////////////////////////////////////////
TEST(CMessageTest, GetVT) {
	LeakCheck;
	CMessage msg(32, 256, 33, 0);

	msg.WriteS32(100);
	msg.WriteFloat(2.5f);
	msg.WriteString(String("variant"));

	EXPECT_EQ(msg.GetVT(0), CMessage::vt_s32);
	EXPECT_EQ(msg.GetVT(1), CMessage::vt_f32);
	EXPECT_EQ(msg.GetVT(2), CMessage::vt_string);
}

//////////////////////////////////////////////////////////////////////////////////////////
// SetUsage 및 GetUsage 테스트
//////////////////////////////////////////////////////////////////////////////////////////
TEST(CMessageTest, SetGetUsage) {
	LeakCheck;
	CMessage msg(32, 256);

	EXPECT_EQ(msg.GetUsage(), CMessage::USAGE_NONE);

	msg.SetUsage(12345);
	EXPECT_EQ(msg.GetUsage(), 12345);

	msg.SetUsage(67890);
	EXPECT_EQ(msg.GetUsage(), 67890);
}

//////////////////////////////////////////////////////////////////////////////////////////
// Bool 쓰기 및 읽기 테스트
//////////////////////////////////////////////////////////////////////////////////////////
TEST(CMessageTest, WriteReadBool) {
	LeakCheck;
	CMessage msg(32, 256, 40, 0);

	msg.WriteBool(true);
	msg.WriteBool(false);
	msg.WriteBool(true);

	msg.ResetReadOffset();

	EXPECT_TRUE(msg.ReadBool());
	EXPECT_FALSE(msg.ReadBool());
	EXPECT_TRUE(msg.ReadBool());
}

//////////////////////////////////////////////////////////////////////////////////////////
// TryReadBool 테스트
//////////////////////////////////////////////////////////////////////////////////////////
TEST(CMessageTest, TryReadBool) {
	LeakCheck;
	CMessage msg(32, 256, 41, 0);

	msg.WriteBool(true);
	msg.WriteBool(false);

	msg.ResetReadOffset();

	bool val1;
	bool val2;

	EXPECT_TRUE(msg.TryReadBool(val1));
	EXPECT_TRUE(val1);

	EXPECT_TRUE(msg.TryReadBool(val2));
	EXPECT_FALSE(val2);
}

//////////////////////////////////////////////////////////////////////////////////////////
// S32L 쓰기 및 읽기 테스트
//////////////////////////////////////////////////////////////////////////////////////////
TEST(CMessageTest, WriteReadS32L) {
	LeakCheck;
	CMessage msg(32, 256, 42, 0);

	msg.WriteS32L(-2147483648);
	msg.WriteS32L(0);
	msg.WriteS32L(2147483647);

	msg.ResetReadOffset();

	EXPECT_EQ(msg.ReadS32L(), -2147483648);
	EXPECT_EQ(msg.ReadS32L(), 0);
	EXPECT_EQ(msg.ReadS32L(), 2147483647);
}

//////////////////////////////////////////////////////////////////////////////////////////
// U32L 쓰기 및 읽기 테스트
//////////////////////////////////////////////////////////////////////////////////////////
TEST(CMessageTest, WriteReadU32L) {
	LeakCheck;
	CMessage msg(32, 256, 43, 0);

	msg.WriteU32L(0);
	msg.WriteU32L(2147483648);
	msg.WriteU32L(4294967295);

	msg.ResetReadOffset();

	EXPECT_EQ(msg.ReadU32L(), 0);
	EXPECT_EQ(msg.ReadU32L(), 2147483648);
	EXPECT_EQ(msg.ReadU32L(), 4294967295);
}

//////////////////////////////////////////////////////////////////////////////////////////
// TryRead ptr 테스트
//////////////////////////////////////////////////////////////////////////////////////////
TEST(CMessageTest, TryReadPtr) {
	LeakCheck;
	CMessage msg(32, 256, 44, 0);

	int value = 999;
	msg.WritePtr(&value);

	msg.ResetReadOffset();

	void* pResult = nullptr;
	EXPECT_TRUE(msg.TryReadPtr(pResult));
	EXPECT_EQ(pResult, &value);
}

//////////////////////////////////////////////////////////////////////////////////////////
// TryReadBinary 테스트
//////////////////////////////////////////////////////////////////////////////////////////
TEST(CMessageTest, TryReadBinary) {
	LeakCheck;
	CMessage msg(32, 512, 45, 0);

	_u8 writeData[] = { 0x10, 0x20, 0x30, 0x40 };
	msg.WriteBinary(writeData, 4);

	msg.ResetReadOffset();

	_u8 readBuffer[10];
	_u32 outLen = 0;

	EXPECT_TRUE(msg.TryReadBinary(readBuffer, 10, outLen));
	EXPECT_EQ(outLen, 4);
	EXPECT_EQ(readBuffer[0], 0x10);
	EXPECT_EQ(readBuffer[1], 0x20);
	EXPECT_EQ(readBuffer[2], 0x30);
	EXPECT_EQ(readBuffer[3], 0x40);
}

//////////////////////////////////////////////////////////////////////////////////////////
// TryRead S64 테스트
//////////////////////////////////////////////////////////////////////////////////////////
TEST(CMessageTest, TryReadS64) {
	LeakCheck;
	CMessage msg(32, 256, 46, 0);

	msg.WriteS64(9223372036854775800LL);

	msg.ResetReadOffset();

	_s64 value = 0;
	EXPECT_TRUE(msg.TryReadS64(value));
	EXPECT_EQ(value, 9223372036854775800LL);
}

//////////////////////////////////////////////////////////////////////////////////////////
// TryRead U64 테스트
//////////////////////////////////////////////////////////////////////////////////////////
TEST(CMessageTest, TryReadU64) {
	LeakCheck;
	CMessage msg(32, 256, 47, 0);

	msg.WriteU64(18446744073709551615ULL);

	msg.ResetReadOffset();

	_u64 value = 0;
	EXPECT_TRUE(msg.TryReadU64(value));
	EXPECT_EQ(value, 18446744073709551615ULL);
}

//////////////////////////////////////////////////////////////////////////////////////////
// SetWriteOffset 테스트
//////////////////////////////////////////////////////////////////////////////////////////
TEST(CMessageTest, SetWriteOffset) {
	LeakCheck;
	CMessage msg(32, 256, 48, 0);

	msg.WriteS32(100);
	msg.WriteS32(200);
	msg.WriteS32(300);

	EXPECT_EQ(msg.GetWriteOffset(), 3);

	// 쓰기 오프셋 감소
	msg.SetWriteOffset(1);
	EXPECT_EQ(msg.GetWriteOffset(), 1);

	msg.ResetReadOffset();
	EXPECT_EQ(msg.ReadS32(), 100);

	// 더 이상 읽을 데이터가 없어야 함
	msg.ResetReadOffset();
	msg.SetWriteOffset(0);
	EXPECT_TRUE(msg.IsEmpty());
}

//////////////////////////////////////////////////////////////////////////////////////////
// SetReadOffset 테스트
//////////////////////////////////////////////////////////////////////////////////////////
TEST(CMessageTest, SetReadOffset) {
	LeakCheck;
	CMessage msg(32, 256, 49, 0);

	msg.WriteS32(111);
	msg.WriteS32(222);
	msg.WriteS32(333);

	msg.SetReadOffset(1);
	EXPECT_EQ(msg.GetReadOffset(), 1);

	EXPECT_EQ(msg.ReadS32(), 222);

	msg.SetReadOffset(0);
	EXPECT_EQ(msg.GetReadOffset(), 0);

	EXPECT_EQ(msg.ReadS32(), 111);
}

#endif
