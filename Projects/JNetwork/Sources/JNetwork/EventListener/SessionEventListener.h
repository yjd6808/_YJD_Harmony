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

	virtual void OnConnected(Session* session) {}
	virtual void OnDisconnected(Session* session) {}
	virtual void OnSent(Session* session, ISendPacket* sentPacket, Int32UL sentBytes) {}
	virtual void OnReceived(Session* session, ICommand* cmd) {}
};

NS_JNET_END