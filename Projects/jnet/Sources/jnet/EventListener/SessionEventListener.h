/*
 * 작성자 : 윤정도
 */

#pragma once

#include <jc/Type.h>
#include <jc/Declspec.h>

#include <jnet/Namespace.h>

NS_JNET_BEGIN

class ICommand;
class IPacket;
class RecvedCommandPacket;

class Session;
class JC_NOVTABLE SessionEventListener
{
public:
	SessionEventListener() = default;
	virtual ~SessionEventListener() = default;

	virtual void OnConnected(Session* _pSession) {}
	virtual void OnDisconnected(Session* _pSession, Int32U _errorCode) {}
	virtual void OnSent(Session* _pSession, IPacket* _pSentPacket, Int32UL _sentBytes) {}
	virtual void OnReceivedRaw(Session* _pSession, char* _pData, int _len) {}
	virtual void OnReceived(Session* _pSession, ICommand* _pRecvCmd) {}
	virtual void OnReceived(Session* _pSession, RecvedCommandPacket* _pRecvPacket) {}
};

NS_JNET_END
