/*
 * 작성자 : 윤정도
 */


#pragma once

#include <JCore/TypeTraits.h>
#include <JCore/Memory.h>

enum class Endianness
{
	Little,
	Big
};

struct ByteOrder final
{
	// 엔디안 체크 방법
	// @참고 : https://sites.google.com/site/insideoscore/endianness
	constexpr static Endianness HostEndianness() {
		constexpr int iEndianness = 0x00000001;
		return (*(char*)&iEndianness == 0x01) ? Endianness::Little : Endianness::Big;
	}

	// 리틀엔디안 : 4바이트 정수 0xaabbccdd가 메모리에 아래와 같이 저장됨
	// 0x100 : dd
	// 0x101 : cc
	// 0x102 : bb
	// 0x103 : aa
	// 
	// 빅엔디안 : 4바이트 정수 0xaabbccdd가 메모리에 아래와 같이 저장됨
	// 0x100 : aa
	// 0x101 : bb
	// 0x102 : cc
	// 0x103 : dd

	template <typename T>
	constexpr static T NetworkToHost(const T val) {
		static_assert(JCore::IsPrimitiveType_v<T>, "... T muse be primitive type");

		// 호스트가 빅 엔디언이면 그냥 반환
		if (HostEndianness() == Endianness::Big) {
			return val;
		}

		T ret = 0;
		const int iSize = sizeof(T);

		Byte* pDst = (Byte*)&ret;
		const Byte* pSrc = (Byte*)&val;

		for (int i = iSize - 1, j = 0; i >= 0; --i, j++) {
			pDst[j] = pSrc[i];
		}

		return ret;
	}

	template <typename T>
	constexpr static T HostToNetwork(const T val) {
		static_assert(JCore::IsPrimitiveType_v<T>, "... T muse be primitive type");

		// 호스트가 빅 엔디언이면 그냥 반환
		if (HostEndianness() == Endianness::Big) {
			return val;
		}

		T ret = 0;
		const int iSize = sizeof(T);

		Byte* pDst = (Byte*)&ret;
		const Byte* pSrc = (Byte*)&val;

		for (int i = iSize - 1, j = 0; i >= 0; --i, j++) {
			pDst[j] = pSrc[i];
		}

		return ret;
	}
};