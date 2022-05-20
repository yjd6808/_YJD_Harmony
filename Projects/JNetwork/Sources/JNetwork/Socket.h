/*
 *	작성자 : 윤정도
 */

#pragma once

#include <JCore/Type.h>

#include <JNetwork/Protocol.h>
#include <JNetwork/IPEndPoint.h>


#ifndef Out_
	#define Out_
#endif

namespace JNetwork {
class SocketOption
{
public:
	SocketOption(SOCKET sock) : m_Socket(sock) {}
	~SocketOption() = default;

	// 성공시 : 0
	// 실패시 : SOCKET_ERROR(-1) 반환

	int SetBroadcastEnabled(bool enabled) const;
	int SetSendBufferSize(int size) const;
	int SetRecvBufferSize(int size) const;
	int SetNonBlockingEnabled(bool enabled) const;
	int SetNagleEnabled(bool enabled) const;
	int SetLingerEnabled(bool enabled) const;
	int SetReuseAddrEnabled(bool enabled) const;
	int SetExclusiveReuseAddrEnabled(bool enabled) const;
	int SetLingerTimeout(Int16U timeout) const;
	int SetKeepAliveEnabled(bool enabled) const;
	int SetUpdateAcceptContext(SOCKET hListeningSocket) const;
	
	// 성공시 : 수치값
	// 실패시 : SOCKET_ERROR(-1) 반환

	int GetSendBufferSize() const;
	int GetRecvBufferSize() const;
	int GetLingerTimeout() const;
	int GetMaximumSegmentSize() const;
	
	// 활성화된 경우 TRUE(1), 비활성화된 경우 FALSE(0)을 반환
	// 오류로 실패한 경우 SOCKET_ERROR(-1) 반환

	int IsNagleEnabled() const;
	int IsReuseAddressEnabled() const;
	int IsExclusiveReuseAddressEnabled() const;
	int IsLingerEnabled() const;
	int IsKeepAliveEnabled() const;
private:
	SOCKET m_Socket;
};

class TcpSocketv4;
class Socketv6;
class Socketv4;
class Socket
{
public:
	Socket() : Socket(TransportProtocol::None, INVALID_SOCKET) {}
	Socket(TransportProtocol tpproto, SOCKET socket) :
		m_Socket(socket),
		m_SocketOption(socket),
		m_TransportProtocol(tpproto)
	{
	}
	virtual ~Socket() = default;

	SocketOption Option() const { return m_SocketOption; }
	SOCKET Handle() const { return m_Socket; }
	bool IsValid() const { return m_Socket != INVALID_SOCKET; }
	int ShutdownBoth() const;
	int ShutdownWrite() const;
	int ShutdownRead() const;
	int Close(); 
public:
	static Socketv4 CreateV4(TransportProtocol tpproto, bool overlapped);
	static Socketv4 CreateTcpV4(bool overlapped);
	static Socketv6 CreateV6(TransportProtocol tpproto, bool overlapped);
protected:
	SOCKET m_Socket;
	SocketOption m_SocketOption;
	TransportProtocol m_TransportProtocol;
};


class Socketv4 : public Socket
{
public:
	Socketv4() : Socket() {}
	Socketv4(TransportProtocol tpproto, SOCKET socket) : Socket(tpproto, socket) {}
	~Socketv4() override = default;

	int Bind(const IPv4EndPoint& ipv4EndPoint) const;
	int BindAny() const;

	int Listen(int connectionWaitingQueueSize = 15) const;

	Socketv4 Accept();

	// 반환값 실패시 FALSE, WSAGetLastError로 확인
	//       성공시 TRUE
	int AcceptEx(SOCKET listenSocket, void* outputBuffer, DWORD receiveDatalen, Out_ LPDWORD receivedBytes, LPOVERLAPPED overlapped) const;
	static void AcceptExResult(char* buff, DWORD receiveDatalen, Out_ IPv4EndPoint* localEp, Out_ IPv4EndPoint* remoteEp);

	int Connect(const IPv4EndPoint& ipv4EndPoint) const;
	int ConnectEx(const IPv4EndPoint& ipv4EndPoint, LPOVERLAPPED overlapped, char* sendbuf, DWORD sendbufSize, Out_ LPDWORD sentBytes) const; 
	 
	int Send(char* buff, Int32U len, Int32U flag = 0) const;
	int SendTo(char* buff, Int32U len, const IPv4EndPoint& ipv4EndPoint, Int32U flag = 0) const;
	int Receive(char* buff, Int32U buffSize, Int32U flag = 0) const;
	int ReceiveFrom(char* buff, Int32U buffSize, Out_ IPv4EndPoint* ipv4EndPoint, Int32U flag = 0) const;

	int SendEx(LPWSABUF lpBuf, Out_ Int32UL* pBytesSent, LPOVERLAPPED lpOverlapped, LPWSAOVERLAPPED_COMPLETION_ROUTINE lpCompRoutine = NULL, Int32U flag = 0) const;
	 // int SendToEx() { return 0; } // NOT IMPLEMENTED, UNUSED
	int ReceiveEx(LPWSABUF lpBuf, Out_ Int32UL* pBytesReceived, LPOVERLAPPED lpOverlapped, LPWSAOVERLAPPED_COMPLETION_ROUTINE lpCompRoutine = NULL, Int32U flag = 0) const;
	//  int ReceiveFromEx() {} // NOT IMPLEMENTED, UNUSED

	IPv4EndPoint GetLocalEndPoint() const;
	IPv4EndPoint GetRemoteEndPoint() const;

	TransportProtocol GetTransportProtocol() const { return m_TransportProtocol; }
	static InternetProtocol GetInternetProtocol() { return InternetProtocol::IPv4; }
};

class Socketv6 final : public Socket
{
	// not implemented : unused
	/*
	Socketv6() : Socket() {}
	Socketv6(TransportProtocol tpproto, SOCKET socket) : Socket(tpproto, socket) {}
	~Socketv6() override {}

	int Bind(const IPv6EndPoint& ipv6EndPoint);
	int Listen(int connectionWaitingQueueSize = 15);
	int Accept(Socketv6& serverSocket);
	int Connet(const IPv6EndPoint& ipv6EndPoint);
	int Send(char* buff, Int32U len, Int32U flag = 0);
	int SendTo(char* buff, Int32U len, const IPv6EndPoint& ipv6EndPoint, Int32U flag = 0);
	int Receive(char* buff, Int32U buffSize, Int32U flag = 0);
	int ReceiveFrom(char* buff, Int32U buffSize, Int32U flag = 0);
	*/
};



}

