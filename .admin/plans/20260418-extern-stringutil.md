# 계획: Extern_StringUtil 구현

## 날짜
2026-04-18

## 작업 목표
- `StringUtil` 클래스에 `ToStringBuffered<TNumber>` 템플릿 추가
- `ToString<TInteger>` → `ToString<TNumber>` 변경 후 `ToStringBuffered` 활용 재구현
- `Extern_StringUtil.h/cpp` 신규 생성 (MASM용 extern "C" 래퍼)
- `Extern.asm`에 StringUtil 섹션 추가

## 영향 파일
1. `Projects/jc/Sources/jc/Primitives/StringUtil.h`
2. `Projects/jc/Sources/jc/Primitives/StringUtil.cpp`
3. `Projects/jc/Sources/jc/_Extern/Extern_StringUtil.h` (신규)
4. `Projects/jc/Sources/jc/_Extern/Extern_StringUtil.cpp` (신규)
5. `Projects/jc/Sources/jc/_Extern/Extern.asm`

## 설계

### StringUtil.h 변경 사항
- `template <typename TNumber> static int ToStringBuffered(char* _pBuff, int _capacity, TNumber _value)` 추가
- `template <typename TInteger> static String ToString(...)` → `template <typename TNumber>` 변경

### ToStringBuffered 특수화 목록 (StringUtil.cpp)
- `_u32`: `snprintf(..., "%u", ...)`
- `_s32`: `snprintf(..., "%d", ...)`
- `_u64`: `snprintf(..., "%llu", ...)`
- `_s64`: `snprintf(..., "%lld", ...)`
- `_f32`: `snprintf(..., "%g", ...)`
- `_f64`: `snprintf(..., "%g", ...)`
- 기타 정수 타입 (_s8, _u8, _s16, _u16, _s32l, _u32l, _s64, _u64, bool)

### ToString 재구현
- `ToStringBuffered`를 활용하여 char 버퍼로 변환 후 `String` 반환
- `bool`/`const String&`/`String&&` 특수화는 별도 유지

### Extern_StringUtil.h 함수 목록
```c
_s32 StringUtil_ToStringBuffered_U32(_s8* _pBuff, _s32 _capacity, _u32 _value);
_s32 StringUtil_ToStringBuffered_S32(_s8* _pBuff, _s32 _capacity, _s32 _value);
_s32 StringUtil_ToStringBuffered_U64(_s8* _pBuff, _s32 _capacity, _u64 _value);
_s32 StringUtil_ToStringBuffered_S64(_s8* _pBuff, _s32 _capacity, _s64 _value);
_s32 StringUtil_ToStringBuffered_Float(_s8* _pBuff, _s32 _capacity, _f32 _value);
_s32 StringUtil_ToStringBuffered_Double(_s8* _pBuff, _s32 _capacity, _f64 _value);
_s32 StringUtil_Length(const _s8* _pStr);
_s32 StringUtil_LengthWithNull(const _s8* _pStr);
_s32 StringUtil_IsNullOrEmpty(const _s8* _pStr);
_s32 StringUtil_Copy(_s8* _pBuffer, _s32 _bufferSize, const _s8* _pCopy);
_s32 StringUtil_CopyUnsafe(_s8* _pBuffer, const _s8* _pCopy);
_s32 StringUtil_IsEqual(const _s8* _pSrc, const _s8* _pDst, _s32 _bCompareCase);
_s32 StringUtil_IsEqualLen(const _s8* _pSrc, _s32 _srcLen, const _s8* _pDst, _s32 _dstLen, _s32 _bCompareCase);
_s32 StringUtil_Find(const _s8* _pSource, _s32 _sourceLen, _s32 _startIdx, _s32 _endIdx, const _s8* _pStr, _s32 _caseSensitive);
_s32 StringUtil_FindLen(const _s8* _pSource, _s32 _sourceLen, _s32 _startIdx, _s32 _endIdx, const _s8* _pStr, _s32 _strLen, _s32 _caseSensitive);
_s32 StringUtil_FindFrom(const _s8* _pSource, _s32 _sourceLen, _s32 _startIdx, const _s8* _pStr, _s32 _caseSensitive);
_s32 StringUtil_FindAll(_s32* _pPositionArray, const _s8* _pSource, const _s8* _pStr, _s32 _caseSensitive);
_s32 StringUtil_FindAllLen(_s32* _pPositionArray, const _s8* _pSource, _s32 _sourceLen, const _s8* _pStr, _s32 _caseSensitive);
_s32 StringUtil_FindAllRange(_s32* _pPositionArray, const _s8* _pSource, _s32 _sourceLen, _s32 _startIdx, _s32 _endIdx, const _s8* _pStr, _s32 _caseSensitive);
_s32 StringUtil_FindChar(const _s8* _pSource, _s8 _ch);
_s32 StringUtil_FindCharReverse(const _s8* _pSource, _s8 _ch);
_s32 StringUtil_FindCharReverseLen(const _s8* _pSource, _s32 _len, _s8 _ch);
_s32 StringUtil_FindCharUncontained(const _s8* _pSource, _s8 _ch);
```
※ `Swap(String&, String&)` - jc::String 객체 전달이 필요하므로 extern "C" 포팅 불가

## 비고
- `bool`은 TNumber가 아닌 특수 케이스이므로 ToString<bool>은 별도 유지
- `const String&`, `String&&` 특수화도 TNumber와 별개로 유지
