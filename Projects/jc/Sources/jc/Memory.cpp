/*
 *	작성자 : 윤정도
 *	메모리 조작을 도와주는 클래스입니다.
 */

#include "jc/Memory.h"

NS_JC_BEGIN
// memcpy_s와 기능이 동일합니다.
void Memory::Copy(void* _dst, const int _dstCapacityByte, const void* _src, const int _srcCopyByte)
{
	jc_assert_msg(_dst && _src && _dstCapacityByte > 0, "인자를 똑띠 전달해주세요");
	int iCopiedBytes = 0;

	_u8* pDst = static_cast<_u8*>(_dst);
	_u8* pSrc = (_u8*)_src;


	while (iCopiedBytes < _dstCapacityByte && iCopiedBytes < _srcCopyByte)
	{
		*pDst = *pSrc;
		pSrc++;
		pDst++;
		iCopiedBytes++;
	}
}

// memcpy와 기능이 동일합니다.
void Memory::CopyUnsafe(void* _dst, const void* _src, const int _srcCopyByte)
{
	jc_assert_msg(_dst && _src, "인자를 똑띠 전달해주세요");
	int iCopiedBytes = 0;

	_u8* pDst = static_cast<_u8*>(_dst);
	_u8* pSrc = (_u8*)_src;


	while (iCopiedBytes < _srcCopyByte)
	{
		*pDst = *pSrc;
		pSrc++;
		pDst++;
		iCopiedBytes++;
	}
}

void Memory::CopyReverse(void* _dst, const int _dstCapacityByte, const void* _src, const int _srcCopyByte)
{
	jc_assert_msg(_dst && _src && _dstCapacityByte > 0, "인자를 똑띠 전달해주세요");
	int iCopiedBytes = 0;

	_u8* pDst = (_u8*)_dst;
	_u8* pSrc = (_u8*)_src;

	pDst += _srcCopyByte - 1; // 마지막 원소가 pDst[srcCopyByte - 1] 이므로
	pSrc += _srcCopyByte - 1;

	while (iCopiedBytes < _dstCapacityByte && iCopiedBytes < _srcCopyByte)
	{
		*pDst = *pSrc;
		pSrc--;
		pDst--;
		iCopiedBytes++;
	}
}

void Memory::CopyUnsafeReverse(void* _dst, const void* _src, const int _srcCopyByte)
{
	jc_assert_msg(_dst && _src, "인자를 똑띠 전달해주세요");
	int iCopiedBytes = 0;

	_u8* pDst = (_u8*)_dst;
	_u8* pSrc = (_u8*)_src;

	pDst += _srcCopyByte - 1;
	pSrc += _srcCopyByte - 1;

	while (iCopiedBytes < _srcCopyByte)
	{
		*pDst = *pSrc;
		pSrc--;
		pDst--;
		iCopiedBytes++;
	}
}

// memset과 기능이 동일합니다.
void Memory::Set(void* _src, const int _srcCapacity, const _u8 _value)
{
#ifdef _DEBUG
	if (_src == nullptr || _srcCapacity <= 0)
	{
		jc_assert_msg(false, "인자를 똑띠 전달해주세요");
	}
#endif

	_u8* pSrc = (_u8*)_src;

	for (int i = 0; i < _srcCapacity; i++)
	{
		*pSrc = _value;
		pSrc++;
	}
}


NS_END
