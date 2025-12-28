/*
 *	작성자 : 윤정도
 */

#include <JNetwork/Network.h>
#include <JNetwork/Socket.h>
#include <JNetwork/ByteOrder.h>

#include <MSWSock.h>

static LPFN_DISCONNECTEX lpfnDisconnectEx = nullptr;
static LPFN_CONNECTEX lpfnConnectEx = nullptr;

NS_JNET_BEGIN

	NS_DETAIL_BEGIN

//////////////////////////////////////////////////////////////////////////////////////////
	template <typename LPFN>
	static bool UseEx(LPFN _pPfn, GUID& _guid)
	{
		Socketv4 dummySocket = Socket::CreateTcpV4(false);

		if (!dummySocket.IsValid())
			return false;

		{
			Int32UL bytes;
			int result = WSAIoctl(
				dummySocket.Handle,
				SIO_GET_EXTENSION_FUNCTION_POINTER,
				&_guid,
				sizeof(_guid),
				_pPfn,
				sizeof(LPFN),
				&bytes,
				NULL,
				NULL);

			if (result != 0)
				return false;
		}

		if (dummySocket.Close() == SOCKET_ERROR)
		{
			DebugAssertMsg(false, "함수주소 획득을 위해 생성한 더미 소켓을 닫는데 실패했습니다.");
		}

		return true;
	}

//////////////////////////////////////////////////////////////////////////////////////////
	bool UseConnectEx()
	{
		if (lpfnConnectEx != nullptr)
			return true;

		GUID guid = WSAID_CONNECTEX;
		bool isSucceeded = UseEx(&lpfnConnectEx, guid);

		if (isSucceeded && lpfnConnectEx)
			return true;

		DebugAssertMsg(false, "ConnectEx 함수 주소를 얻는데 실패했습니다.");
		return false;
	}

//////////////////////////////////////////////////////////////////////////////////////////
	bool UseDisconnectEx()
	{
		if (lpfnDisconnectEx != nullptr)
			return true;

		GUID guid = WSAID_DISCONNECTEX;
		bool isSucceeded = UseEx(&lpfnDisconnectEx, guid);

		if (isSucceeded && lpfnDisconnectEx)
			return true;

		DebugAssertMsg(false, "DisconnectEx 함수 주소를 얻는데 실패했습니다.");
		return false;
	}	

	NS_DETAIL_END

/*=====================================================================================
						Socket
=======================================================================================*/

//////////////////////////////////////////////////////////////////////////////////////////
Socket::Socket()
	: State(eInitialized)
	, Handle(INVALID_SOCKET)
	, Protocol(TransportProtocol::None)
{
}

//////////////////////////////////////////////////////////////////////////////////////////
Socket::Socket(TransportProtocol _protocol, SOCKET _socket)
	: State(eInitialized)
	, Handle(_socket)
	, Protocol(_protocol)
{
}

//////////////////////////////////////////////////////////////////////////////////////////
void Socket::Invalidate()
{
	Protocol = TransportProtocol::None;
	Handle = INVALID_SOCKET;
	State = eInitialized;
}

//////////////////////////////////////////////////////////////////////////////////////////
int Socket::ShutdownBoth() const
{
	return shutdown(Handle, SD_BOTH);
}

//////////////////////////////////////////////////////////////////////////////////////////
int Socket::ShutdownWrite() const
{
	return shutdown(Handle, SD_SEND);
}

//////////////////////////////////////////////////////////////////////////////////////////
int Socket::ShutdownRead() const
{
	return shutdown(Handle, SD_RECEIVE);
}

//////////////////////////////////////////////////////////////////////////////////////////
int Socket::Close()
{
	if (!IsValid())
		return 0;

	const int result = closesocket(Handle);

	if (result == 0)
	{
		Handle = INVALID_SOCKET;
	}

	return result;
}

//////////////////////////////////////////////////////////////////////////////////////////
const char* Socket::ProtocolName()
{
	switch (Protocol)
	{
	case TransportProtocol::None: return "NONE";
	case TransportProtocol::TCP:  return "TCP";
	case TransportProtocol::UDP:  return "UDP";
	}

	return "";
}

//////////////////////////////////////////////////////////////////////////////////////////
Socketv4 Socket::CreateV4(TransportProtocol _protocol, bool _overlapped)
{
	SOCKET socketHandle = WSASocketW(
		AF_INET,
		_protocol == TransportProtocol::TCP ? SOCK_STREAM : SOCK_DGRAM,
		static_cast<int>(_protocol),
		nullptr,
		0,
		_overlapped ? WSA_FLAG_OVERLAPPED : 0);

	return { _protocol, socketHandle };
}

//////////////////////////////////////////////////////////////////////////////////////////
Socketv4 Socket::CreateTcpV4(bool _overlapped)
{
	SOCKET socketHandle = WSASocketW(
		AF_INET,
		SOCK_STREAM,
		IPPROTO_TCP,
		nullptr,
		0,
		_overlapped ? WSA_FLAG_OVERLAPPED : 0);

	return { TransportProtocol::TCP, socketHandle };
}

//////////////////////////////////////////////////////////////////////////////////////////
Socketv6 Socket::CreateV6(TransportProtocol _protocol, bool _overlapped)
{
	// UNUSED
	(void)_protocol;
	(void)_overlapped;
	return {};
}

/*=====================================================================================
						Socketv4
=======================================================================================*/

//////////////////////////////////////////////////////////////////////////////////////////
int Socketv4::Bind(const IPv4EndPoint& _ipv4EndPoint)
{
	SOCKADDR_IN address;
	address.sin_family = AF_INET;
	address.sin_port = ByteOrder::HostToNetwork(_ipv4EndPoint.GetPort());
	address.sin_addr.S_un.S_addr = ByteOrder::HostToNetwork(_ipv4EndPoint.GetAddress().GetAddress());

	int bindResult = bind(Handle, reinterpret_cast<sockaddr*>(&address), sizeof(SOCKADDR_IN));

	if (bindResult == 0)
	{
		State = eBinded;
	}

	return bindResult;
}

//////////////////////////////////////////////////////////////////////////////////////////
int Socketv4::BindAny()
{
	return Bind({ IPv4Address::Any(), 0 });
}

//////////////////////////////////////////////////////////////////////////////////////////
int Socketv4::Listen(int _connectionWaitingQueueSize)
{
	int listenResult = listen(Handle, _connectionWaitingQueueSize);

	if (listenResult == 0)
	{
		State = eListen;
	}

	return listenResult;
}

//////////////////////////////////////////////////////////////////////////////////////////
Socketv4 Socketv4::Accept()
{
	return { this->Protocol, accept(Handle, nullptr, nullptr) };
}

//////////////////////////////////////////////////////////////////////////////////////////
int Socketv4::AcceptEx(SOCKET _listeningSocket, void* _pOutputBuffer, Int32UL _receiveDataLength, OUT Int32UL* _pReceivedBytes, LPOVERLAPPED _pOverlapped) const
{
	// @참고 : https://docs.microsoft.com/en-us/windows/win32/api/mswsock/nf-mswsock-acceptex
	// sListenSocket : 서버 소켓
	// sAcceptSocket : 연결을 수락받을 세션 소켓
	// lpOutputBuffer : 버퍼의 오프셋 0바이트 부터 상대 클라이언트가 Connect하면서 전달한 데이터를 수신하도록 하고 그 뒤에 이어서 로컬 주소, 원격 주소 정보가 저장된다.
	//					lpOutputBuffer 크기 >= dwLocalAddressLength + dwRemoteAddressLength가 되어야한다.
	// dwReceiveDataLength : 이 사이즈는 상대 소켓이 연결하면서 전달한 정보를 담을 크기를 나타낸다. 이게 0일 경우 데이터를 받지 않는듯?
	//					 ConnectEx에 전송하는 기능이 있던데 거기서 전송한 데이터를 lpOutputBuffer여기로 받는 것으로 추측된다. 테스트는 안해봄
	//					 lpOutputBuffer의 시작 주소부터 데이터를 받고 그 이후에 로컬 주소, 원격 주소 정보가 담기는가보다.
	//					 로컬 주소, 원격 주소를 제외한 크기를 전달해줘야한다.
	// dwLocalAddressLength : 로컬 주소 정보 크기를 전달한다. 전송 프로토콜의 최대 주소 크기보다 16바이트 이상 커야한다.
	// dwRemoteAddressLength : 원격 주소 정보 크기를 전달한다. 전송 프로토콜의 최대 주소 크기보다 16바이트 이상 커야한다.
	// lpdwBytesReceived : 전송받은 데이터 크기를 저장할 Int32UL*를 전달한다. 만약 바로 완료된 경우 여기 데이터가 담길 것이다.
	//				   GetLastError()의 ERROR_IO_PENDING 오류를 받는 경우에는 완료 통지 방식으로 데이터를 읽어야한다. (오버랩 말하는 듯?)
	// lpOverlapped : NULL을 절대 전달하면 안된다. 수신한 정보가 비동기적으로 완료될 수 있으므로 오버랩 정보를 전달해야함.

	return ::AcceptEx(
		_listeningSocket,
		Handle,
		_pOutputBuffer,
		_receiveDataLength,
		sizeof(SOCKADDR_IN) + 16,
		sizeof(SOCKADDR_IN) + 16,
		_pReceivedBytes,
		_pOverlapped);
}

//////////////////////////////////////////////////////////////////////////////////////////
void Socketv4::AcceptExResult(char* _pBuffer, Int32UL _receiveDataLength, OUT IPv4EndPoint* _pLocalEndPoint, OUT IPv4EndPoint* _pRemoteEndPoint)
{
	LPSOCKADDR_IN pLocalSockAddrIn;
	LPSOCKADDR_IN pRemoteSockAddrIn;
	INT localAddressLength;
	INT remoteAddressLength;

	// @참고 : https://docs.microsoft.com/en-us/windows/win32/api/mswsock/nf-mswsock-getacceptexsockaddrs
	// AcceptEx 함수 호출결과로 연결된 소켓의 로컬 주소와 리모트 주소를 얻을 수 있도록 한다.
	// lpOutputBuffer : AcceptEx 함후 호출겨과로 데이터가 담길 첫 시작 주소값, AcceptEx에서 전달한 lpOutputBuffer와 같은 값을 넣어준다.
	// dwReceiveDataLength : lpOutputBuffer에서 수신한 데이터의 길이를 나타낸다.  AcceptEx에서 전달한 dwReceiveDataLength와 같은 값을 넣어준다.
	// dwLocalAddressLength : 로컬 주소 저장용으로 예약된 바이트 수를 전달한다. AcceptEx에서 전달한 dwLocalAddressLength와 같은 값을 넣어준다.
	// dwRemoteAddressLength : 원격 주소 저장용으로 예약된 바이트 수를 전달한다. AcceptEx에서 전달한 dwRemoteAddressLength와 같은 값을 넣어준다.
	// LocalSockaddr : 로컬 주소 정보다 담길 구조체 포인터를 전달한다.
	// LocalSockaddrLength : 로컬 주소 정보의 크기를 반환받는다.
	// RemoteSockaddr : 연결된 소켓의 원격 주소 정보를 저장할 구조체 포인터를 전달한다.
	// RemoteSockaddrLength : 원격 주소 정보의 크기를 반환받는다.
	GetAcceptExSockaddrs(
		_pBuffer,
		_receiveDataLength,
		sizeof(SOCKADDR_IN) + 16,
		sizeof(SOCKADDR_IN) + 16,
		reinterpret_cast<sockaddr**>(&pLocalSockAddrIn),
		&localAddressLength,
		reinterpret_cast<sockaddr**>(&pRemoteSockAddrIn),
		&remoteAddressLength);

	if (_pLocalEndPoint)
	{
		ULONG ip = pLocalSockAddrIn->sin_addr.S_un.S_addr;
		char ipstr[256];
		sprintf_s(
			ipstr,
			"%d.%d.%d.%d:%d\n",
			(ip >> 0) & 0xff,
			(ip >> 8) & 0xff,
			(ip >> 16) & 0xff,
			(ip >> 24) & 0xff,
			pLocalSockAddrIn->sin_port);

		const IPv4Address v4Address{ ByteOrder::NetworkToHost(pLocalSockAddrIn->sin_addr.S_un.S_addr) };
		const IPv4EndPoint v4EndPoint{ v4Address, ByteOrder::NetworkToHost(pLocalSockAddrIn->sin_port) };

		*_pLocalEndPoint = v4EndPoint;
	}

	if (_pRemoteEndPoint)
	{
		const IPv4Address v4Address{ ByteOrder::NetworkToHost(pRemoteSockAddrIn->sin_addr.S_un.S_addr) };
		const IPv4EndPoint v4EndPoint{ v4Address, ByteOrder::NetworkToHost(pRemoteSockAddrIn->sin_port) };

		*_pRemoteEndPoint = v4EndPoint;
	}
}

//////////////////////////////////////////////////////////////////////////////////////////
int Socketv4::Connect(const IPv4EndPoint& _ipv4EndPoint) const
{
	SOCKADDR_IN address;
	address.sin_family = AF_INET;
	address.sin_port = ByteOrder::HostToNetwork(_ipv4EndPoint.GetPort());
	address.sin_addr.S_un.S_addr = ByteOrder::HostToNetwork(_ipv4EndPoint.GetAddress().GetAddress());
	return connect(Handle, reinterpret_cast<sockaddr*>(&address), sizeof(SOCKADDR_IN));
}

// Windows Vista 이전에서는 AcceptEx 함수의 주소를 얻어와서 사용해야했는데 바뀌었다고 한다.
// 그냥 써도 잘 동작한다.
// 대신 AcceptEx가 MSWSock.h와 Winsock.h 이 2개에만 있어서 사용할려면 MSWSock.h를 include하고 MSWSock.lib을 링크 해줘야함.
// @참고 : https://jacking75.github.io/cpp_iocp_extension_method/
// @AcceptEx 함수에 대한 설명 : https://programmingdiary.tistory.com/4

// @참고 : https://docs.microsoft.com/en-us/windows/win32/api/mswsock/nc-mswsock-lpfn_connectex
// ConnectEx 사용 예시 코드 : https://gist.github.com/joeyadams/4158972
// 
// [in] s :   connect()의 s와 동일
// [in] name :    connect()의 name과 동일
// [in] namelen : connect()의 namelen과 동일
// [in, opt]	lpSendBuffer : Connection이 Established 된 후에 전달한 버퍼의 데이터를 전송한다.
//					 소켓 s에 TCP_FASTOPEN이 ConnectEx() 호출전에 활성화 되어 있으면 연결 성립 중에 일부 데이터가 전송될 수 있다.
// [in]         dwSendDataLength : lpSendBuffer의 바이트 크기를 전달한다 만약 lpSendBuffer를 NULL로 전달하면 이 값은 무시된다.
// [out]		lpdwBytesSent : 함수 반환값이 TRUE 된 경우 이 연결이 성립된 후 전송된 바이트 크기를 반환해준다. lpSendBuffer를 NULL로 전달하면 이 값은 무시된다.
// [in]			lpOverlapped : 절대 NULL 전달하면 안됨

//////////////////////////////////////////////////////////////////////////////////////////
int Socketv4::ConnectEx(const IPv4EndPoint& _ipv4EndPoint, LPOVERLAPPED _pOverlapped, char* _pSendBuffer, Int32UL _sendBufferSize, OUT Int32UL* _pSentBytes) const
{
	SOCKADDR_IN address;
	address.sin_family = AF_INET;
	address.sin_port = ByteOrder::HostToNetwork(_ipv4EndPoint.GetPort());
	address.sin_addr.S_un.S_addr = ByteOrder::HostToNetwork(_ipv4EndPoint.GetAddress().GetAddress());
	DebugAssertMsg(lpfnConnectEx != nullptr, "ConnectEx 함수를 사용할려면 먼저 UseConnectEx를 호출해주세요");
	DebugAssertMsg(Protocol == TransportProtocol::TCP, "커넥션 오리엔티드 소켓만 사용가능합니다.");
	return lpfnConnectEx(
		Handle,
		reinterpret_cast<sockaddr*>(&address),
		sizeof(SOCKADDR_IN),
		_pSendBuffer,
		_sendBufferSize,
		_pSentBytes,
		_pOverlapped);
}

// @참고: https://learn.microsoft.com/en-us/windows/win32/api/mswsock/nc-mswsock-lpfn_connectex
// 커넥션 오리엔티드 소켓만 사용가능
//////////////////////////////////////////////////////////////////////////////////////////
int Socketv4::DisconnectEx(LPOVERLAPPED _pOverlapped, Int32UL _flag)
{
	DebugAssertMsg(lpfnDisconnectEx != nullptr, "DisconnectEx 함수를 사용할려면 먼저 UseDisconnectEx를 호출해주세요");
	DebugAssertMsg(Protocol == TransportProtocol::TCP, "커넥션 오리엔티드 소켓만 사용가능합니다.");
	return lpfnDisconnectEx(Handle, _pOverlapped, _flag, 0);
}

//////////////////////////////////////////////////////////////////////////////////////////
int Socketv4::Send(char* _pBuff, Int32U _length, Int32U _flag) const
{
	return send(Handle, _pBuff, static_cast<int>(_length), static_cast<int>(_flag));
}

//////////////////////////////////////////////////////////////////////////////////////////
int Socketv4::SendTo(char* _pBuff, Int32U _length, const IPv4EndPoint& _ipv4EndPoint, Int32U _flag) const
{
	SOCKADDR_IN address;
	address.sin_family = AF_INET;
	address.sin_port = ByteOrder::HostToNetwork(_ipv4EndPoint.GetPort());
	address.sin_addr.S_un.S_addr = ByteOrder::HostToNetwork(_ipv4EndPoint.GetAddress().GetAddress());
	return sendto(
		Handle,
		_pBuff,
		static_cast<int>(_length),
		static_cast<int>(_flag),
		reinterpret_cast<sockaddr*>(&address),
		sizeof(SOCKADDR_IN));
}

//////////////////////////////////////////////////////////////////////////////////////////
int Socketv4::Receive(char* _pBuff, Int32U _bufferSize, Int32U _flag) const
{
	return recv(Handle, _pBuff, static_cast<int>(_bufferSize), static_cast<int>(_flag));
}

//////////////////////////////////////////////////////////////////////////////////////////
int Socketv4::ReceiveFrom(char* _pBuff, Int32U _bufferSize, OUT IPv4EndPoint* _pIpv4EndPoint, Int32U _flag) const
{
	SOCKADDR_IN address;
	int addressSize = sizeof(SOCKADDR_IN);
	const int receivedBytes = recvfrom(
		Handle,
		_pBuff,
		static_cast<int>(_bufferSize),
		static_cast<int>(_flag),
		reinterpret_cast<sockaddr*>(&address),
		&addressSize);

	*_pIpv4EndPoint = IPv4EndPoint{ address };
	return receivedBytes;
}

//////////////////////////////////////////////////////////////////////////////////////////
int Socketv4::SendEx(
	LPWSABUF _pBuf,
	OUT Int32UL* _pBytesSent,
	LPOVERLAPPED _pOverlapped,
	LPWSAOVERLAPPED_COMPLETION_ROUTINE _pCompRoutine,
	Int32U _flag) const
{
	return WSASend(Handle, _pBuf, 1, _pBytesSent, _flag, _pOverlapped, _pCompRoutine);
}

//////////////////////////////////////////////////////////////////////////////////////////
int Socketv4::SendToEx(
	LPWSABUF _pBuffers,
	OUT Int32UL* _pBytesSent,
	LPOVERLAPPED _pOverlapped,
	const IPv4EndPoint& _to,
	LPWSAOVERLAPPED_COMPLETION_ROUTINE _pCompRoutine) const
{
	SOCKADDR_IN address;
	address.sin_family = AF_INET;
	address.sin_port = ByteOrder::HostToNetwork(_to.GetPort());
	address.sin_addr.S_un.S_addr = ByteOrder::HostToNetwork(_to.GetAddress().GetAddress());
	return WSASendTo(
		Handle,
		_pBuffers,
		1,
		_pBytesSent,
		0,
		reinterpret_cast<sockaddr*>(&address),
		sizeof(SOCKADDR_IN),
		_pOverlapped,
		_pCompRoutine);
}

//////////////////////////////////////////////////////////////////////////////////////////
int Socketv4::ReceiveEx(
	LPWSABUF _pBuf,
	OUT Int32UL* _pBytesReceived,
	LPOVERLAPPED _pOverlapped,
	LPWSAOVERLAPPED_COMPLETION_ROUTINE _pCompRoutine,
	Int32U _flag) const
{
	return WSARecv(Handle, _pBuf, 1, _pBytesReceived, reinterpret_cast<Int32UL*>(&_flag), _pOverlapped, _pCompRoutine);
}

//////////////////////////////////////////////////////////////////////////////////////////
int Socketv4::ReceiveFromEx(
	LPWSABUF _pBuf,
	OUT Int32UL* _pBytesReceived,
	LPOVERLAPPED _pOverlapped,
	OUT SOCKADDR_IN* _pSenderAddr,
	LPWSAOVERLAPPED_COMPLETION_ROUTINE _pCompRoutine,
	Int32U _flag) const
{
	int addressLength = sizeof(SOCKADDR_IN);
	return WSARecvFrom(
		Handle,
		_pBuf,
		1,
		_pBytesReceived,
		reinterpret_cast<Int32UL*>(&_flag),
		reinterpret_cast<sockaddr*>(_pSenderAddr),
		&addressLength,
		_pOverlapped,
		_pCompRoutine);
}

//////////////////////////////////////////////////////////////////////////////////////////
IPv4EndPoint Socketv4::GetLocalEndPoint() const
{
	SOCKADDR_IN address;
	int addressSize = sizeof(SOCKADDR_IN);
	ZeroMemory(&address, sizeof(SOCKADDR_IN));

	if (getsockname(Handle, reinterpret_cast<SOCKADDR*>(&address), &addressSize) != SOCKET_ERROR)
	{
		return IPv4EndPoint{ address };
	}

	return { IPv4Address{}, 0 };
}

//////////////////////////////////////////////////////////////////////////////////////////
IPv4EndPoint Socketv4::GetRemoteEndPoint() const
{
	SOCKADDR_IN address;
	int addressSize = sizeof(SOCKADDR_IN);
	ZeroMemory(&address, sizeof(SOCKADDR_IN));

	if (getpeername(Handle, reinterpret_cast<SOCKADDR*>(&address), &addressSize) != SOCKET_ERROR)
	{
		return IPv4EndPoint{ address };
	}

	return { IPv4Address{}, 0 };
}

/*=====================================================================================
						SocketOption
=======================================================================================*/

//////////////////////////////////////////////////////////////////////////////////////////
int SocketOption::SetBroadcastEnabled(bool _enabled) const
{
	BOOL option = _enabled ? TRUE : FALSE;
	return setsockopt(socket_, SOL_SOCKET, SO_SNDBUF, reinterpret_cast<char*>(&option), sizeof(BOOL));
}

//////////////////////////////////////////////////////////////////////////////////////////
int SocketOption::SetSendBufferSize(int _size) const
{
	return setsockopt(socket_, SOL_SOCKET, SO_SNDBUF, reinterpret_cast<char*>(&_size), sizeof(int));
}

//////////////////////////////////////////////////////////////////////////////////////////
int SocketOption::SetRecvBufferSize(int _size) const
{
	return setsockopt(socket_, SOL_SOCKET, SO_RCVBUF, reinterpret_cast<char*>(&_size), sizeof(int));
}

//////////////////////////////////////////////////////////////////////////////////////////
int SocketOption::SetNonBlockingEnabled(bool _enabled) const
{
	unsigned long mode = _enabled ? 1UL : 0UL;
	return ioctlsocket(socket_, FIONBIO, &mode);
}

//////////////////////////////////////////////////////////////////////////////////////////
int SocketOption::SetNagleEnabled(bool _enabled) const
{
	BOOL noDelayOption = _enabled ? 0 : 1;
	return setsockopt(socket_, IPPROTO_TCP, TCP_NODELAY, reinterpret_cast<char*>(&noDelayOption), sizeof(BOOL));
}

//////////////////////////////////////////////////////////////////////////////////////////
int SocketOption::SetLingerEnabled(bool _enabled) const
{
	BOOL option = _enabled ? FALSE : TRUE;
	return setsockopt(socket_, SOL_SOCKET, SO_DONTLINGER, reinterpret_cast<char*>(&option), sizeof(BOOL));
}

//////////////////////////////////////////////////////////////////////////////////////////
int SocketOption::SetReuseAddrEnabled(bool _enabled) const
{
	BOOL option = _enabled ? TRUE : FALSE;
	return setsockopt(socket_, SOL_SOCKET, SO_REUSEADDR, reinterpret_cast<char*>(&option), sizeof(BOOL));
}

//////////////////////////////////////////////////////////////////////////////////////////
int SocketOption::SetExclusiveReuseAddrEnabled(bool _enabled) const
{
	BOOL option = _enabled ? TRUE : FALSE;
	return setsockopt(socket_, SOL_SOCKET, SO_EXCLUSIVEADDRUSE, reinterpret_cast<char*>(&option), sizeof(BOOL));
}

//////////////////////////////////////////////////////////////////////////////////////////
int SocketOption::SetLingerTimeout(Int16U _timeout) const
{
	LINGER linger;
	linger.l_onoff = TRUE;
	linger.l_linger = _timeout;
	return setsockopt(socket_, SOL_SOCKET, SO_LINGER, reinterpret_cast<char*>(&linger), sizeof(LINGER));
}

//////////////////////////////////////////////////////////////////////////////////////////
int SocketOption::SetKeepAliveEnabled(bool _enabled) const
{
	BOOL option = _enabled ? TRUE : FALSE;
	return setsockopt(socket_, SOL_SOCKET, SO_KEEPALIVE, reinterpret_cast<char*>(&option), sizeof(BOOL));
}

//////////////////////////////////////////////////////////////////////////////////////////
int SocketOption::SetUpdateAcceptContext(SOCKET _listeningSocket) const
{
	return setsockopt(socket_, SOL_SOCKET, SO_UPDATE_ACCEPT_CONTEXT, reinterpret_cast<char*>(&_listeningSocket), sizeof(SOCKET));
}

//////////////////////////////////////////////////////////////////////////////////////////
int SocketOption::GetSendBufferSize() const
{
	int sendBufferSize;
	int size = sizeof(int);
	const int result = getsockopt(socket_, SOL_SOCKET, SO_SNDBUF, reinterpret_cast<char*>(&sendBufferSize), &size);

	if (result == SOCKET_ERROR)
	{
		return SOCKET_ERROR;
	}

	return sendBufferSize;
}

//////////////////////////////////////////////////////////////////////////////////////////
int SocketOption::GetRecvBufferSize() const
{
	int receiveBufferSize;
	int size = sizeof(int);
	const int result = getsockopt(socket_, SOL_SOCKET, SO_RCVBUF, reinterpret_cast<char*>(&receiveBufferSize), &size);

	if (result == SOCKET_ERROR)
	{
		return SOCKET_ERROR;
	}

	return receiveBufferSize;
}

//////////////////////////////////////////////////////////////////////////////////////////
int SocketOption::GetLingerTimeout() const
{
	LINGER linger;
	int size = sizeof(LINGER);
	ZeroMemory(&linger, sizeof(LINGER));

	const int result = getsockopt(socket_, SOL_SOCKET, SO_LINGER, reinterpret_cast<char*>(&linger), &size);

	if (result == SOCKET_ERROR)
	{
		return SOCKET_ERROR;
	}

	return linger.l_linger;
}

//////////////////////////////////////////////////////////////////////////////////////////
int SocketOption::GetMaximumSegmentSize() const
{
	int maximumSegmentSize;
	int size = sizeof(int);
	const int result = getsockopt(socket_, IPPROTO_TCP, TCP_MAXSEG, reinterpret_cast<char*>(&maximumSegmentSize), &size);

	if (result == SOCKET_ERROR)
	{
		return SOCKET_ERROR;
	}

	return maximumSegmentSize;
}

//////////////////////////////////////////////////////////////////////////////////////////
int SocketOption::GetErrorCode() const
{
	int errorCode;
	int size = sizeof(int);
	const int result = getsockopt(socket_, SOL_SOCKET, SO_ERROR, reinterpret_cast<char*>(&errorCode), &size);

	if (result == SOCKET_ERROR)
	{
		return SOCKET_ERROR;
	}

	return errorCode;
}

//////////////////////////////////////////////////////////////////////////////////////////
int SocketOption::IsNagleEnabled() const
{
	int option;
	int size = sizeof(int);
	const int result = getsockopt(socket_, IPPROTO_TCP, TCP_NODELAY, reinterpret_cast<char*>(&option), &size);

	if (result == SOCKET_ERROR)
	{
		return SOCKET_ERROR;
	}

	return option != FALSE ? FALSE : TRUE;
}

//////////////////////////////////////////////////////////////////////////////////////////
int SocketOption::IsReuseAddressEnabled() const
{
	int option;
	int size = sizeof(int);
	const int result = getsockopt(socket_, SOL_SOCKET, SO_REUSEADDR, reinterpret_cast<char*>(&option), &size);

	if (result == SOCKET_ERROR)
	{
		return SOCKET_ERROR;
	}

	return option == FALSE ? FALSE : TRUE;
}

//////////////////////////////////////////////////////////////////////////////////////////
int SocketOption::IsExclusiveReuseAddressEnabled() const
{
	int option;
	int size = sizeof(int);
	const int result = getsockopt(socket_, SOL_SOCKET, SO_EXCLUSIVEADDRUSE, reinterpret_cast<char*>(&option), &size);

	if (result == SOCKET_ERROR)
	{
		return SOCKET_ERROR;
	}

	return option == 0 ? 0 : 1;
}

//////////////////////////////////////////////////////////////////////////////////////////
int SocketOption::IsLingerEnabled() const
{
	int option;
	int size = sizeof(int);
	const int result = getsockopt(socket_, SOL_SOCKET, SO_DONTLINGER, reinterpret_cast<char*>(&option), &size);

	if (result == SOCKET_ERROR)
	{
		return SOCKET_ERROR;
	}

	return option != FALSE ? FALSE : TRUE;
}

//////////////////////////////////////////////////////////////////////////////////////////
int SocketOption::IsKeepAliveEnabled() const
{
	int option;
	int size = sizeof(int);
	const int result = getsockopt(socket_, SOL_SOCKET, SO_KEEPALIVE, reinterpret_cast<char*>(&option), &size);

	if (result == SOCKET_ERROR)
	{
		return SOCKET_ERROR;
	}

	return option != FALSE ? TRUE : FALSE;
}

NS_JNET_END
