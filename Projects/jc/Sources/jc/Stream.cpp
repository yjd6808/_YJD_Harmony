/*
 * 작성자: 윤정도
 * 생성일: 1/7/2023 4:04:50 AM
 * =====================
 *
 */

#include "jc/Exception.h"
#include "jc/Stream.h"

#include "jc/Primitives/String.h"

#define READ_BUFFER_SIZE		128

NS_JC_BEGIN

String Stream::ReadString()
{
	String szRet(READ_BUFFER_SIZE);
	char* szRetBuffer = szRet.Source();
	int iReadLength = 0;
	for (int i = 0; !IsEnd(); i++)
	{
		szRetBuffer[i] = ReadInt8();
		if (szRetBuffer[i] == NULL) break;

		iReadLength += 1;
		szRet.SetLength(iReadLength);
		szRet.ResizeIfNeeded(iReadLength + 64); // 좀 여유를 두고 확장
	}
	szRetBuffer[iReadLength] = NULL;

	return szRet;
}

_s8 Stream::ReadInt8()
{
	_s8 iRet;
	if (Read((_u8*)&iRet, 0, sizeof(_s8)) != sizeof(_s8))
	{
		throw RuntimeException("스트림에서 _s8을 읽는데 실패했습니다.");
	}
	return iRet;
}

_u8 Stream::ReadByte()
{
	_u8 iRet;
	if (Read(&iRet, 0, sizeof(_u8)) != sizeof(_u8))
	{
		throw RuntimeException("스트림에서 _u8을 읽는데 실패했습니다.");
	}
	return iRet;
}

_s16 Stream::ReadInt16()
{
	_s16 iRet;
	if (Read((_u8*)&iRet, 0, sizeof(_s16)) != sizeof(_s16))
	{
		throw RuntimeException("스트림에서 _s16을 읽는데 실패했습니다.");
	}
	return iRet;
}

_u16 Stream::ReadInt16U()
{
	_s16 iRet;
	if (Read((_u8*)&iRet, 0, sizeof(_u16)) != sizeof(_u16))
	{
		throw RuntimeException("스트림에서 _u16을 읽는데 실패했습니다.");
	}
	return iRet;
}

_s32 Stream::ReadInt32()
{
	_s32 iRet;
	if (Read((_u8*)&iRet, 0, sizeof(_s32)) != sizeof(_s32))
	{
		throw RuntimeException("스트림에서 _s32을 읽는데 실패했습니다.");
	}
	return iRet;
}

_u32 Stream::ReadInt32U()
{
	_u32 iRet;
	if (Read((_u8*)&iRet, 0, sizeof(_u32)) != sizeof(_u32))
	{
		throw RuntimeException("스트림에서 _u32을 읽는데 실패했습니다.");
	}
	return iRet;
}

_s64 Stream::ReadInt64()
{
	_s64 iRet;
	if (Read((_u8*)&iRet, 0, sizeof(_s64)) != sizeof(_s64))
	{
		throw RuntimeException("스트림에서 _s64을 읽는데 실패했습니다.");
	}
	return iRet;
}

_u64 Stream::ReadInt64U()
{
	_u64 iRet;
	if (Read((_u8*)&iRet, 0, sizeof(_u64)) != sizeof(_u64))
	{
		throw RuntimeException("스트림에서 _u64을 읽는데 실패했습니다.");
	}
	return iRet;
}


void Stream::WriteString(const String& _str, bool _withNull)
{
	Write((_u8*)_str.Source(), _withNull ? _str.Length() + 1 : _str.Length());
}


NS_END
