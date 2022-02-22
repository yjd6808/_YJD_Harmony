/*
	�ۼ��� : ������
	�޸� ������ �����ִ� Ŭ�����Դϴ�.
*/

#include <JCore/Core.h>
#include <JCore/Memory.h>

namespace JCore {

// memcpy_s�� ����� �����մϴ�.
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

// memcpy�� ����� �����մϴ�.
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

	pDst += srcCopyByte - 1;	// ������ ���Ұ� pDst[srcCopyByte - 1] �̹Ƿ�
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

// memset�� ����� �����մϴ�.
void Memory::Set(void* src, const int srcCapacity, const Byte value) {
	Byte* pSrc = (Byte*)src;

	for (int i = 0; i < srcCapacity; i++) {
		*pSrc = value;
		pSrc++;
	}
}



} // namespace JCore