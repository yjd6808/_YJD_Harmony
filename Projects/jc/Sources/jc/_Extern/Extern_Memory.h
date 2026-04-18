/*
 * 작성자 : 윤정도
 * 목적 : MASM 어셈블리 코드에서 Memory API를 호출하기 위한 extern "C" 래퍼
 * created by AI.
 */

#pragma once

#ifdef __cplusplus
#include "jc/Type.h"
extern "C" {
#endif

/*
 * 메모리 할당
 * @param _size : 할당할 바이트 크기
 * @return 할당된 메모리 포인터 (실패 시 NULL)
 */
void* Memory_Allocate(_u32 _size);

/*
 * 메모리 해제
 * @param _ptr : 해제할 메모리 포인터
 */
void Memory_Deallocate(void* _ptr);

/*
 * 메모리 복사 (시작 바이트부터 복사, 범위 검사 있음)
 * @param _dst             : 대상 메모리 포인터
 * @param _dstCapacityByte : 대상 버퍼의 바이트 용량
 * @param _src             : 원본 메모리 포인터
 * @param _srcCopyByte     : 복사할 바이트 수
 */
void Memory_Copy(void* _dst, _s32 _dstCapacityByte, const void* _src, _s32 _srcCopyByte);

/*
 * 메모리 복사 (시작 바이트부터 복사, 범위 검사 없음)
 * @param _dst         : 대상 메모리 포인터
 * @param _src         : 원본 메모리 포인터
 * @param _srcCopyByte : 복사할 바이트 수
 */
void Memory_CopyUnsafe(void* _dst, const void* _src, _s32 _srcCopyByte);

/*
 * 메모리 역방향 복사 (마지막 바이트부터 복사, 범위 검사 있음)
 * @param _dst             : 대상 메모리 포인터
 * @param _dstCapacityByte : 대상 버퍼의 바이트 용량
 * @param _src             : 원본 메모리 포인터
 * @param _srcCopyByte     : 복사할 바이트 수
 */
void Memory_CopyReverse(void* _dst, _s32 _dstCapacityByte, const void* _src, _s32 _srcCopyByte);

/*
 * 메모리 역방향 복사 (마지막 바이트부터 복사, 범위 검사 없음)
 * @param _dst         : 대상 메모리 포인터
 * @param _src         : 원본 메모리 포인터
 * @param _srcCopyByte : 복사할 바이트 수
 */
void Memory_CopyUnsafeReverse(void* _dst, const void* _src, _s32 _srcCopyByte);

/*
 * 1바이트(U8) 단위로 메모리 설정
 * @param _src         : 대상 메모리 포인터
 * @param _srcCapacity : 설정할 바이트 수
 * @param _value       : 설정할 값
 */
void Memory_Set(void* _src, _s32 _srcCapacity, _u8 _value);

/*
 * 2바이트(U16) 단위로 메모리 설정
 * @param _src         : 대상 메모리 포인터
 * @param _srcCapacity : 설정할 요소 개수 (바이트 수 아님)
 * @param _value       : 설정할 값
 */
void Memory_SetU16(_u16* _src, _s32 _srcCapacity, _u16 _value);

/*
 * 4바이트(U32) 단위로 메모리 설정
 * @param _src         : 대상 메모리 포인터
 * @param _srcCapacity : 설정할 요소 개수 (바이트 수 아님)
 * @param _value       : 설정할 값
 */
void Memory_SetU32(_u32* _src, _s32 _srcCapacity, _u32 _value);

/*
 * 8바이트(U64) 단위로 메모리 설정
 * @param _src         : 대상 메모리 포인터
 * @param _srcCapacity : 설정할 요소 개수 (바이트 수 아님)
 * @param _value       : 설정할 값
 */
void Memory_SetU64(_u64* _src, _s32 _srcCapacity, _u64 _value);

/*
 * float(4바이트) 단위로 메모리 설정
 * @param _src         : 대상 메모리 포인터
 * @param _srcCapacity : 설정할 요소 개수 (바이트 수 아님)
 * @param _value       : 설정할 값
 */
void Memory_SetFloat(_f32* _src, _s32 _srcCapacity, _f32 _value);

/*
 * double(8바이트) 단위로 메모리 설정
 * @param _src         : 대상 메모리 포인터
 * @param _srcCapacity : 설정할 요소 개수 (바이트 수 아님)
 * @param _value       : 설정할 값
 */
void Memory_SetDouble(_f64* _src, _s32 _srcCapacity, _f64 _value);

#ifdef __cplusplus
}
#endif
