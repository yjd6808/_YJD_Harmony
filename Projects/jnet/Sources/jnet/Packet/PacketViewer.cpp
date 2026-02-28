/*
 * 작성자: 윤정도
 * 생성일: 8/2/2023 10:15:25 PM
 * =====================
 *
 */

#include <jnet/Core.h>
#include <jnet/Packet/PacketViewer.h>

#include <jc/Digit.h>
#include <jc/Primitives/String.h>

USING_NS_JC;

NS_JNET_BEGIN

////////////////////////////////////////////////////////////////////////////////////////////
void PacketViewer::View(jnet::Transmission _transmission, char* _pData, int _len, int _cmdCount)
{
	String hex{ 1024 };

	if (_len > hex.Capacity() - 1)
	{
		_LogWarn_("패킷 뷰 실패(출력 불가능한 크기)");
		return;
	}

	Hex(_pData, _len, hex);
	_LogPlain_("\t[%c 패킷 뷰]\n\t패킷 크기: %d\n\t커맨드 수: %d\n\t헥스\n%s", jnet::TransmissionName(_transmission), _len,
	           _cmdCount, hex.Source());
}

////////////////////////////////////////////////////////////////////////////////////////////
void PacketViewer::View(jnet::Transmission _transmission, char* _pData, int _len)
{
	String hex{ 1024 };

	if (_len > hex.Capacity() - 1)
	{
		_LogWarn_("패킷 뷰 실패(출력 불가능한 크기)");
		return;
	}

	Hex(_pData, _len, hex);
	_LogPlain_("\t[%c 패킷 뷰]\n\t패킷 크기: %d\n\t헥스\n%s", jnet::TransmissionName(_transmission), _len, hex.Source());
}

////////////////////////////////////////////////////////////////////////////////////////////
void PacketViewer::View(jnet::IPacket* _pPacket)
{
	const WSABUF wsaBuf = _pPacket->GetWSABuf();

	if (_pPacket->GetType() == jnet::PacketType::Command)
	{
		View(jnet::Transmission::Send, wsaBuf.buf, wsaBuf.len, static_cast<jnet::CmdPacket*>(_pPacket)->GetCommandCount());
	}
	else if (_pPacket->GetType() == jnet::PacketType::Raw)
	{
		View(jnet::Transmission::Send, wsaBuf.buf, wsaBuf.len);
	}
	else
	{
		jc_assert(false);
	}
}

////////////////////////////////////////////////////////////////////////////////////////////
void PacketViewer::View(jnet::RecvedCmdPacket* _pPacket)
{
	
	if (_pPacket->header_.packetType_ == jnet::PacketType::Raw)
	{
	}
	else if (_pPacket->header_.packetType_ == jnet::PacketType::Command)
	{
		View(jnet::Transmission::Recv, (char*)_pPacket, jnet::PACKET_HEADER_SIZE + _pPacket->header_.payloadLen_, _pPacket->header_.cmdCount_);
	}
	else
	{
		jc_assert(false);
	}
	
}

////////////////////////////////////////////////////////////////////////////////////////////
void PacketViewer::View(jnet::ICommand* _pCmd)
{
	String hex{ 1024 };
	CmdLen_t cmdLen = _pCmd->GetLength();

	if (cmdLen > hex.Capacity() - 1)
	{
		_LogWarn_("커맨드 뷰 실패(출력 불가능한 크기)");
		return;
	}

	Hex((char*)_pCmd, cmdLen, hex);
	_LogPlain_("[커맨드 뷰]\n커맨드 타입:%d\n커맨드: %d\n크기: %d\n헥스\n%s", _pCmd->GetType(), _pCmd->GetId(), cmdLen, hex.Source());
}

////////////////////////////////////////////////////////////////////////////////////////////
void PacketViewer::Hex(char* _pData, int _len, OUT String& _hex)
{
	_hex.SetLength(0);
	_hex += '\t';

	char byteBuf[4]{ '0', '0', ' ', NULL };

	for (int index = 0; index < _len; ++index)
	{
		byteBuf[0] = Digit::HexChar[_u8(_pData[index]) / 0x10];
		byteBuf[1] = Digit::HexChar[_u8(_pData[index]) % 0x10];
		_hex += byteBuf;

		if (index != 0 && index % 10 == 9)
		{
			_hex += "\n\t";
		}
	}
}

NS_END