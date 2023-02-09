/*
 * 작성자: 윤정도
 * 생성일: 2/9/2023 10:48:34 PM
 * =====================
 *
 */


#include <JNetwork/Network.h>
#include <JNetwork/Packet/SendPacket.h>

NS_JNET_BEGIN

CommandBufferPacket::CommandBufferPacket(const CommandBufferPtr& buffer)
	: m_Buffer(buffer)
{}


NS_JNET_END