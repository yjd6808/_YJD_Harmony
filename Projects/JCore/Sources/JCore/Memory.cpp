/*
 *	작성자 : 윤정도
 *	메모리 조작을 도와주는 클래스입니다.
 */

#include <JCore/Core.h>
#include <JCore/Memory.h>

namespace JCore {

// memcpy_s와 기능이 동일합니다.
void Memory::Copy(void* dst, const int dstCapacityByte, const void* src, const int srcCopyByte) {
	DebugAssertMessage(dst && src && dstCapacityByte > 0, "인자를 똑띠 전달해주세요");
	int iCopiedBytes = 0;

	Byte* pDst = static_cast<Byte*>(dst);
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
	DebugAssertMessage(dst && src, "인자를 똑띠 전달해주세요");
	int iCopiedBytes = 0;

	Byte* pDst = static_cast<Byte*>(dst);
	Byte* pSrc = (Byte*)src;
	

	while (iCopiedBytes < srcCopyByte) {
		*pDst = *pSrc;
		pSrc++;
		pDst++;
		iCopiedBytes++;
	}
}

void Memory::CopyReverse(void* dst, const int dstCapacityByte, const void* src, const int srcCopyByte) {
	DebugAssertMessage(dst && src && dstCapacityByte > 0, "인자를 똑띠 전달해주세요");
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
	DebugAssertMessage(dst && src, "인자를 똑띠 전달해주세요");
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
#ifdef _DEBUG
	if (src == nullptr || srcCapacity <= 0) {
		DebugAssertMessage(false, "인자를 똑띠 전달해주세요");
	}
#endif

	Byte* pSrc = (Byte*)src;

	for (int i = 0; i < srcCapacity; i++) {
		*pSrc = value;
		pSrc++;
	}
}



} // namespace JCore