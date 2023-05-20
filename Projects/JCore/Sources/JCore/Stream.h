/*
 * 작성자: 윤정도
 * 생성일: 1/7/2023 3:46:29 AM
 * =====================
 *
 *   ConsoleStream,
 *   FileStream,
 *	 NetworkStream
 *	 MemoryStream
 *
 *	C#에서 자주 쓰던 기능을 모방한다.
 *  그라고 원하는 기능좀 추가함
 */ 

#pragma once

#include <JCore/Type.h>
#include <JCore/Assert.h>
#include <JCore/Define.h>

NS_JC_BEGIN

template <typename T>
class SharedPtr;
class String;
class Stream
{
public:
	enum Origin
	{
		eBegin,
		eCurrent,
		eEnd
	};

	Stream()
		: m_bCanWrite(false)
		, m_bCanRead(false)
		, m_bCanSeek(false) {}
	Stream(bool camWrite, bool canRead, bool canSeek)
		: m_bCanWrite(camWrite)
		, m_bCanRead(canRead)
		, m_bCanSeek(canSeek) {}

	virtual ~Stream() = default;

	int GetOffset() { return m_iOffset; }
	int GetLength() { return m_iLength; }

	void SetOffset(int offset) {
		m_iOffset = offset;

		if (offset >= m_iLength) {
			m_iLength = offset;
		}
	}

	// bytes의 offset 위치부터 len 만큼 스트림으로부터 읽어서 저장한다.
	virtual int Read(JCORE_OUT Byte* bytes, int offset, int len) = 0;

	String ReadString();
	Int8 ReadInt8();
	Byte ReadByte();
	Int16 ReadInt16();
	Int16U ReadInt16U();
	Int32 ReadInt32();
	Int32U ReadInt32U();
	Int64 ReadInt64();
	Int64U ReadInt64U();

	// bytes의 offset 위치부터 len만큼 스트림에 작성한다.
	virtual void Write(const Byte* bytes, int offset, int len) = 0;

	// bytes의 0위치부터 len만큼 스트림에 작성한다.
	virtual void Write(const Byte* bytes, int len) {
		DebugAssertMsg(CanWrite(), "해당 스트림에 Write 할 수 없습니다.");
		Write(bytes, 0, len);
	}
	void WriteString(const String& str, bool withNull = true);
	void WriteInt8(Int8 data) { Write((const Byte*)&data, 0, sizeof(Int8)); }
	void WriteInt8U(Int8U data) { Write((const Byte*)&data, 0, sizeof(Int8U)); }
	void WriteInt16(Int16 data) { Write((const Byte*)&data, 0, sizeof(Int16)); }
	void WriteInt16U(Int16U data) { Write((const Byte*)&data, 0, sizeof(Int16U)); }
	void WriteInt32(Int32 data) { Write((const Byte*)&data, 0, sizeof(Int32)); }
	void WriteInt32U(Int32U data) { Write((const Byte*)&data, 0, sizeof(Int32U)); }
	void WriteInt64(Int64 data) { Write((const Byte*)&data, 0, sizeof(Int64)); }
	void WriteInt64U(Int64U data) { Write((const Byte*)&data, 0, sizeof(Int64U)); }


	// 커서의 위치를 옮긴다.
	virtual void Seek(int offset, Origin origin = Origin::eBegin) = 0;

	// 스트림의 내용을 반영한다.
	virtual bool Flush() = 0;

	// 스트림을 닫는다.
	virtual void Close() = 0;
	virtual bool IsClosed() = 0;

	// 현재 스트림에 Write 기능을 사용 할 수 있는지
	bool CanWrite() { return m_bCanWrite;  }

	// 현재 스트림이 Read 기능을 사용할 수 있는지
	bool CanRead() { return m_bCanRead; }

	// 현재 스트림이 Seek 기능을 사용할 수 있는지
	bool CanSeek() { return m_bCanSeek; }

	bool IsEnd() { return m_iOffset == m_iLength; }

	
protected:
	int m_iOffset{};	// 현재 쓰고/읽을 커서의 위치
	int m_iLength{};	// 작성된 바이트 크기 (커서는 0 ~ Length 사이에서만 움직일 수 있다.)

	bool m_bCanWrite;
	bool m_bCanRead;
	bool m_bCanSeek;
};

using StreamPtr = SharedPtr<Stream>;

NS_JC_END