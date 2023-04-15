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

	virtual void OnDisconnected() {}
	virtual void OnSent(ISendPacket* sentPacket, Int32UL sentBytes) {}
	virtual void OnReceived(ICommand* cmd) {}
};

NS_JNET_END