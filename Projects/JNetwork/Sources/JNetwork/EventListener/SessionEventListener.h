/*
 * 작성자 : 윤정도
 */

#pragma once

#include <JCore/Type.h>
#include <JNetwork/Namespace.h>

NS_JNET_BEGIN

struct ICommand;
struct ISendPacket;

class Session;
class SessionEventListener
{
public:
	SessionEventListener() = default;
	virtual ~SessionEventListener() = default;

	virtual void OnDisconnected() = 0;
	virtual void OnSent(ISendPacket* sentPacket, Int32UL sentBytes) = 0;
	virtual void OnReceived(ICommand* cmd) = 0;
};

NS_JNET_END