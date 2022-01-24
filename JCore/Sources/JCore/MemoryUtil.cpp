/*
	작성자 : 윤정도
	메모리 조작을 도와주는 클래스입니다.
*/

#include <JCore/Core.h>
#include <JCore/MemoryUtil.h>

namespace JCore {

// memcpy_s와 기능이 동일합니다.
void MemoryUtil::Copy(void* src, const int srcCapacity, const void* dst, const int dstCopySize) {
	int iCopiedBytes = 0;

	Byte* pSrc = (Byte*)src;
	Byte* pDst = (Byte*)dst;

	while (iCopiedBytes <= srcCapacity && iCopiedBytes <= dstCopySize) {
		*pSrc = *pDst;
		pSrc++;
		pDst++;
		iCopiedBytes++;
	}
}

// memcpy와 기능이 동일합니다.
void MemoryUtil::CopyUnsafe(void* src, const void* dst, const int dstCopySize) {
	int iCopiedBytes = 0;

	Byte* pSrc = (Byte*)src;
	Byte* pDst = (Byte*)dst;

	while (iCopiedBytes <= dstCopySize) {
		*pSrc = *pDst;
		pSrc++;
		pDst++;
		iCopiedBytes++;
	}
}

// memset과 기능이 동일합니다.
void MemoryUtil::Set(void* src, const int srcCapacity, const Byte value) {
	Byte* pSrc = (Byte*)src;

	for (int i = 0; i < srcCapacity; i++) {
		*pSrc = value;
		pSrc++;
	}
}



} // namespace JCore