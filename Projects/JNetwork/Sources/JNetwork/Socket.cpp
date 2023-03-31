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

	template <typename LPFN>
	static bool UseEx(LPFN ppfn, GUID& id) {
		Socketv4 dummySock = Socket::CreateTcpV4(false);

		if (!dummySock.IsValid())
			return false;

		{
			Int32UL dwBytes;
			int iResult = WSAIoctl(dummySock.Handle, SIO_GET_EXTENSION_FUNCTION_POINTER,
				&id, sizeof(id),
				ppfn, sizeof(LPFN),
				&dwBytes, NULL, NULL);

			if (iResult != 0)
				return false;
		}

		if (dummySock.Close() == SOCKET_ERROR) {
			DebugAssertMsg(false, "함수주소 획득을 위해 생성한 더미 소켓을 닫는데 실패했습니다.");
		}

		return true;
	}

	bool UseConnectEx() {

		if (lpfnConnectEx != nullptr)
			return true;

		GUID guid = WSAID_CONNECTEX;
		bool bRet = UseEx(&lpfnConnectEx, guid);

		if (bRet && lpfnConnectEx)
			return true;

		DebugAssertMsg(false, "ConnectEx 함수 주소를 얻는데 실패했습니다.");
		return false;
	}

	bool UseDisconnectEx() {

		if (lpfnDisconnectEx != nullptr)
			return true;

		GUID guid = WSAID_DISCONNECTEX;
		bool bRet = UseEx(&lpfnDisconnectEx, guid);

		if (bRet && lpfnDisconnectEx)
			return true;

		DebugAssertMsg(false, "DisconnectEx 함수 주소를 얻는데 실패했습니다.");
		return false;
	}	

	NS_DETAIL_END

/*=====================================================================================
							Socket
=======================================================================================*/

Socket::Socket()
	: State(eInitialized)
	, Handle(INVALID_SOCKET)
	, Protocol(TransportProtocol::None)
{}

Socket::Socket(TransportProtocol tpproto, SOCKET socket)
	: Handle(socket)
	, State(eInitialized)
	, Protocol(tpproto)
{}

void Socket::Invalidate() {
	Protocol = TransportProtocol::None;
	Handle = INVALID_SOCKET;
	State = eInitialized;
}

int Socket::ShutdownBoth() const {
	return shutdown(Handle, SD_BOTH);
}

int Socket::ShutdownWrite() const {
	return shutdown(Handle, SD_SEND);
}

int Socket::ShutdownRead() const {
	return shutdown(Handle, SD_RECEIVE);
}

int Socket::Close() {

	if (!IsValid())
		return 0;

	const int ret = closesocket(Handle);

	if (ret == 0) {
		Handle = INVALID_SOCKET;
	}

	return ret;
}

Socketv4 Socket::CreateV4(TransportProtocol tpproto, bool overlapped) {
	
	SOCKET hSocket = WSASocketW(
		AF_INET,
		tpproto == TransportProtocol::TCP ? SOCK_STREAM : SOCK_DGRAM,
		(int)tpproto,
		NULL,
		0,
		overlapped ? WSA_FLAG_OVERLAPPED : 0);

	return { tpproto, hSocket };
}


Socketv4 Socket::CreateTcpV4(bool overlapped) {
	SOCKET hSocket = WSASocketW(
		AF_INET,
		SOCK_STREAM,
		IPPROTO_TCP,
		NULL,
		0,
		overlapped ? WSA_FLAG_OVERLAPPED : 0);

	return { TransportProtocol::TCP, hSocket };
}

Socketv6 Socket::CreateV6(TransportProtocol tpproto, bool overlapped) {
	// UNUSED
	return {};
}

/*=====================================================================================
							Socketv4
=======================================================================================*/

int Socketv4::Bind(const IPv4EndPoint& ipv4EndPoint) {
	SOCKADDR_IN addr;
	addr.sin_family = AF_INET;
	addr.sin_port = ByteOrder::HostToNetwork(ipv4EndPoint.GetPort());
	addr.sin_addr.S_un.S_addr = ByteOrder::HostToNetwork(ipv4EndPoint.GetAddress().GetAddress());

	int iBindRet = bind(Handle, (sockaddr*)&addr, sizeof(SOCKADDR_IN));
	if (iBindRet == 0) {
		State = eBinded;
	}
	return iBindRet;
}

int Socketv4::BindAny() {
	return Bind({IPv4Address::Any(), 0});
}

int Socketv4::Listen(int connectionWaitingQueueSize) {

	int iListenRet = listen(Handle, connectionWaitingQueueSize);

	if (iListenRet == 0) {
		State = eListen;
	}

	return iListenRet;
}

Socketv4 Socketv4::Accept() {
	return { this->Protocol, accept(Handle, nullptr, nullptr) };
}

int Socketv4::AcceptEx(SOCKET listenSocket, void* outputBuffer, Int32UL receiveDatalen, Out_ Int32UL* receivedBytes, LPOVERLAPPED overlapped) const {

	// @참고 : https://docs.microsoft.com/en-us/windows/win32/api/mswsock/nf-mswsock-acceptex
	// sListenSocket : 서버 소켓
	// sAcceptSocket : 연결을 수락받을 세션 소켓
	// lpOutputBuffer : 버퍼의 오프셋 0바이트 부터 상대 클라이언트가 Connect하면서 전달한 데이터를 수신하도록 하고 그 뒤에 이어서 로컬 주소, 원격 주소 정보가 저장된다.
	//					lpOutputBuffer 크기 >= dwLocalAddressLength + dwRemoteAddressLength가 되어야한다.
	// dwReceiveDataLength : 이 사이즈는 상대 소켓이 연결하면서 전달한 정보를 담을 크기를 나타낸다. 이게 0일 경우 데이터를 받지 않는듯?
	//						 ConnectEx에 전송하는 기능이 있던데 거기서 전송한 데이터를 lpOutputBuffer여기로 받는 것으로 추측된다. 테스트는 안해봄
	//						 lpOutputBuffer의 시작 주소부터 데이터를 받고 그 이후에 로컬 주소, 원격 주소 정보가 담기는가보다.
	//					     로컬 주소, 원격 주소를 제외한 크기를 전달해줘야한다.
	// dwLocalAddressLength : 로컬 주소 정보 크기를 전달한다. 전송 프로토콜의 최대 주소 크기보다 16바이트 이상 커야한다.
	// dwRemoteAddressLength : 원격 주소 정보 크기를 전달한다. 전송 프로토콜의 최대 주소 크기보다 16바이트 이상 커야한다.
	// lpdwBytesReceived : 전송받은 데이터 크기를 저장할 Int32UL*를 전달한다. 만약 바로 완료된 경우 여기 데이터가 담길 것이다.
	//					   GetLastError()의 ERROR_IO_PENDING 오류를 받는 경우에는 완료 통지 방식으로 데이터를 읽어야한다. (오버랩 말하는 듯?)
	// lpOverlapped : NULL을 절대 전달하면 안된다. 수신한 정보가 비동기적으로 완료될 수 있으므로 오버랩 정보를 전달해야함.

	

	return ::AcceptEx(listenSocket,
		Handle, 
		outputBuffer,
		receiveDatalen,
		sizeof(SOCKADDR_IN) + 16, 
		sizeof(SOCKADDR_IN) + 16, 
		receivedBytes,
		overlapped
	);
}

void Socketv4::AcceptExResult(char* buff, Int32UL receiveDatalen, Out_ IPv4EndPoint* localEp, Out_ IPv4EndPoint* remoteEp) {

	LPSOCKADDR_IN pSockLocalAddrIn;
	LPSOCKADDR_IN pSockRemoteAddrIn;
	INT iLocalAddrLen;
	INT iRemoteAddrLen;

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
	GetAcceptExSockaddrs(buff,
		receiveDatalen,
		sizeof(SOCKADDR_IN) + 16,
		sizeof(SOCKADDR_IN) + 16,
		(sockaddr**)&pSockLocalAddrIn,
		&iLocalAddrLen,
		(sockaddr**)&pSockRemoteAddrIn,
		&iRemoteAddrLen);

	if (localEp) {
		const IPv4Address v4Address{ ByteOrder::NetworkToHost(pSockLocalAddrIn->sin_addr.S_un.S_addr) };
		const IPv4EndPoint v4EndPoint{ v4Address, ByteOrder::NetworkToHost(pSockLocalAddrIn->sin_port) };

		*localEp = v4EndPoint;
	}

	if (remoteEp) {
		const IPv4Address v4Address{ ByteOrder::NetworkToHost(pSockRemoteAddrIn->sin_addr.S_un.S_addr) };
		const IPv4EndPoint v4EndPoint{ v4Address, ByteOrder::NetworkToHost(pSockRemoteAddrIn->sin_port) };

		*remoteEp = v4EndPoint;
	}
}

int Socketv4::Connect(const IPv4EndPoint& ipv4EndPoint) const {
	SOCKADDR_IN addr;
	addr.sin_family = AF_INET;
	addr.sin_port = ByteOrder::HostToNetwork(ipv4EndPoint.GetPort());
	addr.sin_addr.S_un.S_addr = ByteOrder::HostToNetwork(ipv4EndPoint.GetAddress().GetAddress());
	return connect(Handle, (sockaddr*)&addr, sizeof(SOCKADDR_IN));
}




// Windows Vista 이전에서는 AcceptEx 함수의 주소를 얻어와서 사용해야했는데 바뀌었다고 한다.
// 그냥 써도 잘 동작한다.
// 대신 AcceptEx가 MSWSock.h와 Winsock.h 이 2개에만 있어서 사용할려면 MSWSock.h를 include하고 MSWSock.lib을 링크 해줘야함.
// @참고 : https://jacking75.github.io/cpp_iocp_extension_method/
// @AcceptEx 함수에 대한 설명 : https://programmingdiary.tistory.com/4

// @참고 : https://docs.microsoft.com/en-us/windows/win32/api/mswsock/nc-mswsock-lpfn_connectex
// ConnectEx 사용 예시 코드 : https://gist.github.com/joeyadams/4158972
// 
// [in] s :		connect()의 s와 동일
// [in] name :	connect()의 name과 동일
// [in] namelen : connect()의 namelen과 동일
// [in, opt]	lpSendBuffer : Connection이 Established 된 후에 전달한 버퍼의 데이터를 전송한다.
//								 소켓 s에 TCP_FASTOPEN이 ConnectEx() 호출전에 활성화 되어 있으면 연결 성립 중에 일부 데이터가 전송될 수 있다.
// [in]			dwSendDataLength : lpSendBuffer의 바이트 크기를 전달한다 만약 lpSendBuffer를 NULL로 전달하면 이 값은 무시된다.
// [out]		lpdwBytesSent : 함수 반환값이 TRUE 된 경우 이 연결이 성립된 후 전송된 바이트 크기를 반환해준다. lpSendBuffer를 NULL로 전달하면 이 값은 무시된다.
// [in]			lpOverlapped : 절대 NULL 전달하면 안됨

int Socketv4::ConnectEx(const IPv4EndPoint& ipv4EndPoint, LPOVERLAPPED overlapped, char* sendbuf, Int32UL sendbufSize, Out_ Int32UL* sentBytes) const {
	SOCKADDR_IN addr;
	addr.sin_family = AF_INET;
	addr.sin_port = ByteOrder::HostToNetwork(ipv4EndPoint.GetPort());
	addr.sin_addr.S_un.S_addr = ByteOrder::HostToNetwork(ipv4EndPoint.GetAddress().GetAddress());
	DebugAssertMsg(lpfnConnectEx != nullptr, "ConnectEx 함수를 사용할려면 먼저 UseConnectEx를 호출해주세요");
	DebugAssertMsg(Protocol == TransportProtocol::TCP, "커넥션 오리엔티드 소켓만 사용가능합니다.");
	return lpfnConnectEx(Handle, (sockaddr*)&addr, sizeof(SOCKADDR_IN), sendbuf, sendbufSize, sentBytes, overlapped);
}

// @참고: https://learn.microsoft.com/en-us/windows/win32/api/mswsock/nc-mswsock-lpfn_connectex
// 커넥션 오리엔티드 소켓만 사용가능
int Socketv4::DisconnectEx(LPOVERLAPPED overlapped, Int32UL flag) {
	DebugAssertMsg(lpfnDisconnectEx != nullptr, "DisconnectEx 함수를 사용할려면 먼저 UseDisconnectEx를 호출해주세요");
	DebugAssertMsg(Protocol == TransportProtocol::TCP, "커넥션 오리엔티드 소켓만 사용가능합니다.");
	return lpfnDisconnectEx(Handle, overlapped, flag, 0);
}

int Socketv4::Send(char* buff, Int32U len, Int32U flag) const {
	return send(Handle, buff, len, flag);
}

int Socketv4::SendTo(char* buff, Int32U len, const IPv4EndPoint& ipv4EndPoint, Int32U flag) const {
	SOCKADDR_IN addr;
	addr.sin_family = AF_INET;
	addr.sin_port = ByteOrder::HostToNetwork(ipv4EndPoint.GetPort());
	addr.sin_addr.S_un.S_addr = ByteOrder::HostToNetwork(ipv4EndPoint.GetAddress().GetAddress());
	return sendto(Handle, buff, len, flag, (sockaddr*)&addr, sizeof(SOCKADDR_IN));
}

int Socketv4::Receive(char* buff, Int32U buffSize, Int32U flag) const {
	return recv(Handle, buff, buffSize, flag);
}

int Socketv4::ReceiveFrom(char* buff, Int32U buffSize, Out_ IPv4EndPoint* ipv4EndPoint, Int32U flag) const {
	SOCKADDR_IN addr;
	int sz = sizeof(SOCKADDR_IN);
	const int recvBytes = recvfrom(Handle, buff, buffSize, flag, (sockaddr*)&addr, &sz);

	*ipv4EndPoint = IPv4EndPoint{ addr };
	return recvBytes;
}

int Socketv4::SendEx(
	LPWSABUF lpBuf, 
	Out_ Int32UL* pBytesSent, 
	LPOVERLAPPED lpOverlapped, 
	LPWSAOVERLAPPED_COMPLETION_ROUTINE lpCompRoutine, 
	Int32U flag) const
{
	return WSASend(Handle, lpBuf, 1, pBytesSent, flag, lpOverlapped, lpCompRoutine);
}

int Socketv4::SendToEx(
	LPWSABUF lpBuffers, 
	Out_ Int32UL* pBytesSent, 
	LPOVERLAPPED lpOverlapped, 
	const IPv4EndPoint& to,
	LPWSAOVERLAPPED_COMPLETION_ROUTINE lpCompRoutine) const
{
	SOCKADDR_IN addr;
	addr.sin_family = AF_INET;
	addr.sin_port = ByteOrder::HostToNetwork(to.GetPort());
	addr.sin_addr.S_un.S_addr = ByteOrder::HostToNetwork(to.GetAddress().GetAddress());
	return WSASendTo(Handle, lpBuffers, 1, pBytesSent, 0, (sockaddr*)&addr, sizeof(SOCKADDR_IN), lpOverlapped, lpCompRoutine);
}

int Socketv4::ReceiveEx(
	LPWSABUF lpBuf, 
	Out_ Int32UL* pBytesReceived, 
	LPOVERLAPPED lpOverlapped, 
	LPWSAOVERLAPPED_COMPLETION_ROUTINE lpCompRoutine, 
	Int32U flag) const
{
	return WSARecv(Handle, lpBuf, 1, pBytesReceived, (Int32UL*)&flag, lpOverlapped, lpCompRoutine);
}

int Socketv4::ReceiveFromEx(
	LPWSABUF lpBuf, 
	Out_ Int32UL* pBytesReceived,
	LPOVERLAPPED lpOverlapped, 
	Out_ SOCKADDR_IN* senderAddr,
	LPWSAOVERLAPPED_COMPLETION_ROUTINE lpCompRoutine, 
	Int32U flag) const
{
	int iAddrLen = sizeof(SOCKADDR_IN);
	return WSARecvFrom(Handle, lpBuf, 1, pBytesReceived, (Int32UL*)&flag, (sockaddr*)senderAddr, &iAddrLen, lpOverlapped, lpCompRoutine);
}

IPv4EndPoint Socketv4::GetLocalEndPoint() const {
	SOCKADDR_IN addr;
	int size = sizeof(SOCKADDR_IN);
	ZeroMemory(&addr, sizeof(SOCKADDR_IN));

	if (getsockname(Handle, (SOCKADDR*)&addr, &size) != SOCKET_ERROR) {
		return IPv4EndPoint{ addr };
	}

	return { IPv4Address{}, 0 };
}

IPv4EndPoint Socketv4::GetRemoteEndPoint() const {
	SOCKADDR_IN addr;
	int size = sizeof(SOCKADDR_IN);
	ZeroMemory(&addr, sizeof(SOCKADDR_IN));

	if (getpeername(Handle, (SOCKADDR*)&addr, &size) != SOCKET_ERROR) {
		return IPv4EndPoint{ addr };
	}

	return { IPv4Address{}, 0 };
}

/*=====================================================================================
							Socketv6
=======================================================================================*/



/*=====================================================================================
							SocketOption
=======================================================================================*/

int SocketOption::SetBroadcastEnabled(bool enabled) const {
	BOOL op = enabled ? TRUE : FALSE;
	return setsockopt(m_Socket, SOL_SOCKET, SO_SNDBUF, (char*)&op, sizeof(BOOL));
}

int SocketOption::SetSendBufferSize(int size) const {
	return setsockopt(m_Socket, SOL_SOCKET, SO_SNDBUF, (char*)&size, sizeof(int));
}

int SocketOption::SetRecvBufferSize(int size) const {
	return setsockopt(m_Socket, SOL_SOCKET, SO_RCVBUF, (char*)&size, sizeof(int));
}

int SocketOption::SetNonBlockingEnabled(bool enabled) const {
	unsigned long mode = enabled ? 1 : 0;
	return ioctlsocket(m_Socket, FIONBIO, &mode);
}

int SocketOption::SetNagleEnabled(bool enabled) const {
	BOOL nodelayOp = enabled ? 0 : 1;
	return setsockopt(m_Socket, IPPROTO_TCP, TCP_NODELAY, (char*)&nodelayOp, sizeof(BOOL));
}

int SocketOption::SetLingerEnabled(bool enabled) const {
	BOOL op = enabled ? FALSE : TRUE;
	return setsockopt(m_Socket, SOL_SOCKET, SO_DONTLINGER, (char*)&op, sizeof(BOOL));
}

int SocketOption::SetReuseAddrEnabled(bool enabled) const {
	BOOL op = enabled ? TRUE : FALSE;
	return setsockopt(m_Socket, SOL_SOCKET, SO_REUSEADDR, (char*)&op, sizeof(BOOL));
}

int SocketOption::SetExclusiveReuseAddrEnabled(bool enabled) const {
	BOOL op = enabled ? TRUE : FALSE;
	return setsockopt(m_Socket, SOL_SOCKET, SO_EXCLUSIVEADDRUSE, (char*)&op, sizeof(BOOL));
}

int SocketOption::SetLingerTimeout(Int16U timeout) const {
	LINGER linger;
	linger.l_onoff = TRUE;
	linger.l_linger = timeout;
	return setsockopt(m_Socket, SOL_SOCKET, SO_LINGER, (char*)&linger, sizeof(LINGER));
}

int SocketOption::SetKeepAliveEnabled(bool enabled) const {
	BOOL op = enabled ? TRUE : FALSE;
	return setsockopt(m_Socket, SOL_SOCKET, SO_KEEPALIVE, (char*)&op, sizeof(BOOL));
}

int SocketOption::SetUpdateAcceptContext(SOCKET hListeningSocket) const {
	return setsockopt(m_Socket, SOL_SOCKET, SO_UPDATE_ACCEPT_CONTEXT, (char*)&hListeningSocket, sizeof(SOCKET));
}

int SocketOption::GetSendBufferSize() const {
	int snd_size;
	int sz = sizeof(int);
	const int ret = getsockopt(m_Socket, SOL_SOCKET, SO_SNDBUF, (char*)&snd_size, &sz);

	if (ret == SOCKET_ERROR) {
		return SOCKET_ERROR;
	}

	return snd_size;
}

int SocketOption::GetRecvBufferSize() const {
	int recv_size;
	int sz = sizeof(int);
	const int ret = getsockopt(m_Socket, SOL_SOCKET, SO_RCVBUF, (char*)&recv_size, &sz);

	if (ret == SOCKET_ERROR) {
		return SOCKET_ERROR;
	}

	return recv_size;
}

int SocketOption::GetLingerTimeout() const {
	LINGER linger;
	int sz = sizeof(LINGER);
	ZeroMemory(&linger, sizeof(LINGER));

	const int ret = getsockopt(m_Socket, SOL_SOCKET, SO_LINGER, (char*)&linger, &sz);

	if (ret == SOCKET_ERROR) {
		return SOCKET_ERROR;
	}

	return linger.l_linger;
}

int SocketOption::GetMaximumSegmentSize() const {
	int mss;
	int sz = sizeof(int);
	const int ret = getsockopt(m_Socket, IPPROTO_TCP, TCP_MAXSEG, (char*)&mss, &sz);

	if (ret == SOCKET_ERROR) {
		return SOCKET_ERROR;
	}

	return mss;
}

int SocketOption::GetErrorCode() const {
	int errorCode;
	int sz = sizeof(int);
	const int ret = getsockopt(m_Socket, SOL_SOCKET, SO_ERROR, (char*)&errorCode, &sz);

	if (ret == SOCKET_ERROR) {
		return SOCKET_ERROR;
	}

	return errorCode;
}

int SocketOption::IsNagleEnabled() const {
	int op;
	int sz = sizeof(int);
	const int ret = getsockopt(m_Socket, IPPROTO_TCP, TCP_NODELAY, (char*)&op, &sz);

	if (ret == SOCKET_ERROR)
		return SOCKET_ERROR;

	return op != FALSE ? FALSE : TRUE;
}

int SocketOption::IsReuseAddressEnabled() const {
	int op;
	int sz = sizeof(int);
	const int ret = getsockopt(m_Socket, SOL_SOCKET, SO_REUSEADDR, (char*)&op, &sz);

	if (ret == SOCKET_ERROR)
		return SOCKET_ERROR;

	return op == FALSE ? FALSE : TRUE;
}

int SocketOption::IsExclusiveReuseAddressEnabled() const {
	int op;
	int sz = sizeof(int);
	const int ret = getsockopt(m_Socket, SOL_SOCKET, SO_EXCLUSIVEADDRUSE, (char*)&op, &sz);

	if (ret == SOCKET_ERROR)
		return SOCKET_ERROR;

	return op == 0 ? 0 : 1;
}

int SocketOption::IsLingerEnabled() const {
	int op;
	int sz = sizeof(int);
	const int ret = getsockopt(m_Socket, SOL_SOCKET, SO_DONTLINGER, (char*)&op, &sz);

	if (ret == SOCKET_ERROR)
		return SOCKET_ERROR;

	return op != FALSE ? FALSE : TRUE;
}

int SocketOption::IsKeepAliveEnabled() const {
	int op;
	int sz = sizeof(int);
	const int ret = getsockopt(m_Socket, SOL_SOCKET, SO_KEEPALIVE, (char*)&op, &sz);

	if (ret == SOCKET_ERROR)
		return SOCKET_ERROR;

	return op != FALSE ? TRUE : FALSE;
}


NS_JNET_END
