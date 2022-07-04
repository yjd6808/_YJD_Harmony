/*
	�ۼ��� : ������
	�ִ� �ּ� ��ġ�� ǥ��
*/

#pragma once

#include <JCore/Type.h>

#pragma warning (push)
#pragma warning (disable : 4307)	// '+': signed integral constant overflow
#pragma warning (disable : 4309)	// 'initializing': truncation of constant value
#pragma warning (disable : 26450)	// Arithmetic overflow : '+' operation causes overflow at compile time.Use a wider type to store the operands -> MinInt32_v �̷���ó�� ���� ���Ƿ� �����÷� ����Ų �� ����


namespace JCore {

constexpr char		MaxChar_v = (1 << 7) - 1;
constexpr char		MinChar_v = (1 << 7);
constexpr char		MaxInt8_v = (1 << 7) - 1;
constexpr char		MinInt8_v = (1 << 7);
constexpr Byte		MaxByte_v = (1 << 8) - 1;
constexpr Byte		MaxInt8U_v = (1 << 8) - 1;
constexpr Int16		MaxInt16_v = (1 << 15) - 1;
constexpr Int16		MinInt16_v = (1 << 15);
constexpr Int16		MaxShort_v = (1 << 15) - 1;
constexpr Int16		MinShort_v = (1 << 15);
constexpr Int16U	MaxInt16U_v = (1 << 16) - 1;
constexpr int		MaxInt32_v = 0x7fffffff;
constexpr int		MinInt32_v = MaxInt32_v + 1;
constexpr int		MaxInt_v = 0x7fffffff;
constexpr int		MinInt_v = MaxInt32_v + 1;
constexpr Int32U	MaxInt32U_v = 0xffffffffU;
constexpr Int64		MaxInt64_v = 0x7fffffffffffffffLL;
constexpr Int64		MinInt64_v = MaxInt64_v + 1LL;
constexpr Int64U	MaxInt64U_v = 0xffffffffffffffffLLU;
constexpr int		BytePerBit_v = 8;

template <typename T>
constexpr int BitCount() {
	return sizeof(T) * BytePerBit_v;
}

template <typename T>
constexpr int BitCount(T&& val) {
	return sizeof(val) * BytePerBit_v;
}

} // namespace JCore

#pragma warning (pop)

