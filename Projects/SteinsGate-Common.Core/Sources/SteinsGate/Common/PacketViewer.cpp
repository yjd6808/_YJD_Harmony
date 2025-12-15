/*
 * 작성자: 윤정도
 * 생성일: 8/2/2023 10:15:25 PM
 * =====================
 *
 */


#include "Core.h"
#include "CommonCoreHeader.h"
#include "PacketViewer.h"

#include <JCore/Digit.h>
#include <JCore/Primitives/String.h>

USING_NS_JC;

////////////////////////////////////////////////////////////////////////////////////////////
void PacketViewer::View(JNetwork::Transmission _transmission, char* _pData, int _len, int _cmdCount)
{
	String hex{ 1024 };

	if (_len > hex.Capacity() - 1)
	{
		_LogWarn_("패킷 뷰 실패(출력 불가능한 크기)");
		return;
	}

	Hex(_pData, _len, hex);
	_LogPlain_("\t[%c 패킷 뷰]\n\t패킷 크기: %d\n\t커맨드 수: %d\n\t헥스\n%s", JNetwork::TransmissionName(_transmission), _len,
	           _cmdCount, hex.Source());
}

////////////////////////////////////////////////////////////////////////////////////////////
void PacketViewer::View(JNetwork::Transmission _transmission, char* _pData, int _len)
{
	String hex{ 1024 };

	if (_len > hex.Capacity() - 1)
	{
		_LogWarn_("패킷 뷰 실패(출력 불가능한 크기)");
		return;
	}

	Hex(_pData, _len, hex);
	_LogPlain_("\t[%c 패킷 뷰]\n\t패킷 크기: %d\n\t헥스\n%s", JNetwork::TransmissionName(_transmission), _len, hex.Source());
}

////////////////////////////////////////////////////////////////////////////////////////////
void PacketViewer::View(JNetwork::IPacket* _pPacket)
{
	const WSABUF wsaBuf = _pPacket->GetWSABuf();

	if (_pPacket->GetType() == JNetwork::PacketType::Command)
	{
		View(JNetwork::Transmission::Send, wsaBuf.buf, wsaBuf.len,
		     static_cast<JNetwork::CommandPacket*>(_pPacket)->GetCommandCount());
	}
	else if (_pPacket->GetType() == JNetwork::PacketType::Raw)
	{
		View(JNetwork::Transmission::Send, wsaBuf.buf, wsaBuf.len);
	}
	else
	{
		DebugAssert(false);
	}
}

////////////////////////////////////////////////////////////////////////////////////////////
void PacketViewer::View(JNetwork::RecvedCommandPacket* _pPacket)
{
	View(JNetwork::Transmission::Recv, reinterpret_cast<char*>(_pPacket),
	     _pPacket->GetPacketLength() + JNetwork::PACKET_HEADER_SIZE, _pPacket->GetCommandCount());
}

////////////////////////////////////////////////////////////////////////////////////////////
void PacketViewer::View(JNetwork::ICommand* _pCmd)
{
	String hex{ 1024 };
	CmdLen_t cmdLen = _pCmd->GetLength();

	if (cmdLen > hex.Capacity() - 1)
	{
		_LogWarn_("커맨드 뷰 실패(출력 불가능한 크기)");
		return;
	}

	Hex(reinterpret_cast<char*>(_pCmd), cmdLen, hex);
	_LogPlain_("[커맨드 뷰]\n커맨드 타입:%d\n커맨드: %d\n크기: %d\n헥스\n%s", _pCmd->GetType(), _pCmd->GetId(), cmdLen, hex.Source());
}

////////////////////////////////////////////////////////////////////////////////////////////
void PacketViewer::Hex(char* _pData, int _len, JCORE_OUT String& _hex)
{
	_hex.SetLength(0);
	_hex += '\t';

	char byteBuf[4]{ '0', '0', ' ', NULL };

	for (int index = 0; index < _len; ++index)
	{
		byteBuf[0] = Digit::HexChar[Byte(_pData[index]) / 0x10];
		byteBuf[1] = Digit::HexChar[Byte(_pData[index]) % 0x10];
		_hex += byteBuf;

		if (index != 0 && index % 10 == 9)
		{
			_hex += "\n\t";
		}
	}
}
