/*
	작성자 : 윤정도
	CDataMap 클래스를 테스트합니다.
*/


#include <jc_gtest/Core.h>

#include <jc/Core.h>
#include <jc/Container/DataMap.h>

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
	EXPECT_TRUE(map.SetS32(String("intKey1"), -12345));
	EXPECT_TRUE(map.SetS32(String("intKey2"), 0));
	EXPECT_TRUE(map.SetS32(String("intKey3"), 2147483647));

	// 기존 키에 대해 다시 설정 - 업데이트되어야 함
	EXPECT_TRUE(map.SetS32(String("intKey1"), 54321));

	EXPECT_EQ(map.GetS32(String("intKey1")), 54321);
	EXPECT_EQ(map.GetS32(String("intKey2")), 0);
	EXPECT_EQ(map.GetS32(String("intKey3")), 2147483647);

	// 존재하지 않는 키 조회 - 기본값 0 반환
	EXPECT_EQ(map.GetS32(String("nonExistent")), 0);

	// TryGetS32 테스트
	_s32 outValue = 0;
	EXPECT_TRUE(map.TryGetS32(String("intKey1"), outValue));
	EXPECT_EQ(outValue, 54321);

	EXPECT_FALSE(map.TryGetS32(String("nonExistent"), outValue));
}

//////////////////////////////////////////////////////////////////////////////////////////
// SetGetU32 테스트
//////////////////////////////////////////////////////////////////////////////////////////
TEST(DataMapTest, SetGetU32) {
	LeakCheck;
	CDataMap map;

	// U32 데이터 설정 및 조회
	EXPECT_TRUE(map.SetU32(String("uint1"), 0));
	EXPECT_TRUE(map.SetU32(String("uint2"), 4294967295));
	EXPECT_TRUE(map.SetU32(String("uint3"), 2000000000));

	// 업데이트 테스트
	EXPECT_TRUE(map.SetU32(String("uint1"), 1234567890));

	EXPECT_EQ(map.GetU32(String("uint1")), 1234567890);
	EXPECT_EQ(map.GetU32(String("uint2")), 4294967295);
	EXPECT_EQ(map.GetU32(String("uint3")), 2000000000);

	// TryGetU32 테스트
	_u32 outValue = 0;
	EXPECT_TRUE(map.TryGetU32(String("uint2"), outValue));
	EXPECT_EQ(outValue, 4294967295);

	EXPECT_FALSE(map.TryGetU32(String("nonExistent"), outValue));
}

//////////////////////////////////////////////////////////////////////////////////////////
// SetGetS64 테스트
//////////////////////////////////////////////////////////////////////////////////////////
TEST(DataMapTest, SetGetS64) {
	LeakCheck;
	CDataMap map;

	// S64 데이터 설정 및 조회
	EXPECT_TRUE(map.SetS64(String("s64Key1"), -9223372036854775800LL));
	EXPECT_TRUE(map.SetS64(String("s64Key2"), 0));
	EXPECT_TRUE(map.SetS64(String("s64Key3"), 9223372036854775800LL));

	// 업데이트 테스트
	EXPECT_TRUE(map.SetS64(String("s64Key1"), 1234567890123456789LL));

	EXPECT_EQ(map.GetS64(String("s64Key1")), 1234567890123456789LL);
	EXPECT_EQ(map.GetS64(String("s64Key2")), 0);
	EXPECT_EQ(map.GetS64(String("s64Key3")), 9223372036854775800LL);

	// TryGetS64 테스트
	_s64 outValue = 0;
	EXPECT_TRUE(map.TryGetS64(String("s64Key3"), outValue));
	EXPECT_EQ(outValue, 9223372036854775800LL);

	EXPECT_FALSE(map.TryGetS64(String("nonExistent"), outValue));
}

//////////////////////////////////////////////////////////////////////////////////////////
// SetGetU64 테스트
//////////////////////////////////////////////////////////////////////////////////////////
TEST(DataMapTest, SetGetU64) {
	LeakCheck;
	CDataMap map;

	// U64 데이터 설정 및 조회
	EXPECT_TRUE(map.SetU64(String("u64Key1"), 0));
	EXPECT_TRUE(map.SetU64(String("u64Key2"), 18446744073709551615ULL));
	EXPECT_TRUE(map.SetU64(String("u64Key3"), 9999999999999999999ULL));

	// 업데이트 테스트
	EXPECT_TRUE(map.SetU64(String("u64Key1"), 12345678901234567890ULL));

	EXPECT_EQ(map.GetU64(String("u64Key1")), 12345678901234567890ULL);
	EXPECT_EQ(map.GetU64(String("u64Key2")), 18446744073709551615ULL);
	EXPECT_EQ(map.GetU64(String("u64Key3")), 9999999999999999999ULL);

	// TryGetU64 테스트
	_u64 outValue = 0;
	EXPECT_TRUE(map.TryGetU64(String("u64Key2"), outValue));
	EXPECT_EQ(outValue, 18446744073709551615ULL);

	EXPECT_FALSE(map.TryGetU64(String("nonExistent"), outValue));
}

//////////////////////////////////////////////////////////////////////////////////////////
// SetGetFloat 테스트
//////////////////////////////////////////////////////////////////////////////////////////
TEST(DataMapTest, SetGetFloat) {
	LeakCheck;
	CDataMap map;

	// Float 데이터 설정 및 조회
	EXPECT_TRUE(map.SetFloat(String("float1"), 0.0f));
	EXPECT_TRUE(map.SetFloat(String("float2"), 3.141592f));
	EXPECT_TRUE(map.SetFloat(String("float3"), -99.999f));

	// 업데이트 테스트
	EXPECT_TRUE(map.SetFloat(String("float1"), 2.718f));

	EXPECT_FLOAT_EQ(map.GetFloat(String("float1")), 2.718f);
	EXPECT_FLOAT_EQ(map.GetFloat(String("float2")), 3.141592f);
	EXPECT_FLOAT_EQ(map.GetFloat(String("float3")), -99.999f);

	// TryGetFloat 테스트
	_f32 outValue = 0.0f;
	EXPECT_TRUE(map.TryGetFloat(String("float2"), outValue));
	EXPECT_FLOAT_EQ(outValue, 3.141592f);

	EXPECT_FALSE(map.TryGetFloat(String("nonExistent"), outValue));
}

//////////////////////////////////////////////////////////////////////////////////////////
// SetGetDouble 테스트
//////////////////////////////////////////////////////////////////////////////////////////
TEST(DataMapTest, SetGetDouble) {
	LeakCheck;
	CDataMap map;

	// Double 데이터 설정 및 조회
	EXPECT_TRUE(map.SetDouble(String("double1"), 0.0));
	EXPECT_TRUE(map.SetDouble(String("double2"), 2.718281828459045));
	EXPECT_TRUE(map.SetDouble(String("double3"), -123.456789));

	// 업데이트 테스트
	EXPECT_TRUE(map.SetDouble(String("double1"), 1.414213562373095));

	EXPECT_DOUBLE_EQ(map.GetDouble(String("double1")), 1.414213562373095);
	EXPECT_DOUBLE_EQ(map.GetDouble(String("double2")), 2.718281828459045);
	EXPECT_DOUBLE_EQ(map.GetDouble(String("double3")), -123.456789);

	// TryGetDouble 테스트
	_f64 outValue = 0.0;
	EXPECT_TRUE(map.TryGetDouble(String("double2"), outValue));
	EXPECT_DOUBLE_EQ(outValue, 2.718281828459045);

	EXPECT_FALSE(map.TryGetDouble(String("nonExistent"), outValue));
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
	EXPECT_TRUE(map.SetPtr(String("ptr1"), &value1));
	EXPECT_TRUE(map.SetPtr(String("ptr2"), &value2));
	EXPECT_TRUE(map.SetPtr(String("ptr3"), &value3));

	// 업데이트 테스트
	EXPECT_TRUE(map.SetPtr(String("ptr1"), &value3));

	EXPECT_EQ(map.GetPtr(String("ptr1")), &value3);
	EXPECT_EQ(map.GetPtr(String("ptr2")), &value2);
	EXPECT_EQ(map.GetPtr(String("ptr3")), &value3);

	// Ptr 값 검증
	int* pValue = reinterpret_cast<int*>(map.GetPtr(String("ptr2")));
	EXPECT_EQ(*pValue, 200);

	// TryGetPtr 테스트
	_ptr outValue = 0;
	EXPECT_TRUE(map.TryGetPtr(String("ptr1"), outValue));
	EXPECT_EQ(outValue, (_ptr)&value3);

	EXPECT_FALSE(map.TryGetPtr(String("nonExistent"), outValue));
}

//////////////////////////////////////////////////////////////////////////////////////////
// SetGetString 테스트
//////////////////////////////////////////////////////////////////////////////////////////
TEST(DataMapTest, SetGetString) {
	LeakCheck;
	CDataMap map;

	// String 데이터 설정 및 조회
	EXPECT_TRUE(map.SetString(String("str1"), String("Hello")));
	EXPECT_TRUE(map.SetString(String("str2"), String("World")));
	EXPECT_TRUE(map.SetString(String("str3"), String("")));

	// 업데이트 테스트 - 기존 값을 다른 값으로 변경
	EXPECT_TRUE(map.SetString(String("str1"), String("Updated")));

	EXPECT_EQ(map.GetString(String("str1")), String("Updated"));
	EXPECT_EQ(map.GetString(String("str2")), String("World"));
	EXPECT_EQ(map.GetString(String("str3")), String(""));

	// 문자열 재업데이트
	EXPECT_TRUE(map.SetString(String("str1"), String("Final")));
	EXPECT_EQ(map.GetString(String("str1")), String("Final"));

	// TryGetString 테스트
	String outValue;
	EXPECT_TRUE(map.TryGetString(String("str2"), outValue));
	EXPECT_EQ(outValue, String("World"));

	EXPECT_FALSE(map.TryGetString(String("nonExistent"), outValue));
}

//////////////////////////////////////////////////////////////////////////////////////////
// SetGetBool 테스트
//////////////////////////////////////////////////////////////////////////////////////////
TEST(DataMapTest, SetGetBool) {
	LeakCheck;
	CDataMap map;

	// Bool 데이터 설정 및 조회
	EXPECT_TRUE(map.SetBool(String("bool1"), true));
	EXPECT_TRUE(map.SetBool(String("bool2"), false));
	EXPECT_TRUE(map.SetBool(String("bool3"), true));

	// 업데이트 테스트
	EXPECT_TRUE(map.SetBool(String("bool1"), false));

	EXPECT_FALSE(map.GetBool(String("bool1")));
	EXPECT_FALSE(map.GetBool(String("bool2")));
	EXPECT_TRUE(map.GetBool(String("bool3")));

	// TryGetBool 테스트
	bool outValue = false;
	EXPECT_TRUE(map.TryGetBool(String("bool3"), outValue));
	EXPECT_TRUE(outValue);

	EXPECT_FALSE(map.TryGetBool(String("nonExistent"), outValue));
}

//////////////////////////////////////////////////////////////////////////////////////////
// 혼합 데이터 타입 테스트
//////////////////////////////////////////////////////////////////////////////////////////
TEST(DataMapTest, MixedDataTypes) {
	LeakCheck;
	CDataMap map;

	// 다양한 타입의 데이터를 같은 맵에 저장
	EXPECT_TRUE(map.SetS32(String("age"), 25));
	EXPECT_TRUE(map.SetFloat(String("height"), 180.5f));
	EXPECT_TRUE(map.SetString(String("name"), String("John")));
	EXPECT_TRUE(map.SetBool(String("active"), true));
	EXPECT_TRUE(map.SetU32(String("id"), 12345));

	EXPECT_EQ(map.GetS32(String("age")), 25);
	EXPECT_FLOAT_EQ(map.GetFloat(String("height")), 180.5f);
	EXPECT_EQ(map.GetString(String("name")), String("John"));
	EXPECT_TRUE(map.GetBool(String("active")));
	EXPECT_EQ(map.GetU32(String("id")), 12345);

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
	EXPECT_TRUE(map.SetS32(String("key1"), 100));
	EXPECT_TRUE(map.SetString(String("key2"), String("value")));
	EXPECT_TRUE(map.SetFloat(String("key3"), 3.14f));

	// 크기 및 Empty 상태 검증
	EXPECT_FALSE(map.IsEmpty());
	EXPECT_EQ(map.Size(), 3);

	// Exist 테스트
	EXPECT_TRUE(map.Exist(String("key1")));
	EXPECT_TRUE(map.Exist(String("key2")));
	EXPECT_TRUE(map.Exist(String("key3")));
	EXPECT_FALSE(map.Exist(String("nonExistent")));

	// Remove 테스트 - 개별 제거
	EXPECT_TRUE(map.Remove(String("key2")));
	EXPECT_EQ(map.Size(), 2);
	EXPECT_FALSE(map.Exist(String("key2")));

	// Remove 테스트 - 존재하지 않는 키 제거 시도
	EXPECT_FALSE(map.Remove(String("nonExistent")));
	EXPECT_EQ(map.Size(), 2);

	// Clear 테스트
	map.Clear();
	EXPECT_TRUE(map.IsEmpty());
	EXPECT_EQ(map.Size(), 0);
	EXPECT_FALSE(map.Exist(String("key1")));
	EXPECT_FALSE(map.Exist(String("key3")));
}

//////////////////////////////////////////////////////////////////////////////////////////
// 타입 불일치 테스트
//////////////////////////////////////////////////////////////////////////////////////////
TEST(DataMapTest, TypeMismatch) {
	LeakCheck;
	CDataMap map;

	// S32로 저장
	EXPECT_TRUE(map.SetS32(String("number"), 42));

	// 다른 타입으로 조회 - 잘못된 타입이므로 기본값 반환
	EXPECT_EQ(map.GetU32(String("number")), 0);		// U32로 조회하면 0 반환
	EXPECT_FLOAT_EQ(map.GetFloat(String("number")), 0.0f);	// Float로 조회하면 0.0f 반환
	EXPECT_FALSE(map.GetBool(String("number")));	// Bool로 조회하면 false 반환
	EXPECT_EQ(map.GetString(String("number")), String());	// String으로 조회하면 빈 문자열 반환

	// TryGet 메서드로도 실패해야 함
	_u32 u32Value = 0;
	EXPECT_FALSE(map.TryGetU32(String("number"), u32Value));

	_f32 floatValue = 0.0f;
	EXPECT_FALSE(map.TryGetFloat(String("number"), floatValue));

	bool boolValue = false;
	EXPECT_FALSE(map.TryGetBool(String("number"), boolValue));

	String strValue;
	EXPECT_FALSE(map.TryGetString(String("number"), strValue));
}

//////////////////////////////////////////////////////////////////////////////////////////
// 같은 키 덮어쓰기 테스트
//////////////////////////////////////////////////////////////////////////////////////////
TEST(DataMapTest, OverwriteValues) {
	LeakCheck;
	CDataMap map;

	// 같은 키에 S32 저장
	EXPECT_TRUE(map.SetS32(String("data"), 100));
	EXPECT_EQ(map.GetS32(String("data")), 100);

	// 같은 키에 다른 값으로 덮어쓰기
	EXPECT_TRUE(map.SetS32(String("data"), 200));
	EXPECT_EQ(map.GetS32(String("data")), 200);
	EXPECT_EQ(map.Size(), 1);  // 크기는 1 유지

	// 같은 키에 String으로 변경
	EXPECT_TRUE(map.SetString(String("data"), String("text")));
	EXPECT_EQ(map.GetString(String("data")), String("text"));
	EXPECT_EQ(map.Size(), 1);  // 크기는 1 유지

	// 다시 S32로 변경
	EXPECT_TRUE(map.SetS32(String("data"), 300));
	EXPECT_EQ(map.GetS32(String("data")), 300);
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
		EXPECT_TRUE(map.SetString(String("str1"), String("first")));
		EXPECT_TRUE(map.SetString(String("str2"), String("second")));
		EXPECT_TRUE(map.SetString(String("str3"), String("third")));

		// 문자열 업데이트 (기존 메모리 해제 후 새로운 메모리 할당)
		EXPECT_TRUE(map.SetString(String("str1"), String("updated_first")));
		EXPECT_EQ(map.GetString(String("str1")), String("updated_first"));

		// Remove로 문자열 제거 (메모리 해제)
		EXPECT_TRUE(map.Remove(String("str2")));
		EXPECT_FALSE(map.Exist(String("str2")));

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
	EXPECT_TRUE(map1.SetS32(String("key1"), 100));
	EXPECT_TRUE(map1.SetString(String("key2"), String("value1")));

	// map2에 데이터 추가
	EXPECT_TRUE(map2.SetS32(String("key3"), 200));
	EXPECT_TRUE(map2.SetString(String("key2"), String("value2")));  // key2는 중복

	// Overwrite false - map1에 map2의 데이터를 병합하되 기존 키는 유지
	map1.AddData(map2, false);
	EXPECT_EQ(map1.GetS32(String("key1")), 100);
	EXPECT_EQ(map1.GetString(String("key2")), String("value1"));  // 기존 값 유지
	EXPECT_EQ(map1.GetS32(String("key3")), 200);
	EXPECT_EQ(map1.Size(), 3);

	// 새로운 맵으로 테스트
	CDataMap map3;
	CDataMap map4;

	EXPECT_TRUE(map3.SetS32(String("key1"), 100));
	EXPECT_TRUE(map3.SetString(String("key2"), String("value1")));

	EXPECT_TRUE(map4.SetS32(String("key3"), 200));
	EXPECT_TRUE(map4.SetString(String("key2"), String("value2")));

	// Overwrite true - map3에 map4의 데이터를 병합하되 중복 키는 덮어쓰기
	map3.AddData(map4, true);
	EXPECT_EQ(map3.GetS32(String("key1")), 100);
	EXPECT_EQ(map3.GetString(String("key2")), String("value2"));  // 덮어쓰여짐
	EXPECT_EQ(map3.GetS32(String("key3")), 200);
	EXPECT_EQ(map3.Size(), 3);
}

//////////////////////////////////////////////////////////////////////////////////////////
// 복사 생성자 및 이동 생성자 테스트
//////////////////////////////////////////////////////////////////////////////////////////
TEST(DataMapTest, CopyAndMoveConstructor) {
	LeakCheck;
	{
		CDataMap map1;
		EXPECT_TRUE(map1.SetS32(String("key1"), 100));
		EXPECT_TRUE(map1.SetString(String("key2"), String("hello")));
		EXPECT_TRUE(map1.SetFloat(String("key3"), 3.14f));

		// 복사 생성자 테스트
		{
			CDataMap map2(map1);
			EXPECT_EQ(map2.GetS32(String("key1")), 100);
			EXPECT_EQ(map2.GetString(String("key2")), String("hello"));
			EXPECT_FLOAT_EQ(map2.GetFloat(String("key3")), 3.14f);
			EXPECT_EQ(map2.Size(), 3);

			// 원본 수정이 복사본에 영향을 주지 않음
			EXPECT_TRUE(map1.SetS32(String("key1"), 200));
			EXPECT_EQ(map2.GetS32(String("key1")), 100);
		}

		// 이동 생성자 테스트
		CDataMap map3(Move(map1));
		EXPECT_EQ(map3.GetS32(String("key1")), 200);
		EXPECT_EQ(map3.GetString(String("key2")), String("hello"));
		EXPECT_FLOAT_EQ(map3.GetFloat(String("key3")), 3.14f);
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
		EXPECT_TRUE(map1.SetS32(String("key1"), 100));
		EXPECT_TRUE(map1.SetString(String("key2"), String("hello")));

		CDataMap map2;
		EXPECT_TRUE(map2.SetS32(String("key3"), 300));

		// 복사 할당 테스트
		map2 = map1;
		EXPECT_EQ(map2.GetS32(String("key1")), 100);
		EXPECT_EQ(map2.GetString(String("key2")), String("hello"));
		EXPECT_FALSE(map2.Exist(String("key3")));  // 기존 데이터는 제거됨
		EXPECT_EQ(map2.Size(), 2);

		CDataMap map3;
		EXPECT_TRUE(map3.SetS32(String("keyA"), 1000));

		// 이동 할당 테스트
		map3 = Move(map1);
		EXPECT_EQ(map3.GetS32(String("key1")), 100);
		EXPECT_EQ(map3.GetString(String("key2")), String("hello"));
		EXPECT_FALSE(map3.Exist(String("keyA")));
		EXPECT_EQ(map3.Size(), 2);
	}
}

#endif
