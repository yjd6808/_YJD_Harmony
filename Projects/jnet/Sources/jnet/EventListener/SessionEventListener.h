/*
 * 작성자 : 윤정도
 */

#pragma once

#include "jc/Type.h"
#include "jc/Declspec.h"

#include "jc/Container/CMessage.h"

#include "jnet/Namespace.h"

NS_JC_BEGIN
class CMessage;
NS_END;

NS_JNET_BEGIN

class ICommand;
class IPacket;
struct RecvedPacket;

class Session;
class JC_NOVTABLE SessionEventListener
{
public:
	using FnConnected = jc::Action<jnet::Session*>;
	using FnConnectFailed = jc::Action<jnet::Session*, _u32>;
	using FnDisconnected = jc::Action<jnet::Session*, _u32>;
	using FnSent = jc::Action<jnet::Session*, jnet::IPacket*, _u32l>;
	using FnReceivedRaw = jc::Action<jnet::Session*, char*, int>;
	using FnReceivedCmd = jc::Action<jnet::Session*, jnet::ICommand*>;
	using FnReceivedMsg = jc::Action<jnet::Session*, jc::CMessage>;
	using FnReceivedPacket = jc::Action<jnet::Session*, jnet::RecvedPacket*>;

	SessionEventListener() = default;
	virtual ~SessionEventListener() = default;

	virtual void OnConnected(Session* _pSession) {}
	virtual void OnDisconnected(Session* _pSession, _u32 _errorCode) {}
	virtual void OnSent(Session* _pSession, IPacket* _pSentPacket, _u32l _sentBytes) {}
	virtual void OnReceivedRaw(Session* _pSession, char* _pData, int _len) {}
	virtual void OnReceivedCmd(Session* _pSession, ICommand* _pRecvCmd) {}
	virtual void OnReceivedMsg(Session* _pSession, jc::CMessage _msg) {}
	virtual void OnReceivedPacket(Session* _pSession, RecvedPacket* _pRecvPacket) {}

	void SetConnectedCallback(const FnConnected& _fn) { fnConnected_ = _fn; }
	void SetConnectFailedCallback(const FnConnectFailed& _fn) { fnConnectFailed_ = _fn; }
	void SetDisconnectedCallback(const FnDisconnected& _fn) { fnDisconnected_ = _fn; }
	void SetSentCallback(const FnSent& _fn) { fnSent_ = _fn; }
	void SetReceivedRawCallback(const FnReceivedRaw& _fn) { fnReceivedRaw_ = _fn; }
	void SetReceivedCmdCallback(const FnReceivedCmd& _fn) { fnReceivedCmd_ = _fn; }
	void SetReceivedMsgCallback(const FnReceivedMsg& _fn) { fnReceivedMsg_ = _fn; }
	void SetReceivedPacketCallback(const FnReceivedPacket& _fn) { fnReceivedPacket_ = _fn; }

	void SetConnectedCallback(FnConnected&& _fn) { fnConnected_ = std::move(_fn); }
	void SetConnectFailedCallback(FnConnectFailed&& _fn) { fnConnectFailed_ = std::move(_fn); }
	void SetDisconnectedCallback(FnDisconnected&& _fn) { fnDisconnected_ = std::move(_fn); }
	void SetSentCallback(FnSent&& _fn) { fnSent_ = std::move(_fn); }
	void SetReceivedRawCallback(FnReceivedRaw&& _fn) { fnReceivedRaw_ = std::move(_fn); }
	void SetReceivedCmdCallback(FnReceivedCmd&& _fn) { fnReceivedCmd_ = std::move(_fn); }
	void SetReceivedMsgCallback(FnReceivedMsg&& _fn) { fnReceivedMsg_ = std::move(_fn); }
	void SetReceivedPacketCallback(FnReceivedPacket&& _fn) { fnReceivedPacket_ = std::move(_fn); }

protected:
	FnConnected fnConnected_;
	FnConnectFailed fnConnectFailed_;
	FnDisconnected fnDisconnected_;
	FnSent fnSent_;
	FnReceivedRaw fnReceivedRaw_;
	FnReceivedCmd fnReceivedCmd_;
	FnReceivedMsg fnReceivedMsg_;
	FnReceivedPacket fnReceivedPacket_;
};

NS_END
