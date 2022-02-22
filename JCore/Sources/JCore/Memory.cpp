/*
	작성자 : 윤정도
	메모리 조작을 도와주는 클래스입니다.
*/

#include <JCore/Core.h>
#include <JCore/Memory.h>

namespace JCore {

// memcpy_s와 기능이 동일합니다.
void Memory::Copy(void* dst, const int dstCapacityByte, const void* src, const int srcCopyByte) {
	int iCopiedBytes = 0;

	Byte* pDst = (Byte*)dst;
	Byte* pSrc = (Byte*)src;
	

	while (iCopiedBytes < dstCapacityByte && iCopiedBytes < srcCopyByte) {
		*pDst = *pSrc;
		pSrc++;
		pDst++;
		iCopiedBytes++;
	}
}

// memcpy와 기능이 동일합니다.
void Memory::CopyUnsafe(void* dst, const void* src, const int srcCopyByte) {
	int iCopiedBytes = 0;

	Byte* pDst = (Byte*)dst;
	Byte* pSrc = (Byte*)src;
	

	while (iCopiedBytes < srcCopyByte) {
		*pDst = *pSrc;
		pSrc++;
		pDst++;
		iCopiedBytes++;
	}
}

void Memory::CopyReverse(void* dst, const int dstCapacityByte, const void* src, const int srcCopyByte) {
	int iCopiedBytes = 0;

	Byte* pDst = (Byte*)dst;
	Byte* pSrc = (Byte*)src;

	pDst += srcCopyByte - 1;	// 마지막 원소가 pDst[srcCopyByte - 1] 이므로
	pSrc += srcCopyByte - 1;

	while (iCopiedBytes < dstCapacityByte && iCopiedBytes < srcCopyByte) {
		*pDst = *pSrc;
		pSrc--;
		pDst--;
		iCopiedBytes++;
	}
}

void Memory::CopyUnsafeReverse(void* dst, const void* src, const int srcCopyByte) {
	int iCopiedBytes = 0;

	Byte* pDst = (Byte*)dst;
	Byte* pSrc = (Byte*)src;

	pDst += srcCopyByte - 1;
	pSrc += srcCopyByte - 1;

	while (iCopiedBytes < srcCopyByte) {
		*pDst = *pSrc;
		pSrc--;
		pDst--;
		iCopiedBytes++;
	}
}

// memset과 기능이 동일합니다.
void Memory::Set(void* src, const int srcCapacity, const Byte value) {
	Byte* pSrc = (Byte*)src;

	for (int i = 0; i < srcCapacity; i++) {
		*pSrc = value;
		pSrc++;
	}
}



} // namespace JCore