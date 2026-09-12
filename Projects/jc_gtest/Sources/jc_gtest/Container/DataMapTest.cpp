/*
	작성자 : 윤정도
	CDataMap 클래스를 테스트합니다.
*/


#include "jc/Container/DataMap.h"

USING_NS_STD;
USING_NS_JC;

#if TEST_DataMapTest == ON

//////////////////////////////////////////////////////////////////////////////////////////
// SetS32 테스트
//////////////////////////////////////////////////////////////////////////////////////////
TEST(DataMapTest, SetGetS32) {
	LeakCheck;
	CDataMap map;

	// S32 데이터 설정 및 조회
	EXPECT_TRUE(map.SetS32(String(_T("intKey1")), -12345));
	EXPECT_TRUE(map.SetS32(String(_T("intKey2")), 0));
	EXPECT_TRUE(map.SetS32(String(_T("intKey3")), 2147483647));

	// 기존 키에 대해 다시 설정 - 업데이트되어야 함
	EXPECT_TRUE(map.SetS32(String(_T("intKey1")), 54321));

	EXPECT_EQ(map.GetS32(String(_T("intKey1"))), 54321);
	EXPECT_EQ(map.GetS32(String(_T("intKey2"))), 0);
	EXPECT_EQ(map.GetS32(String(_T("intKey3"))), 2147483647);

	// 존재하지 않는 키 조회 - 기본값 0 반환
	EXPECT_EQ(map.GetS32(String(_T("nonExistent"))), 0);

	// TryGetS32 테스트
	_s32 outValue = 0;
	EXPECT_TRUE(map.TryGetS32(String(_T("intKey1")), outValue));
	EXPECT_EQ(outValue, 54321);

	EXPECT_FALSE(map.TryGetS32(String(_T("nonExistent")), outValue));
}

//////////////////////////////////////////////////////////////////////////////////////////
// SetGetU32 테스트
//////////////////////////////////////////////////////////////////////////////////////////
TEST(DataMapTest, SetGetU32) {
	LeakCheck;
	CDataMap map;

	// U32 데이터 설정 및 조회
	EXPECT_TRUE(map.SetU32(String(_T("uint1")), 0));
	EXPECT_TRUE(map.SetU32(String(_T("uint2")), 4294967295));
	EXPECT_TRUE(map.SetU32(String(_T("uint3")), 2000000000));

	// 업데이트 테스트
	EXPECT_TRUE(map.SetU32(String(_T("uint1")), 1234567890));

	EXPECT_EQ(map.GetU32(String(_T("uint1"))), 1234567890);
	EXPECT_EQ(map.GetU32(String(_T("uint2"))), 4294967295);
	EXPECT_EQ(map.GetU32(String(_T("uint3"))), 2000000000);

	// TryGetU32 테스트
	_u32 outValue = 0;
	EXPECT_TRUE(map.TryGetU32(String(_T("uint2")), outValue));
	EXPECT_EQ(outValue, 4294967295);

	EXPECT_FALSE(map.TryGetU32(String(_T("nonExistent")), outValue));
}

//////////////////////////////////////////////////////////////////////////////////////////
// SetGetS64 테스트
//////////////////////////////////////////////////////////////////////////////////////////
TEST(DataMapTest, SetGetS64) {
	LeakCheck;
	CDataMap map;

	// S64 데이터 설정 및 조회
	EXPECT_TRUE(map.SetS64(String(_T("s64Key1")), -9223372036854775800LL));
	EXPECT_TRUE(map.SetS64(String(_T("s64Key2")), 0));
	EXPECT_TRUE(map.SetS64(String(_T("s64Key3")), 9223372036854775800LL));

	// 업데이트 테스트
	EXPECT_TRUE(map.SetS64(String(_T("s64Key1")), 1234567890123456789LL));

	EXPECT_EQ(map.GetS64(String(_T("s64Key1"))), 1234567890123456789LL);
	EXPECT_EQ(map.GetS64(String(_T("s64Key2"))), 0);
	EXPECT_EQ(map.GetS64(String(_T("s64Key3"))), 9223372036854775800LL);

	// TryGetS64 테스트
	_s64 outValue = 0;
	EXPECT_TRUE(map.TryGetS64(String(_T("s64Key3")), outValue));
	EXPECT_EQ(outValue, 9223372036854775800LL);

	EXPECT_FALSE(map.TryGetS64(String(_T("nonExistent")), outValue));
}

//////////////////////////////////////////////////////////////////////////////////////////
// SetGetU64 테스트
//////////////////////////////////////////////////////////////////////////////////////////
TEST(DataMapTest, SetGetU64) {
	LeakCheck;
	CDataMap map;

	// U64 데이터 설정 및 조회
	EXPECT_TRUE(map.SetU64(String(_T("u64Key1")), 0));
	EXPECT_TRUE(map.SetU64(String(_T("u64Key2")), 18446744073709551615ULL));
	EXPECT_TRUE(map.SetU64(String(_T("u64Key3")), 9999999999999999999ULL));

	// 업데이트 테스트
	EXPECT_TRUE(map.SetU64(String(_T("u64Key1")), 12345678901234567890ULL));

	EXPECT_EQ(map.GetU64(String(_T("u64Key1"))), 12345678901234567890ULL);
	EXPECT_EQ(map.GetU64(String(_T("u64Key2"))), 18446744073709551615ULL);
	EXPECT_EQ(map.GetU64(String(_T("u64Key3"))), 9999999999999999999ULL);

	// TryGetU64 테스트
	_u64 outValue = 0;
	EXPECT_TRUE(map.TryGetU64(String(_T("u64Key2")), outValue));
	EXPECT_EQ(outValue, 18446744073709551615ULL);

	EXPECT_FALSE(map.TryGetU64(String(_T("nonExistent")), outValue));
}

//////////////////////////////////////////////////////////////////////////////////////////
// SetGetFloat 테스트
//////////////////////////////////////////////////////////////////////////////////////////
TEST(DataMapTest, SetGetFloat) {
	LeakCheck;
	CDataMap map;

	// Float 데이터 설정 및 조회
	EXPECT_TRUE(map.SetFloat(String(_T("float1")), 0.0f));
	EXPECT_TRUE(map.SetFloat(String(_T("float2")), 3.141592f));
	EXPECT_TRUE(map.SetFloat(String(_T("float3")), -99.999f));

	// 업데이트 테스트
	EXPECT_TRUE(map.SetFloat(String(_T("float1")), 2.718f));

	EXPECT_FLOAT_EQ(map.GetFloat(String(_T("float1"))), 2.718f);
	EXPECT_FLOAT_EQ(map.GetFloat(String(_T("float2"))), 3.141592f);
	EXPECT_FLOAT_EQ(map.GetFloat(String(_T("float3"))), -99.999f);

	// TryGetFloat 테스트
	_f32 outValue = 0.0f;
	EXPECT_TRUE(map.TryGetFloat(String(_T("float2")), outValue));
	EXPECT_FLOAT_EQ(outValue, 3.141592f);

	EXPECT_FALSE(map.TryGetFloat(String(_T("nonExistent")), outValue));
}

//////////////////////////////////////////////////////////////////////////////////////////
// SetGetDouble 테스트
//////////////////////////////////////////////////////////////////////////////////////////
TEST(DataMapTest, SetGetDouble) {
	LeakCheck;
	CDataMap map;

	// Double 데이터 설정 및 조회
	EXPECT_TRUE(map.SetDouble(String(_T("double1")), 0.0));
	EXPECT_TRUE(map.SetDouble(String(_T("double2")), 2.718281828459045));
	EXPECT_TRUE(map.SetDouble(String(_T("double3")), -123.456789));

	// 업데이트 테스트
	EXPECT_TRUE(map.SetDouble(String(_T("double1")), 1.414213562373095));

	EXPECT_DOUBLE_EQ(map.GetDouble(String(_T("double1"))), 1.414213562373095);
	EXPECT_DOUBLE_EQ(map.GetDouble(String(_T("double2"))), 2.718281828459045);
	EXPECT_DOUBLE_EQ(map.GetDouble(String(_T("double3"))), -123.456789);

	// TryGetDouble 테스트
	_f64 outValue = 0.0;
	EXPECT_TRUE(map.TryGetDouble(String(_T("double2")), outValue));
	EXPECT_DOUBLE_EQ(outValue, 2.718281828459045);

	EXPECT_FALSE(map.TryGetDouble(String(_T("nonExistent")), outValue));
}

//////////////////////////////////////////////////////////////////////////////////////////
// SetGetPtr 테스트
//////////////////////////////////////////////////////////////////////////////////////////
TEST(DataMapTest, SetGetPtr) {
	LeakCheck;
	CDataMap map;

	int value1 = 100;
	int value2 = 200;
	int value3 = 300;

	// Ptr 데이터 설정 및 조회
	EXPECT_TRUE(map.SetPtr(String(_T("ptr1")), &value1));
	EXPECT_TRUE(map.SetPtr(String(_T("ptr2")), &value2));
	EXPECT_TRUE(map.SetPtr(String(_T("ptr3")), &value3));

	// 업데이트 테스트
	EXPECT_TRUE(map.SetPtr(String(_T("ptr1")), &value3));

	EXPECT_EQ(map.GetPtr(String(_T("ptr1"))), &value3);
	EXPECT_EQ(map.GetPtr(String(_T("ptr2"))), &value2);
	EXPECT_EQ(map.GetPtr(String(_T("ptr3"))), &value3);

	// Ptr 값 검증
	int* pValue = reinterpret_cast<int*>(map.GetPtr(String(_T("ptr2"))));
	EXPECT_EQ(*pValue, 200);

	// TryGetPtr 테스트
	_ptr outValue = 0;
	EXPECT_TRUE(map.TryGetPtr(String(_T("ptr1")), outValue));
	EXPECT_EQ(outValue, (_ptr)&value3);

	EXPECT_FALSE(map.TryGetPtr(String(_T("nonExistent")), outValue));
}

//////////////////////////////////////////////////////////////////////////////////////////
// SetGetString 테스트
//////////////////////////////////////////////////////////////////////////////////////////
TEST(DataMapTest, SetGetString) {
	LeakCheck;
	CDataMap map;

	// String 데이터 설정 및 조회
	EXPECT_TRUE(map.SetString(String(_T("str1")), String(_T("Hello"))));
	EXPECT_TRUE(map.SetString(String(_T("str2")), String(_T("World"))));
	EXPECT_TRUE(map.SetString(String(_T("str3")), String(_T(""))));

	// 업데이트 테스트 - 기존 값을 다른 값으로 변경
	EXPECT_TRUE(map.SetString(String(_T("str1")), String(_T("Updated"))));

	EXPECT_EQ(map.GetString(String(_T("str1"))), String(_T("Updated")));
	EXPECT_EQ(map.GetString(String(_T("str2"))), String(_T("World")));
	EXPECT_EQ(map.GetString(String(_T("str3"))), String(_T("")));

	// 문자열 재업데이트
	EXPECT_TRUE(map.SetString(String(_T("str1")), String(_T("Final"))));
	EXPECT_EQ(map.GetString(String(_T("str1"))), String(_T("Final")));

	// TryGetString 테스트
	String outValue;
	EXPECT_TRUE(map.TryGetString(String(_T("str2")), outValue));
	EXPECT_EQ(outValue, String(_T("World")));

	EXPECT_FALSE(map.TryGetString(String(_T("nonExistent")), outValue));
}

//////////////////////////////////////////////////////////////////////////////////////////
// SetGetBool 테스트
//////////////////////////////////////////////////////////////////////////////////////////
TEST(DataMapTest, SetGetBool) {
	LeakCheck;
	CDataMap map;

	// Bool 데이터 설정 및 조회
	EXPECT_TRUE(map.SetBool(String(_T("bool1")), true));
	EXPECT_TRUE(map.SetBool(String(_T("bool2")), false));
	EXPECT_TRUE(map.SetBool(String(_T("bool3")), true));

	// 업데이트 테스트
	EXPECT_TRUE(map.SetBool(String(_T("bool1")), false));

	EXPECT_FALSE(map.GetBool(String(_T("bool1"))));
	EXPECT_FALSE(map.GetBool(String(_T("bool2"))));
	EXPECT_TRUE(map.GetBool(String(_T("bool3"))));

	// TryGetBool 테스트
	bool outValue = false;
	EXPECT_TRUE(map.TryGetBool(String(_T("bool3")), outValue));
	EXPECT_TRUE(outValue);

	EXPECT_FALSE(map.TryGetBool(String(_T("nonExistent")), outValue));
}

//////////////////////////////////////////////////////////////////////////////////////////
// 혼합 데이터 타입 테스트
//////////////////////////////////////////////////////////////////////////////////////////
TEST(DataMapTest, MixedDataTypes) {
	LeakCheck;
	CDataMap map;

	// 다양한 타입의 데이터를 같은 맵에 저장
	EXPECT_TRUE(map.SetS32(String(_T("age")), 25));
	EXPECT_TRUE(map.SetFloat(String(_T("height")), 180.5f));
	EXPECT_TRUE(map.SetString(String(_T("name")), String(_T("John"))));
	EXPECT_TRUE(map.SetBool(String(_T("active")), true));
	EXPECT_TRUE(map.SetU32(String(_T("id")), 12345));

	EXPECT_EQ(map.GetS32(String(_T("age"))), 25);
	EXPECT_FLOAT_EQ(map.GetFloat(String(_T("height"))), 180.5f);
	EXPECT_EQ(map.GetString(String(_T("name"))), String(_T("John")));
	EXPECT_TRUE(map.GetBool(String(_T("active"))));
	EXPECT_EQ(map.GetU32(String(_T("id"))), 12345);

	// 크기 검증
	EXPECT_EQ(map.Size(), 5);
}

//////////////////////////////////////////////////////////////////////////////////////////
// 유틸리티 메서드 테스트 (Exist, Remove, Clear, Size, IsEmpty)
//////////////////////////////////////////////////////////////////////////////////////////
TEST(DataMapTest, UtilityMethods) {
	LeakCheck;
	CDataMap map;

	// 초기 상태 - 비어있음
	EXPECT_TRUE(map.IsEmpty());
	EXPECT_EQ(map.Size(), 0);

	// 데이터 추가
	EXPECT_TRUE(map.SetS32(String(_T("key1")), 100));
	EXPECT_TRUE(map.SetString(String(_T("key2")), String(_T("value"))));
	EXPECT_TRUE(map.SetFloat(String(_T("key3")), 3.14f));

	// 크기 및 Empty 상태 검증
	EXPECT_FALSE(map.IsEmpty());
	EXPECT_EQ(map.Size(), 3);

	// Exist 테스트
	EXPECT_TRUE(map.Exist(String(_T("key1"))));
	EXPECT_TRUE(map.Exist(String(_T("key2"))));
	EXPECT_TRUE(map.Exist(String(_T("key3"))));
	EXPECT_FALSE(map.Exist(String(_T("nonExistent"))));

	// Remove 테스트 - 개별 제거
	EXPECT_TRUE(map.Remove(String(_T("key2"))));
	EXPECT_EQ(map.Size(), 2);
	EXPECT_FALSE(map.Exist(String(_T("key2"))));

	// Remove 테스트 - 존재하지 않는 키 제거 시도
	EXPECT_FALSE(map.Remove(String(_T("nonExistent"))));
	EXPECT_EQ(map.Size(), 2);

	// Clear 테스트
	map.Clear();
	EXPECT_TRUE(map.IsEmpty());
	EXPECT_EQ(map.Size(), 0);
	EXPECT_FALSE(map.Exist(String(_T("key1"))));
	EXPECT_FALSE(map.Exist(String(_T("key3"))));
}

//////////////////////////////////////////////////////////////////////////////////////////
// 타입 불일치 테스트
//////////////////////////////////////////////////////////////////////////////////////////
TEST(DataMapTest, TypeMismatch) {
	LeakCheck;
	CDataMap map;

	// S32로 저장
	EXPECT_TRUE(map.SetS32(String(_T("number")), 42));

	// 다른 타입으로 조회 - 잘못된 타입이므로 기본값 반환
	EXPECT_EQ(map.GetU32(String(_T("number"))), 0);		// U32로 조회하면 0 반환
	EXPECT_FLOAT_EQ(map.GetFloat(String(_T("number"))), 0.0f);	// Float로 조회하면 0.0f 반환
	EXPECT_FALSE(map.GetBool(String(_T("number"))));	// Bool로 조회하면 false 반환
	EXPECT_EQ(map.GetString(String(_T("number"))), String());	// String으로 조회하면 빈 문자열 반환

	// TryGet 메서드로도 실패해야 함
	_u32 u32Value = 0;
	EXPECT_FALSE(map.TryGetU32(String(_T("number")), u32Value));

	_f32 floatValue = 0.0f;
	EXPECT_FALSE(map.TryGetFloat(String(_T("number")), floatValue));

	bool boolValue = false;
	EXPECT_FALSE(map.TryGetBool(String(_T("number")), boolValue));

	String strValue;
	EXPECT_FALSE(map.TryGetString(String(_T("number")), strValue));
}

//////////////////////////////////////////////////////////////////////////////////////////
// 같은 키 덮어쓰기 테스트
//////////////////////////////////////////////////////////////////////////////////////////
TEST(DataMapTest, OverwriteValues) {
	LeakCheck;
	CDataMap map;

	// 같은 키에 S32 저장
	EXPECT_TRUE(map.SetS32(String(_T("data")), 100));
	EXPECT_EQ(map.GetS32(String(_T("data"))), 100);

	// 같은 키에 다른 값으로 덮어쓰기
	EXPECT_TRUE(map.SetS32(String(_T("data")), 200));
	EXPECT_EQ(map.GetS32(String(_T("data"))), 200);
	EXPECT_EQ(map.Size(), 1);  // 크기는 1 유지

	// 같은 키에 String으로 변경
	EXPECT_TRUE(map.SetString(String(_T("data")), String(_T("text"))));
	EXPECT_EQ(map.GetString(String(_T("data"))), String(_T("text")));
	EXPECT_EQ(map.Size(), 1);  // 크기는 1 유지

	// 다시 S32로 변경
	EXPECT_TRUE(map.SetS32(String(_T("data")), 300));
	EXPECT_EQ(map.GetS32(String(_T("data"))), 300);
	EXPECT_EQ(map.Size(), 1);  // 크기는 1 유지
}

//////////////////////////////////////////////////////////////////////////////////////////
// 문자열 메모리 관리 테스트
//////////////////////////////////////////////////////////////////////////////////////////
TEST(DataMapTest, StringMemoryManagement) {
	LeakCheck;
	{
		CDataMap map;

		// 여러 문자열 저장
		EXPECT_TRUE(map.SetString(String(_T("str1")), String(_T("first"))));
		EXPECT_TRUE(map.SetString(String(_T("str2")), String(_T("second"))));
		EXPECT_TRUE(map.SetString(String(_T("str3")), String(_T("third"))));

		// 문자열 업데이트 (기존 메모리 해제 후 새로운 메모리 할당)
		EXPECT_TRUE(map.SetString(String(_T("str1")), String(_T("updated_first"))));
		EXPECT_EQ(map.GetString(String(_T("str1"))), String(_T("updated_first")));

		// Remove로 문자열 제거 (메모리 해제)
		EXPECT_TRUE(map.Remove(String(_T("str2"))));
		EXPECT_FALSE(map.Exist(String(_T("str2"))));

		// Clear로 모든 문자열 제거 (메모리 해제)
		map.Clear();
		EXPECT_TRUE(map.IsEmpty());
	}
	// 스코프 탈출 시 모든 할당된 메모리가 해제되어야 함
}

//////////////////////////////////////////////////////////////////////////////////////////
// AddData 메서드 테스트
//////////////////////////////////////////////////////////////////////////////////////////
TEST(DataMapTest, AddData) {
	LeakCheck;
	CDataMap map1;
	CDataMap map2;

	// map1에 데이터 추가
	EXPECT_TRUE(map1.SetS32(String(_T("key1")), 100));
	EXPECT_TRUE(map1.SetString(String(_T("key2")), String(_T("value1"))));

	// map2에 데이터 추가
	EXPECT_TRUE(map2.SetS32(String(_T("key3")), 200));
	EXPECT_TRUE(map2.SetString(String(_T("key2")), String(_T("value2"))));  // key2는 중복

	// Overwrite false - map1에 map2의 데이터를 병합하되 기존 키는 유지
	map1.AddData(map2, false);
	EXPECT_EQ(map1.GetS32(String(_T("key1"))), 100);
	EXPECT_EQ(map1.GetString(String(_T("key2"))), String(_T("value1")));  // 기존 값 유지
	EXPECT_EQ(map1.GetS32(String(_T("key3"))), 200);
	EXPECT_EQ(map1.Size(), 3);

	// 새로운 맵으로 테스트
	CDataMap map3;
	CDataMap map4;

	EXPECT_TRUE(map3.SetS32(String(_T("key1")), 100));
	EXPECT_TRUE(map3.SetString(String(_T("key2")), String(_T("value1"))));

	EXPECT_TRUE(map4.SetS32(String(_T("key3")), 200));
	EXPECT_TRUE(map4.SetString(String(_T("key2")), String(_T("value2"))));

	// Overwrite true - map3에 map4의 데이터를 병합하되 중복 키는 덮어쓰기
	map3.AddData(map4, true);
	EXPECT_EQ(map3.GetS32(String(_T("key1"))), 100);
	EXPECT_EQ(map3.GetString(String(_T("key2"))), String(_T("value2")));  // 덮어쓰여짐
	EXPECT_EQ(map3.GetS32(String(_T("key3"))), 200);
	EXPECT_EQ(map3.Size(), 3);
}

//////////////////////////////////////////////////////////////////////////////////////////
// 복사 생성자 및 이동 생성자 테스트
//////////////////////////////////////////////////////////////////////////////////////////
TEST(DataMapTest, CopyAndMoveConstructor) {
	LeakCheck;
	{
		CDataMap map1;
		EXPECT_TRUE(map1.SetS32(String(_T("key1")), 100));
		EXPECT_TRUE(map1.SetString(String(_T("key2")), String(_T("hello"))));
		EXPECT_TRUE(map1.SetFloat(String(_T("key3")), 3.14f));

		// 복사 생성자 테스트
		{
			CDataMap map2(map1);
			EXPECT_EQ(map2.GetS32(String(_T("key1"))), 100);
			EXPECT_EQ(map2.GetString(String(_T("key2"))), String(_T("hello")));
			EXPECT_FLOAT_EQ(map2.GetFloat(String(_T("key3"))), 3.14f);
			EXPECT_EQ(map2.Size(), 3);

			// 원본 수정이 복사본에 영향을 주지 않음
			EXPECT_TRUE(map1.SetS32(String(_T("key1")), 200));
			EXPECT_EQ(map2.GetS32(String(_T("key1"))), 100);
		}

		// 이동 생성자 테스트
		CDataMap map3(Move(map1));
		EXPECT_EQ(map3.GetS32(String(_T("key1"))), 200);
		EXPECT_EQ(map3.GetString(String(_T("key2"))), String(_T("hello")));
		EXPECT_FLOAT_EQ(map3.GetFloat(String(_T("key3"))), 3.14f);
		EXPECT_EQ(map3.Size(), 3);
	}
}

//////////////////////////////////////////////////////////////////////////////////////////
// 할당 연산자 테스트
//////////////////////////////////////////////////////////////////////////////////////////
TEST(DataMapTest, AssignmentOperator) {
	LeakCheck;
	{
		CDataMap map1;
		EXPECT_TRUE(map1.SetS32(String(_T("key1")), 100));
		EXPECT_TRUE(map1.SetString(String(_T("key2")), String(_T("hello"))));

		CDataMap map2;
		EXPECT_TRUE(map2.SetS32(String(_T("key3")), 300));

		// 복사 할당 테스트
		map2 = map1;
		EXPECT_EQ(map2.GetS32(String(_T("key1"))), 100);
		EXPECT_EQ(map2.GetString(String(_T("key2"))), String(_T("hello")));
		EXPECT_FALSE(map2.Exist(String(_T("key3"))));  // 기존 데이터는 제거됨
		EXPECT_EQ(map2.Size(), 2);

		CDataMap map3;
		EXPECT_TRUE(map3.SetS32(String(_T("keyA")), 1000));

		// 이동 할당 테스트
		map3 = Move(map1);
		EXPECT_EQ(map3.GetS32(String(_T("key1"))), 100);
		EXPECT_EQ(map3.GetString(String(_T("key2"))), String(_T("hello")));
		EXPECT_FALSE(map3.Exist(String(_T("keyA"))));
		EXPECT_EQ(map3.Size(), 2);
	}
}

#endif
