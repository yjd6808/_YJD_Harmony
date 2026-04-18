# 작업 히스토리: Extern_StringUtil 구현

## 날짜
2026-04-18

## 작업 결과
빌드 성공 (경고 0개, 오류 0개)

## 변경된 파일 목록

### 신규 생성
1. `Projects/jc/Sources/jc/_Extern/Extern_StringUtil.h`
   - MASM용 extern "C" 함수 선언
2. `Projects/jc/Sources/jc/_Extern/Extern_StringUtil.cpp`
   - extern "C" 래퍼 구현

### 수정
3. `Projects/jc/Sources/jc/Primitives/StringUtil.h`
   - `template <typename TNumber> static int ToStringBuffered(char* _pBuff, int _capacity, TNumber _value)` 추가
   - `template <typename TInteger> static String ToString(...)` → `template <typename TNumber>` 변경
4. `Projects/jc/Sources/jc/Primitives/StringUtil.cpp`
   - `ToStringBuffered` 특수화 구현 (bool, float, double, _s8, _u8, _s16, _u16, _s32, _u32, _s32l, _u32l, _s64, _u64)
   - `ToString` 특수화를 `std::to_string` 대신 `ToStringBuffered` 활용으로 재구현
5. `Projects/jc/Sources/jc/_Extern/Extern.asm`
   - StringUtil 관련 extern 함수 선언 섹션 추가 (24개 함수)
6. `Projects/jc/Project/jc.vcxproj`
   - `Extern_StringUtil.cpp`, `Extern_StringUtil.h` ClCompile/ClInclude 항목 추가

## 제공된 extern "C" 함수 목록

### ToStringBuffered 계열
- `StringUtil_ToStringBuffered_U32`
- `StringUtil_ToStringBuffered_S32`
- `StringUtil_ToStringBuffered_U64`
- `StringUtil_ToStringBuffered_S64`
- `StringUtil_ToStringBuffered_Float`
- `StringUtil_ToStringBuffered_Double`

### 문자열 유틸리티
- `StringUtil_Length`, `StringUtil_LengthWithNull`
- `StringUtil_IsNullOrEmpty`
- `StringUtil_Copy`, `StringUtil_CopyUnsafe`
- `StringUtil_IsEqual`, `StringUtil_IsEqualLen`
- `StringUtil_Find`, `StringUtil_FindLen`, `StringUtil_FindFrom`
- `StringUtil_FindAll`, `StringUtil_FindAllLen`, `StringUtil_FindAllRange`
- `StringUtil_FindChar`, `StringUtil_FindCharReverse`, `StringUtil_FindCharReverseLen`
- `StringUtil_FindCharUncontained`

## 미포팅 함수
- `Swap(String&, String&)` - jc::String 객체 직접 전달이 필요하여 extern "C" 포팅 불가

## 비고
- float/double은 `%g` 포맷 사용 (기존 `std::to_string`의 `%f` 대비 trailing zero 제거)
- bool은 `ToStringBuffered` 특수화에서 "true"/"false" 문자열 반환
