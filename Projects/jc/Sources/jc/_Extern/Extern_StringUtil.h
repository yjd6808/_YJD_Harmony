/*
 * 작성자 : 윤정도
 * 목적 : MASM 어셈블리 코드에서 StringUtil API를 호출하기 위한 extern "C" 래퍼
 * created by AI.
 */

#pragma once

#ifdef __cplusplus
#include "jc/Type.h"
extern "C" {
#endif

/*
 * 숫자를 문자열 버퍼에 기록 (unsigned 32-bit)
 * @param _pBuff     : 출력 버퍼
 * @param _capacity  : 버퍼 크기
 * @param _value     : 변환할 값
 * @return 기록된 문자 개수 (null 제외), 실패 시 음수
 */
_s32 StringUtil_ToStringBuffered_U32(_s8* _pBuff, _s32 _capacity, _u32 _value);

/*
 * 숫자를 문자열 버퍼에 기록 (signed 32-bit)
 */
_s32 StringUtil_ToStringBuffered_S32(_s8* _pBuff, _s32 _capacity, _s32 _value);

/*
 * 숫자를 문자열 버퍼에 기록 (unsigned 64-bit)
 */
_s32 StringUtil_ToStringBuffered_U64(_s8* _pBuff, _s32 _capacity, _u64 _value);

/*
 * 숫자를 문자열 버퍼에 기록 (signed 64-bit)
 */
_s32 StringUtil_ToStringBuffered_S64(_s8* _pBuff, _s32 _capacity, _s64 _value);

/*
 * 숫자를 문자열 버퍼에 기록 (float)
 */
_s32 StringUtil_ToStringBuffered_Float(_s8* _pBuff, _s32 _capacity, _f32 _value);

/*
 * 숫자를 문자열 버퍼에 기록 (double)
 */
_s32 StringUtil_ToStringBuffered_Double(_s8* _pBuff, _s32 _capacity, _f64 _value);

/*
 * 문자열을 숫자로 변환 (signed 32-bit)
 * @param _pStr              : 변환할 문자열
 * @param _ppEndptr          : 변환 종료 위치 포인터 (불필요 시 NULL 전달)
 * @param _ignoreLeadingZero : 1 = 선행 0 무시, 0 = 그대로 처리
 * @return 변환된 값
 */
_s32 StringUtil_ToNumber_S32(const _s8* _pStr, _s8** _ppEndptr = NULL, _s32 _ignoreLeadingZero = 1);

/*
 * 문자열을 숫자로 변환 (unsigned 32-bit)
 */
_u32 StringUtil_ToNumber_U32(const _s8* _pStr, _s8** _ppEndptr = NULL, _s32 _ignoreLeadingZero = 1);

/*
 * 문자열을 숫자로 변환 (signed 64-bit)
 */
_s64 StringUtil_ToNumber_S64(const _s8* _pStr, _s8** _ppEndptr = NULL, _s32 _ignoreLeadingZero = 1);

/*
 * 문자열을 숫자로 변환 (unsigned 64-bit)
 */
_u64 StringUtil_ToNumber_U64(const _s8* _pStr, _s8** _ppEndptr = NULL, _s32 _ignoreLeadingZero = 1);

/*
 * 문자열을 숫자로 변환 (float)
 */
_f32 StringUtil_ToNumber_Float(const _s8* _pStr, _s8** _ppEndptr = NULL, _s32 _ignoreLeadingZero = 1);

/*
 * 문자열을 숫자로 변환 (double)
 */
_f64 StringUtil_ToNumber_Double(const _s8* _pStr, _s8** _ppEndptr = NULL, _s32 _ignoreLeadingZero = 1);

/*
 * 문자열을 숫자로 안전하게 변환 (signed 32-bit)
 * @param _pVal              : 변환 결과를 저장할 포인터
 * @param _pStr              : 변환할 문자열
 * @param _ignoreLeadingZero : 1 = 선행 0 무시, 0 = 그대로 처리
 * @return 1 if successful, 0 otherwise
 */
_s32 StringUtil_TryToNumber_S32(_s32* _pVal, const _s8* _pStr, _s32 _ignoreLeadingZero = 1);

/*
 * 문자열을 숫자로 안전하게 변환 (unsigned 32-bit)
 */
_s32 StringUtil_TryToNumber_U32(_u32* _pVal, const _s8* _pStr, _s32 _ignoreLeadingZero = 1);

/*
 * 문자열을 숫자로 안전하게 변환 (signed 64-bit)
 */
_s32 StringUtil_TryToNumber_S64(_s64* _pVal, const _s8* _pStr, _s32 _ignoreLeadingZero = 1);

/*
 * 문자열을 숫자로 안전하게 변환 (unsigned 64-bit)
 */
_s32 StringUtil_TryToNumber_U64(_u64* _pVal, const _s8* _pStr, _s32 _ignoreLeadingZero = 1);

/*
 * 문자열을 숫자로 안전하게 변환 (float)
 */
_s32 StringUtil_TryToNumber_Float(_f32* _pVal, const _s8* _pStr, _s32 _ignoreLeadingZero = 1);

/*
 * 문자열을 숫자로 안전하게 변환 (double)
 */
_s32 StringUtil_TryToNumber_Double(_f64* _pVal, const _s8* _pStr, _s32 _ignoreLeadingZero = 1);

/*
 * 문자열 길이 반환 (null 제외)
 * @param _pStr : 문자열 포인터 (null 전달 시 -1 반환)
 * @return 문자열 길이
 */
_s32 StringUtil_Length(const _s8* _pStr);

/*
 * 문자열 길이 반환 (null 포함)
 * @return Length + 1
 */
_s32 StringUtil_LengthWithNull(const _s8* _pStr);

/*
 * 문자열이 null이거나 빈 문자열인지 확인
 * @return 1 if null or empty, 0 otherwise
 */
_s32 StringUtil_IsNullOrEmpty(const _s8* _pStr);

/*
 * 버퍼에 문자열 복사 (용량 초과 방지)
 * @param _pBuffer    : 대상 버퍼
 * @param _bufferSize : 버퍼 크기
 * @param _pCopy      : 복사할 문자열
 * @return 복사된 문자 개수, 실패 시 -1
 */
_s32 StringUtil_Copy(_s8* _pBuffer, _s32 _bufferSize, const _s8* _pCopy);

/*
 * 버퍼에 문자열 복사 (용량 검사 없음)
 * @return 복사된 문자 개수, 실패 시 -1
 */
_s32 StringUtil_CopyUnsafe(_s8* _pBuffer, const _s8* _pCopy);

/*
 * 두 문자열 동등 비교
 * @param _pSrc         : 비교 문자열 1
 * @param _pDst         : 비교 문자열 2
 * @param _bCompareCase : 1 = 대소문자 구분, 0 = 대소문자 무시
 * @return 1 if equal, 0 otherwise
 */
_s32 StringUtil_IsEqual(const _s8* _pSrc, const _s8* _pDst, _s32 _bCompareCase = 1);

/*
 * 두 문자열 동등 비교 (길이 명시)
 */
_s32 StringUtil_IsEqualLen(const _s8* _pSrc, _s32 _srcLen, const _s8* _pDst, _s32 _dstLen, _s32 _bCompareCase = 1);

/*
 * 문자열에서 부분 문자열 검색 (범위 지정)
 * @param _pSource      : 검색 대상 문자열
 * @param _sourceLen    : 대상 문자열 길이
 * @param _startIdx     : 검색 시작 인덱스
 * @param _endIdx       : 검색 종료 인덱스 (포함)
 * @param _pStr         : 검색할 문자열
 * @param _caseSensitive: 1 = 대소문자 구분, 0 = 무시
 * @return 발견된 인덱스, 없으면 -1
 */
_s32 StringUtil_Find(const _s8* _pSource, _s32 _sourceLen, _s32 _startIdx, _s32 _endIdx, const _s8* _pStr, _s32 _caseSensitive = 1);

/*
 * 문자열에서 부분 문자열 검색 (범위 + 검색 문자열 길이 명시)
 */
_s32 StringUtil_FindLen(const _s8* _pSource, _s32 _sourceLen, _s32 _startIdx, _s32 _endIdx, const _s8* _pStr, _s32 _strLen, _s32 _caseSensitive = 1);

/*
 * 문자열에서 부분 문자열 검색 (시작 인덱스만 지정, 끝까지 검색)
 */
_s32 StringUtil_FindFrom(const _s8* _pSource, _s32 _sourceLen, _s32 _startIdx, const _s8* _pStr, _s32 _caseSensitive = 1);

/*
 * 문자열에서 부분 문자열의 모든 위치 탐색
 * @param _pPositionArray : 결과 인덱스를 저장할 배열 (충분한 크기 필요)
 * @return 발견된 개수
 */
_s32 StringUtil_FindAll(_s32* _pPositionArray, const _s8* _pSource, const _s8* _pStr, _s32 _caseSensitive = 1);

/*
 * 문자열에서 부분 문자열의 모든 위치 탐색 (소스 길이 명시)
 */
_s32 StringUtil_FindAllLen(_s32* _pPositionArray, const _s8* _pSource, _s32 _sourceLen, const _s8* _pStr, _s32 _caseSensitive = 1);

/*
 * 문자열에서 부분 문자열의 모든 위치 탐색 (범위 지정)
 */
_s32 StringUtil_FindAllRange(_s32* _pPositionArray, const _s8* _pSource, _s32 _sourceLen, _s32 _startIdx, _s32 _endIdx, const _s8* _pStr, _s32 _caseSensitive = 1);

/*
 * 문자열에서 특정 문자 검색 (앞에서부터)
 * @return 발견된 인덱스, 없으면 -1
 */
_s32 StringUtil_FindChar(const _s8* _pSource, _s8 _ch);

/*
 * 문자열에서 특정 문자 검색 (뒤에서부터)
 * @return 발견된 인덱스, 없으면 -1
 */
_s32 StringUtil_FindCharReverse(const _s8* _pSource, _s8 _ch);

/*
 * 문자열에서 특정 문자 검색 (뒤에서부터, 길이 명시)
 */
_s32 StringUtil_FindCharReverseLen(const _s8* _pSource, _s32 _len, _s8 _ch);

/*
 * 문자열에서 특정 문자가 아닌 첫 번째 위치 반환
 * @return 발견된 인덱스
 */
_s32 StringUtil_FindCharUncontained(const _s8* _pSource, _s8 _ch);

#ifdef __cplusplus
}
#endif
