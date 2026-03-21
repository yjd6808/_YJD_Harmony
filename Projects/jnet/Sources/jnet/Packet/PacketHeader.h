#pragma once

#include "jc/Type.h"
#include "jc/Define.h"

#include "jnet/Namespace.h"

NS_JNET_BEGIN

// 패킷 헤더
using MagicNum_t = _u16;
using PaloadLen_t = _u16;
using ElemCnt_t = _u8;

JC_SENUM_BEGIN_2(PacketType, _u8)
None,
Raw,
Command,
Message,
JC_SENUM_END(PacketType)

inline constexpr int PACKET_MAGIC_NUMBER = 0xAFD3;

#pragma pack(push, 1)
struct PacketHeader
{
	MagicNum_t	magicNumber_ = PACKET_MAGIC_NUMBER;		// 2 byte
	PaloadLen_t	payloadLen_ = 0;						// 2 byte
	_u8			packetType_ = PacketType::None;			// 1 byte
	ElemCnt_t	elemCount_ = 0;							// 1 byte
};
#pragma pack(pop)

inline constexpr int PACKET_HEADER_SIZE = sizeof(PacketHeader);
inline static PacketHeader DefaultPacketHeader;

NS_END
