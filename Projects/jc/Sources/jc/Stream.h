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

#include <jc/Type.h>
#include <jc/Assert.h>
#include <jc/Define.h>

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
	, m_bCanSeek(false)
	{
	}

	Stream(bool _camWrite, bool canRead, bool canSeek)
	: m_bCanWrite(_camWrite)
	, m_bCanRead(canRead)
	, m_bCanSeek(canSeek)
	{
	}

	virtual ~Stream() = default;

	int GetOffset() { return m_iOffset; }
	int GetLength() { return m_iLength; }

	void SetOffset(int _offset)
	{
		m_iOffset = _offset;

		if (_offset >= m_iLength)
		{
			m_iLength = _offset;
		}
	}

	// bytes의 offset 위치부터 len 만큼 스트림으로부터 읽어서 저장한다.
	virtual int Read(OUT _u8* _pBytes, int _offset, int _length) = 0;

	String ReadString();
	_s8 ReadInt8();
	_u8 ReadByte();
	_s16 ReadInt16();
	_u16 ReadInt16U();
	_s32 ReadInt32();
	_u32 ReadInt32U();
	_s64 ReadInt64();
	_u64 ReadInt64U();

	// bytes의 offset 위치부터 len만큼 스트림에 작성한다.
	virtual void Write(const _u8* _pBytes, int _offset, int _length) = 0;

	// bytes의 0위치부터 len만큼 스트림에 작성한다.
	virtual void Write(const _u8* _pBytes, int _length)
	{
		jc_assert_msg(CanWrite(), "해당 스트림에 Write 할 수 없습니다.");
		Write(_pBytes, 0, _length);
	}

	void WriteString(const String& str, bool withNull = true);
	void WriteInt8(_s8 data) { Write((const _u8*)&data, 0, sizeof(_s8)); }
	void WriteInt8U(_u8 data) { Write(&data, 0, sizeof(_u8)); }
	void WriteInt16(_s16 data) { Write((const _u8*)&data, 0, sizeof(_s16)); }
	void WriteInt16U(_u16 data) { Write((const _u8*)&data, 0, sizeof(_u16)); }
	void WriteInt32(_s32 data) { Write((const _u8*)&data, 0, sizeof(_s32)); }
	void WriteInt32U(_u32 data) { Write((const _u8*)&data, 0, sizeof(_u32)); }
	void WriteInt64(_s64 data) { Write((const _u8*)&data, 0, sizeof(_s64)); }
	void WriteInt64U(_u64 data) { Write((const _u8*)&data, 0, sizeof(_u64)); }


	// 커서의 위치를 옮긴다.
	virtual void Seek(int offset, Origin origin = Origin::eBegin) = 0;

	// 스트림의 내용을 반영한다.
	virtual bool Flush() = 0;

	// 스트림을 닫는다.
	virtual void Close() = 0;
	virtual bool IsClosed() = 0;

	// 현재 스트림에 Write 기능을 사용 할 수 있는지
	bool CanWrite() { return m_bCanWrite; }

	// 현재 스트림이 Read 기능을 사용할 수 있는지
	bool CanRead() { return m_bCanRead; }

	// 현재 스트림이 Seek 기능을 사용할 수 있는지
	bool CanSeek() { return m_bCanSeek; }

	bool IsEnd() { return m_iOffset == m_iLength; }

protected:
	int m_iOffset{}; // 현재 쓰고/읽을 커서의 위치
	int m_iLength{}; // 작성된 바이트 크기 (커서는 0 ~ Length 사이에서만 움직일 수 있다.)

	bool m_bCanWrite;
	bool m_bCanRead;
	bool m_bCanSeek;
};

using StreamPtr = SharedPtr<Stream>;

NS_END
