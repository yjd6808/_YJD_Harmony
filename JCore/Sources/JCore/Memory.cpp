/*
	�ۼ��� : ������
	�޸� ������ �����ִ� Ŭ�����Դϴ�.
*/

#include <JCore/Core.h>
#include <JCore/Memory.h>

namespace JCore {

// memcpy_s�� ����� �����մϴ�.
void Memory::Copy(void* src, const int srcCapacity, const void* dst, const int dstCopySize) {
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

// memcpy�� ����� �����մϴ�.
void Memory::CopyUnsafe(void* src, const void* dst, const int dstCopySize) {
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

// memset�� ����� �����մϴ�.
void Memory::Set(void* src, const int srcCapacity, const Byte value) {
	Byte* pSrc = (Byte*)src;

	for (int i = 0; i < srcCapacity; i++) {
		*pSrc = value;
		pSrc++;
	}
}



} // namespace JCore