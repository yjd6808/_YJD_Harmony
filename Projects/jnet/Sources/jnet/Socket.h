/*
 *	작성자 : 윤정도
 */

#pragma once

#include <jc/Type.h>

#include <jnet/Protocol.h>
#include <jnet/IPEndPoint.h>


NS_JNET_BEGIN

	NS_DETAIL_BEGIN
		bool UseConnectEx();
		bool UseDisconnectEx();
	NS_DETAIL_END

class SocketOption
{
public:
	SocketOption(SOCKET _socket)
		: socket_(_socket)
	{
	}

	~SocketOption() = default;

	// 성공시 : 0
	// 실패시 : SOCKET_ERROR(-1) 반환

	int SetBroadcastEnabled(bool _enabled) const;
	int SetSendBufferSize(int _size) const;
	int SetRecvBufferSize(int _size) const;
	int SetNonBlockingEnabled(bool _enabled) const;
	int SetNagleEnabled(bool _enabled) const;
	int SetLingerEnabled(bool _enabled) const;
	int SetReuseAddrEnabled(bool _enabled) const;
	int SetExclusiveReuseAddrEnabled(bool _enabled) const;
	int SetLingerTimeout(Int16U _timeout) const;
	int SetKeepAliveEnabled(bool _enabled) const;
	int SetUpdateAcceptContext(SOCKET _listeningSocket) const;
	
	// 성공시 : 수치값
	// 실패시 : SOCKET_ERROR(-1) 반환

	int GetSendBufferSize() const;
	int GetRecvBufferSize() const;
	int GetLingerTimeout() const;
	int GetMaximumSegmentSize() const;
	int GetErrorCode() const;
	
	// 활성화된 경우 TRUE(1), 비활성화된 경우 FALSE(0)을 반환
	// 오류로 실패한 경우 SOCKET_ERROR(-1) 반환

	int IsNagleEnabled() const;
	int IsReuseAddressEnabled() const;
	int IsExclusiveReuseAddressEnabled() const;
	int IsLingerEnabled() const;
	int IsKeepAliveEnabled() const;

private:
	SOCKET socket_;
};

class TcpSocketv4;
class Socketv6;
class Socketv4;
class Socket
{
public:
	enum State
	{
		eInitialized	= 0,
		eBinded			= 1,
		eListen			= 2
	};

	Socket();
	Socket(TransportProtocol _protocol, SOCKET _socket);

	bool IsBinded() const { return State == eBinded; }
	bool IsListening() const { return State == eListen; }

	SocketOption Option() const { return SocketOption(Handle); }
	bool IsValid() const { return Handle != INVALID_SOCKET; }
	void Invalidate();

	// 반환값 실패시 SOCKET_ERROR - WSAGetLastError로 확인
	//       성공시 0
	int ShutdownBoth() const;
	int ShutdownWrite() const;
	int ShutdownRead() const;
	int Close();
	const char* ProtocolName();

	static Socketv4 CreateV4(TransportProtocol _protocol, bool _overlapped);
	static Socketv4 CreateTcpV4(bool _overlapped);
	static Socketv6 CreateV6(TransportProtocol _protocol, bool _overlapped);

	State State;
	SOCKET Handle;
	TransportProtocol Protocol;
};


class Socketv4 : public Socket
{
public:
	Socketv4()
		: Socket()
	{
	}

	Socketv4(TransportProtocol _protocol, SOCKET _socket)
		: Socket(_protocol, _socket)
	{
	}

	// 반환값 성공시 0
	// 실패시 SOCKET_ERROR, WSAGetLastError()로 확인
	int Bind(const IPv4EndPoint& _ipv4EndPoint);
	int BindAny();

	int Listen(int _connectionWaitingQueueSize = 15);

	Socketv4 Accept();

	// 반환값 실패시 FALSE, WSAGetLastError로 확인
	//       성공시 TRUE
	int AcceptEx(SOCKET _listeningSocket, void* _pOutputBuffer, Int32UL _receiveDataLength, OUT Int32UL* _pReceivedBytes, LPOVERLAPPED _pOverlapped) const;
	static void AcceptExResult(char* _pBuffer, Int32UL _receiveDataLength, OUT IPv4EndPoint* _pLocalEndPoint, OUT IPv4EndPoint* _pRemoteEndPoint);

	// 반환값 실패시 SOCKET_ERROR
	//       성공시 0
	int Connect(const IPv4EndPoint& _ipv4EndPoint) const;

	// 반환값 실패시 FALSE, WSAGetLastError로 확인
	//       성공시 TRUE
	int ConnectEx(const IPv4EndPoint& _ipv4EndPoint, LPOVERLAPPED _pOverlapped, char* _pSendBuffer, Int32UL _sendBufferSize, OUT Int32UL* _pSentBytes) const;
	int DisconnectEx(LPOVERLAPPED _pOverlapped, Int32UL _flag);
	 
	int Send(char* _pBuff, Int32U _length, Int32U _flag = 0) const;
	int SendTo(char* _pBuff, Int32U _length, const IPv4EndPoint& _ipv4EndPoint, Int32U _flag = 0) const;

	// 반환: 연결이 정상적으로 닫힌경우 0반환
	//      오류가 발생하지 않은 경우 수신된 바이트 크기를 반환
	int Receive(char* _pBuff, Int32U _bufferSize, Int32U _flag = 0) const;
	int ReceiveFrom(char* _pBuff, Int32U _bufferSize, OUT IPv4EndPoint* _pIpv4EndPoint, Int32U _flag = 0) const;

	int SendEx(
		LPWSABUF _pBuf,
		OUT Int32UL* _pBytesSent,
		LPOVERLAPPED _pOverlapped,
		LPWSAOVERLAPPED_COMPLETION_ROUTINE _pCompRoutine = nullptr,
		Int32U _flag = 0
	) const;

	int SendToEx(
		LPWSABUF _pBuffers,
		OUT Int32UL* _pBytesSent,
		LPOVERLAPPED _pOverlapped,
		const IPv4EndPoint& _to,
		LPWSAOVERLAPPED_COMPLETION_ROUTINE _pCompRoutine = nullptr
	) const;

	int ReceiveEx(
		LPWSABUF _pBuf,
		OUT Int32UL* _pBytesReceived,
		LPOVERLAPPED _pOverlapped,
		LPWSAOVERLAPPED_COMPLETION_ROUTINE _pCompRoutine = nullptr,
		Int32U _flag = 0
	) const;

	int ReceiveFromEx(
		LPWSABUF _pBuf,
		OUT Int32UL* _pBytesReceived,
		LPOVERLAPPED _pOverlapped,
		OUT SOCKADDR_IN* _pSenderAddr,
		LPWSAOVERLAPPED_COMPLETION_ROUTINE _pCompRoutine = nullptr,
		Int32U _flag = 0
	) const;

	IPv4EndPoint GetLocalEndPoint() const;
	IPv4EndPoint GetRemoteEndPoint() const;

	TransportProtocol GetTransportProtocol() const { return Protocol; }
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


NS_JNET_END
