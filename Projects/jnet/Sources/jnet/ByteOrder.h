/*
 * 작성자 : 윤정도
 */


#pragma once

#include <jc/TypeTraits.h>
#include <jc/Memory.h>

NS_JNET_BEGIN

enum class Endianness
{
	Little,
	Big
};

struct ByteOrder final
{
	// 엔디안 체크 방법
	// @참고 : https://sites.google.com/site/insideoscore/endianness
	constexpr static Endianness HostEndianness()
	{
		constexpr int ENDIANNESS = 0x00000001;
		return (*(char*)&ENDIANNESS == 0x01) ? Endianness::Little : Endianness::Big;
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
	constexpr static T NetworkToHost(const T _val)
	{
		static_assert(jc::IsFundamentalType_v<T>, "... T muse be primitive type");

		// 호스트가 빅 엔디언이면 그냥 반환
		if (HostEndianness() == Endianness::Big)
		{
			return _val;
		}

		T result = 0;
		const int size = sizeof(T);

		_u8* pDst = (_u8*)&result;
		const _u8* pSrc = (_u8*)&_val;

		for (int index = size - 1, offset = 0; index >= 0; --index, offset++)
		{
			pDst[offset] = pSrc[index];
		}

		return result;
	}

	template <typename T>
	constexpr static T HostToNetwork(const T _val)
	{
		static_assert(jc::IsFundamentalType_v<T>, "... T muse be primitive type");

		// 호스트가 빅 엔디언이면 그냥 반환
		if (HostEndianness() == Endianness::Big)
		{
			return _val;
		}

		T result = 0;
		const int size = sizeof(T);

		_u8* pDst = (_u8*)&result;
		const _u8* pSrc = (_u8*)&_val;

		for (int index = size - 1, offset = 0; index >= 0; --index, offset++)
		{
			pDst[offset] = pSrc[index];
		}

		return result;
	}
};

NS_END
