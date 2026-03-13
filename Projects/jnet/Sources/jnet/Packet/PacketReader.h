/*
 * 작성자: 윤정도
 * 생성일: 3/3/2026 9:17:36 PM
 * =====================
 */

#pragma once

#include <jc/Primitives/RefCountObjectPtr.h>

#include <jnet/Namespace.h>
#include <jnet/Packet/Packet.h>

NS_JNET_BEGIN

class ICommand;
class IPacket;

//////////////////////////////////////////////////////////////////////////////////////////
struct PacketElementView
{
	jc::CMessage	msg_{};
	ICommand*		cmd_{};
	WSABUF			raw_{};
	_u8 type_ = 0; // PacketType::Raw / PacketType::Command / PacketType::Message
};

//////////////////////////////////////////////////////////////////////////////////////////
class PacketReader
{
public:
	PacketReader(WSABUF _buf);
	PacketReader(char* _pBuf, _u32 _len);

	enum ReadResult
	{
		rrSuccess,
		rrReadEnd,
		rrReadHeaderFailed_InsufficientHeaderSize,
		rrReadHeaderFailed_InsufficientPaylodeLen,
		rrInvalidPacket_BufferIsNull,
		rrInvalidPacket_MagicNumberMismatch,
		rrInvalidPacket_UnknownPacketType,
		rrInvalidCmd,
		rrInvalidMsg,
	};

	void	Reset();
	int		Next(OUT PacketElementView& _view);
	int		Next(OUT PacketElementView& _view, OUT _u32& _readMemSize);	// 0 리턴시 성공, -1 더이상 읽을게 없음
private:
	WSABUF packetBuf_;

	_u32 readOffset_ = 0;
	_u32 readMemOffset_ = 0;
	_u32 currHeaderMemOffset_ = 0;
	jc::CMessageView msgView_;
	bool isValidPacket_ = false;
};



NS_END
