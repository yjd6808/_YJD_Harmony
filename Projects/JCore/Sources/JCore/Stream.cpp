/*
 * 작성자: 윤정도
 * 생성일: 1/7/2023 4:04:50 AM
 * =====================
 *
 */

#include <JCore/Core.h>
#include <JCore/Exception.h>
#include <JCore/Stream.h>

#include <JCore/Primitives/String.h>

#define ReadBufferSize		128

NS_JC_BEGIN

String Stream::ReadString() {
	String szRet(ReadBufferSize);
	char* szRetBuffer = szRet.Source();
	int iReadLength = 0;
	for (int i = 0; !IsEnd(); i++) {
		szRetBuffer[i] = ReadInt8();
		if (szRetBuffer[i] == NULL) break;

		iReadLength += 1;
		szRet.SetLength(iReadLength);
		szRet.ResizeIfNeeded(iReadLength + 64);	// 좀 여유를 두고 확장
	}

	return szRet;
}

Int8 Stream::ReadInt8() {
	Int8 iRet;
	if (Read((Byte*)&iRet, 0, sizeof(Int8)) != sizeof(Int8)) {
		throw RuntimeException("스트림에서 Int8을 읽는데 실패했습니다.");
	}
	return iRet;
}

Byte Stream::ReadByte() {
	Byte iRet;
	if (Read((Byte*)&iRet, 0, sizeof(Byte)) != sizeof(Byte)) {
		throw RuntimeException("스트림에서 Byte을 읽는데 실패했습니다.");
	}
	return iRet;
}

Int16 Stream::ReadInt16() {
	Int16 iRet;
	if (Read((Byte*)&iRet, 0, sizeof(Int16)) != sizeof(Int16)) {
		throw RuntimeException("스트림에서 Int16을 읽는데 실패했습니다.");
	}
	return iRet;
}

Int16U Stream::ReadInt16U() {
	Int16 iRet;
	if (Read((Byte*)&iRet, 0, sizeof(Int16U)) != sizeof(Int16U)) {
		throw RuntimeException("스트림에서 Int16U을 읽는데 실패했습니다.");
	}
	return iRet;
}

Int32 Stream::ReadInt32() {
	Int32 iRet;
	if (Read((Byte*)&iRet, 0, sizeof(Int32)) != sizeof(Int32)) {
		throw RuntimeException("스트림에서 Int32을 읽는데 실패했습니다.");
	}
	return iRet;
}

Int32U Stream::ReadInt32U() {
	Int32U iRet;
	if (Read((Byte*)&iRet, 0, sizeof(Int32U)) != sizeof(Int32U)) {
		throw RuntimeException("스트림에서 Int32U을 읽는데 실패했습니다.");
	}
	return iRet;
}

Int64 Stream::ReadInt64() {
	Int64 iRet;
	if (Read((Byte*)&iRet, 0, sizeof(Int64)) != sizeof(Int64)) {
		throw RuntimeException("스트림에서 Int64을 읽는데 실패했습니다.");
	}
	return iRet;
}

Int64U Stream::ReadInt64U() {
	Int64U iRet;
	if (Read((Byte*)&iRet, 0, sizeof(Int64U)) != sizeof(Int64U)) {
		throw RuntimeException("스트림에서 Int64U을 읽는데 실패했습니다.");
	}
	return iRet;
}


void Stream::WriteString(const String& str, bool withNull) {
	Write((Byte*)str.Source(), withNull ? str.Length() : str.Length() + 1);
}



NS_JC_END