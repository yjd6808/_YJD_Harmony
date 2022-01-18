/*
	�ۼ��� : ������
	�޸� ������ �����ִ� Ŭ�����Դϴ�.
*/

#pragma once

#include <Core/Type.h>

namespace JCore {

class MemoryUtil final
{
public:
	static void Copy(void* src, const int srcCapacity, const void* dst, const int dstCopySize);
	static void CopyUnsafe(void* src, const void* dst, const int dstCopySize);

	static void Set(void* src, const int srcCapacity, const Byte value);

	// �ڷ��� T ũ�� ������ ���縦 �����մϴ�.
	template <typename T>
	static void Set(T* src, const int srcCapacity, const T value) {
		const int kiDataTypeSize = sizeof(T);

		Byte* pSrc = (Byte*)src;
		Byte* pValue = (Byte*)&value;

		for (int i = 0; i < srcCapacity; i++) {
			Copy(pSrc, kiDataTypeSize, pValue, kiDataTypeSize);
			pSrc += kiDataTypeSize;
		}
	}
};

} // namespace JCore

