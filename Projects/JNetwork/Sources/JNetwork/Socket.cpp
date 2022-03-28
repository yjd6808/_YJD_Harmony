#include <JNetwork/Network.h>
#include <JNetwork/Socket.h>
#include <JNetwork/ByteOrder.h>

#include <MSWSock.h>

namespace JNetwork {

	namespace Winsock {

		// Windows Vista ���������� AcceptEx �Լ��� �ּҸ� ���ͼ� ����ؾ��ߴµ� �ٲ���ٰ� �Ѵ�.
		// �׳� �ᵵ �� �����Ѵ�.
		// ��� AcceptEx�� MSWSock.h�� Winsock.h �� 2������ �־ ����ҷ��� MSWSock.h�� include�ϰ� MSWSock.lib�� ��ũ �������.
		// @���� : https://jacking75.github.io/cpp_iocp_extension_method/
		// @AcceptEx �Լ��� ���� ���� : https://programmingdiary.tistory.com/4
		
		// �����Լ� ������ ������ ��
		int AcceptEx_(SOCKET sListenSocket,
			SOCKET sAcceptSocket,
			PVOID lpOutputBuffer,
			DWORD dwReceiveDataLength,
			DWORD dwLocalAddressLength,
			DWORD dwRemoteAddressLength,
			LPDWORD lpdwBytesReceived,
			LPOVERLAPPED lpOverlapped) {
			
			return AcceptEx(sListenSocket,
				sAcceptSocket, 
				lpOutputBuffer, 
				dwReceiveDataLength, 
				dwLocalAddressLength, 
				dwRemoteAddressLength,
				lpdwBytesReceived, 
				lpOverlapped);
		}

		
		int ConnectEx_(SOCKET s, 
			sockaddr* pConnectAddr, 
			int ConnectAddrSize,
			PVOID lpSendBuffer, 
			DWORD dwSendDataLength,
			LPDWORD lpdwBytesSent, 
			LPOVERLAPPED lpOverlapped) {

			static LPFN_CONNECTEX lpfnConnectEx = nullptr;

			
			if (lpfnConnectEx == nullptr) {
				// WSAIoctl �Լ� ����� ���� ���� ���� ����
				TcpSocketv4 dummySock = Socket::CreateTcpV4(false);
				int iResult;
				
				if (!dummySock.IsValid())
					return FALSE;

				{
					DWORD dwBytes;
					GUID guid = WSAID_CONNECTEX;
					iResult = WSAIoctl(dummySock.Handle(), SIO_GET_EXTENSION_FUNCTION_POINTER,
						&guid, sizeof(guid),
						&lpfnConnectEx, sizeof(lpfnConnectEx),
						&dwBytes, NULL, NULL);

					if (iResult != 0)
						return FALSE;
				}

				iResult = dummySock.Close();
				if (iResult != 0)
					return FALSE;

			}

			// @���� : https://docs.microsoft.com/en-us/windows/win32/api/mswsock/nc-mswsock-lpfn_connectex
			// ConnectEx ��� ���� �ڵ� : https://gist.github.com/joeyadams/4158972
			// 
			// [in] s :		connect()�� s�� ����
			// [in] name :	connect()�� name�� ����
			// [in] namelen : connect()�� namelen�� ����
			// [in, opt]	lpSendBuffer : Connection�� Established �� �Ŀ� ������ ������ �����͸� �����Ѵ�.
			//								 ���� s�� TCP_FASTOPEN�� ConnectEx() ȣ������ Ȱ��ȭ �Ǿ� ������ ���� ���� �߿� �Ϻ� �����Ͱ� ���۵� �� �ִ�.
			// [in]			dwSendDataLength : lpSendBuffer�� ����Ʈ ũ�⸦ �����Ѵ� ���� lpSendBuffer�� NULL�� �����ϸ� �� ���� ���õȴ�.
			// [out]		lpdwBytesSent : �Լ� ��ȯ���� TRUE �� ��� �� ������ ������ �� ���۵� ����Ʈ ũ�⸦ ��ȯ���ش�. lpSendBuffer�� NULL�� �����ϸ� �� ���� ���õȴ�.
			// [in]			lpOverlapped : ���� NULL �����ϸ� �ȵ�


			return lpfnConnectEx(
				s,
				pConnectAddr,
				ConnectAddrSize,
				lpSendBuffer,
				dwSendDataLength,
				lpdwBytesSent,
				lpOverlapped);
		}
	}

	/*=====================================================================================
								Socket
	=======================================================================================*/

	int Socket::ShutdownBoth() const {
		return shutdown(m_Socket, SD_BOTH);
	}

	int Socket::ShutdownWrite() const {
		return shutdown(m_Socket, SD_SEND);
	}

	int Socket::ShutdownRead() const {
		return shutdown(m_Socket, SD_RECEIVE);
	}

	int Socket::Close() {
		int ret = closesocket(m_Socket);

		if (ret == 0) {
			m_Socket = INVALID_SOCKET;
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


	TcpSocketv4 Socket::CreateTcpV4(bool overlapped) {
		SOCKET hSocket = WSASocketW(
			AF_INET,
			SOCK_STREAM,
			IPPROTO_TCP,
			NULL,
			0,
			overlapped ? WSA_FLAG_OVERLAPPED : 0);

		return { hSocket };
	}

	Socketv6 Socket::CreateV6(TransportProtocol tpproto, bool overlapped) {
		// UNUSED
		return Socketv6();
	}

	/*=====================================================================================
								Socketv4
	=======================================================================================*/

	int Socketv4::Bind(const IPv4EndPoint& ipv4EndPoint) {
		SOCKADDR_IN addr;
		addr.sin_family = AF_INET;
		addr.sin_port = ByteOrder::HostToNetwork(ipv4EndPoint.GetPort());
		addr.sin_addr.S_un.S_addr = ByteOrder::HostToNetwork(ipv4EndPoint.GetAddress().GetAddress());
		return bind(m_Socket, (sockaddr*)&addr, sizeof(SOCKADDR_IN));
	}

	int Socketv4::BindAny() {
		return Bind({IPv4Address::Any(), 0});
	}

	int Socketv4::Listen(int connectionWaitingQueueSize) {
		
		return listen(m_Socket, connectionWaitingQueueSize);
	}

	Socketv4 Socketv4::Accept() {
		return { this->m_TransportProtocol, accept(m_Socket, nullptr, nullptr) };
	}

	int Socketv4::AcceptEx(SOCKET listenSocket, void* outputBuffer, DWORD receiveDatalen, Out_ LPDWORD receivedBytes, LPOVERLAPPED overlapped) {

		// @���� : https://docs.microsoft.com/en-us/windows/win32/api/mswsock/nf-mswsock-acceptex
		// sListenSocket : ���� ����
		// sAcceptSocket : ������ �������� ���� ����
		// lpOutputBuffer : ������ ������ 0����Ʈ ���� ��� Ŭ���̾�Ʈ�� Connect�ϸ鼭 ������ �����͸� �����ϵ��� �ϰ� �� �ڿ� �̾ ���� �ּ�, ���� �ּ� ������ ����ȴ�.
		//					lpOutputBuffer ũ�� >= dwLocalAddressLength + dwRemoteAddressLength�� �Ǿ���Ѵ�.
		// dwReceiveDataLength : �� ������� ��� ������ �����ϸ鼭 ������ ������ ���� ũ�⸦ ��Ÿ����. �̰� 0�� ��� �����͸� ���� �ʴµ�?
		//						 ConnectEx�� �����ϴ� ����� �ִ��� �ű⼭ ������ �����͸� lpOutputBuffer����� �޴� ������ �����ȴ�. �׽�Ʈ�� ���غ�
		//						 lpOutputBuffer�� ���� �ּҺ��� �����͸� �ް� �� ���Ŀ� ���� �ּ�, ���� �ּ� ������ ���°�����.
		//					     ���� �ּ�, ���� �ּҸ� ������ ũ�⸦ ����������Ѵ�.
		// dwLocalAddressLength : ���� �ּ� ���� ũ�⸦ �����Ѵ�. ���� ���������� �ִ� �ּ� ũ�⺸�� 16����Ʈ �̻� Ŀ���Ѵ�.
		// dwRemoteAddressLength : ���� �ּ� ���� ũ�⸦ �����Ѵ�. ���� ���������� �ִ� �ּ� ũ�⺸�� 16����Ʈ �̻� Ŀ���Ѵ�.
		// lpdwBytesReceived : ���۹��� ������ ũ�⸦ ������ LPDWORD�� �����Ѵ�. ���� �ٷ� �Ϸ�� ��� ���� �����Ͱ� ��� ���̴�.
		//					   GetLastError()�� ERROR_IO_PENDING ������ �޴� ��쿡�� �Ϸ� ���� ������� �����͸� �о���Ѵ�. (������ ���ϴ� ��?)
		// lpOverlapped : NULL�� ���� �����ϸ� �ȵȴ�. ������ ������ �񵿱������� �Ϸ�� �� �����Ƿ� ������ ������ �����ؾ���.
		
		return Winsock::AcceptEx_(listenSocket,
			m_Socket, 
			outputBuffer,
			receiveDatalen,
			sizeof(SOCKADDR_IN) + 16, 
			sizeof(SOCKADDR_IN) + 16, 
			receivedBytes,
			overlapped
		);
	}

	void Socketv4::AcceptExResult(char* buff, DWORD receiveDatalen, Out_ IPv4EndPoint* localEp, Out_ IPv4EndPoint* remoteEp) {

		LPSOCKADDR_IN pSockLocalAddrIn;
		LPSOCKADDR_IN pSockRemoteAddrIn;
		INT iLocalAddrLen;
		INT iRemoteAddrLen;

		// @���� : https://docs.microsoft.com/en-us/windows/win32/api/mswsock/nf-mswsock-getacceptexsockaddrs
		// AcceptEx �Լ� ȣ������ ����� ������ ���� �ּҿ� ����Ʈ �ּҸ� ���� �� �ֵ��� �Ѵ�.
		// lpOutputBuffer : AcceptEx ���� ȣ��ܰ��� �����Ͱ� ��� ù ���� �ּҰ�, AcceptEx���� ������ lpOutputBuffer�� ���� ���� �־��ش�.
		// dwReceiveDataLength : lpOutputBuffer���� ������ �������� ���̸� ��Ÿ����.  AcceptEx���� ������ dwReceiveDataLength�� ���� ���� �־��ش�.
		// dwLocalAddressLength : ���� �ּ� ��������� ����� ����Ʈ ���� �����Ѵ�. AcceptEx���� ������ dwLocalAddressLength�� ���� ���� �־��ش�.
		// dwRemoteAddressLength : ���� �ּ� ��������� ����� ����Ʈ ���� �����Ѵ�. AcceptEx���� ������ dwRemoteAddressLength�� ���� ���� �־��ش�.
		// LocalSockaddr : ���� �ּ� ������ ��� ����ü �����͸� �����Ѵ�.
		// LocalSockaddrLength : ���� �ּ� ������ ũ�⸦ ��ȯ�޴´�.
		// RemoteSockaddr : ����� ������ ���� �ּ� ������ ������ ����ü �����͸� �����Ѵ�.
		// RemoteSockaddrLength : ���� �ּ� ������ ũ�⸦ ��ȯ�޴´�.
		GetAcceptExSockaddrs(buff,
			receiveDatalen,
			sizeof(SOCKADDR_IN) + 16,
			sizeof(SOCKADDR_IN) + 16,
			(sockaddr**)&pSockLocalAddrIn,
			&iLocalAddrLen,
			(sockaddr**)&pSockRemoteAddrIn,
			&iRemoteAddrLen);

		if (localEp) {
			IPv4Address v4Address{ ByteOrder::NetworkToHost(pSockLocalAddrIn->sin_addr.S_un.S_addr) };
			IPv4EndPoint v4EndPoint{ v4Address, ByteOrder::NetworkToHost(pSockLocalAddrIn->sin_port) };

			*localEp = v4EndPoint;
		}

		if (remoteEp) {
			IPv4Address v4Address{ ByteOrder::NetworkToHost(pSockRemoteAddrIn->sin_addr.S_un.S_addr) };
			IPv4EndPoint v4EndPoint{ v4Address, ByteOrder::NetworkToHost(pSockRemoteAddrIn->sin_port) };

			*remoteEp = v4EndPoint;
		}
	}

	int Socketv4::Connect(const IPv4EndPoint& ipv4EndPoint) {
		SOCKADDR_IN addr;
		addr.sin_family = AF_INET;
		addr.sin_port = ByteOrder::HostToNetwork(ipv4EndPoint.GetPort());
		addr.sin_addr.S_un.S_addr = ByteOrder::HostToNetwork(ipv4EndPoint.GetAddress().GetAddress());
		return connect(m_Socket, (sockaddr*)&addr, sizeof(SOCKADDR_IN));
	}

	int Socketv4::ConnectEx(const IPv4EndPoint& ipv4EndPoint, LPOVERLAPPED overlapped, char* sendbuf, DWORD sendbufSize, Out_ LPDWORD sentBytes) {
		SOCKADDR_IN addr;
		addr.sin_family = AF_INET;
		addr.sin_port = ByteOrder::HostToNetwork(ipv4EndPoint.GetPort());
		addr.sin_addr.S_un.S_addr = ByteOrder::HostToNetwork(ipv4EndPoint.GetAddress().GetAddress());
		return Winsock::ConnectEx_(m_Socket, (sockaddr*)&addr, sizeof(SOCKADDR_IN), sendbuf, sendbufSize, sentBytes, overlapped);
	}

	int Socketv4::Send(char* buff, Int32U len, Int32U flag) {
		return send(m_Socket, buff, len, flag);
	}

	int Socketv4::SendTo(char* buff, Int32U len, const IPv4EndPoint& ipv4EndPoint, Int32U flag) {
		SOCKADDR_IN addr;
		addr.sin_family = AF_INET;
		addr.sin_port = ByteOrder::HostToNetwork(ipv4EndPoint.GetPort());
		addr.sin_addr.S_un.S_addr = ByteOrder::HostToNetwork(ipv4EndPoint.GetAddress().GetAddress());
		return sendto(m_Socket, buff, len, flag, (sockaddr*)&addr, sizeof(SOCKADDR_IN));
	}

	int Socketv4::Receive(char* buff, Int32U buffSize, Int32U flag) {
		return recv(m_Socket, buff, buffSize, flag);
	}

	int Socketv4::ReceiveFrom(char* buff, Int32U buffSize, Out_ IPv4EndPoint* ipv4EndPoint, Int32U flag) {
		SOCKADDR_IN addr;
		int sz = sizeof(SOCKADDR_IN);
		int recvBytes = recvfrom(m_Socket, buff, buffSize, flag, (sockaddr*)&addr, &sz);
		
		IPv4Address v4Address { ByteOrder::NetworkToHost(addr.sin_addr.S_un.S_addr) };
		IPv4EndPoint v4EndPoint{ v4Address, ByteOrder::NetworkToHost(addr.sin_port) };

		*ipv4EndPoint = v4EndPoint;
		return recvBytes;
	}

	int Socketv4::SendEx(LPWSABUF lpBuf, Out_ Int32UL* pBytesSent, LPOVERLAPPED lpOverlapped, LPWSAOVERLAPPED_COMPLETION_ROUTINE lpCompRoutine, Int32U flag) {
		return WSASend(m_Socket, lpBuf, 1, pBytesSent, flag, lpOverlapped, lpCompRoutine);
	}

	int Socketv4::ReceiveEx(LPWSABUF lpBuf, Out_ Int32UL* pBytesReceived, LPOVERLAPPED lpOverlapped, LPWSAOVERLAPPED_COMPLETION_ROUTINE lpCompRoutine, Int32U flag) {
		return WSARecv(m_Socket, lpBuf, 1, pBytesReceived, (LPDWORD)&flag, lpOverlapped, lpCompRoutine);
	}

	IPv4EndPoint Socketv4::GetLocalEndPoint() const {

		SOCKADDR_IN addr;
		int size = sizeof(SOCKADDR_IN);
		ZeroMemory(&addr, sizeof(SOCKADDR_IN));

		if (getsockname(m_Socket, (SOCKADDR*)&addr, &size) != SOCKET_ERROR) {
			IPv4Address v4Address{ ByteOrder::NetworkToHost(addr.sin_addr.S_un.S_addr) };
			IPv4EndPoint v4EndPoint{ v4Address, ByteOrder::NetworkToHost(addr.sin_port) };

			return v4EndPoint;
		}

		return { INVALID_SOCKET, 0 };
	}

	IPv4EndPoint Socketv4::GetRemoteEndPoint() const {
		SOCKADDR_IN addr;
		int size = sizeof(SOCKADDR_IN);
		ZeroMemory(&addr, sizeof(SOCKADDR_IN));

		if (getpeername(m_Socket, (SOCKADDR*)&addr, &size) != SOCKET_ERROR) {
			IPv4Address v4Address{ ByteOrder::NetworkToHost(addr.sin_addr.S_un.S_addr) };
			IPv4EndPoint v4EndPoint{ v4Address, ByteOrder::NetworkToHost(addr.sin_port) };

			return v4EndPoint;
		}

		return { INVALID_SOCKET, 0 };
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

	int SocketOption::SetLingerTimeout(int timeout) const {
		LINGER linger;
		int lingerSize = sizeof(LINGER);
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
		int ret = getsockopt(m_Socket, SOL_SOCKET, SO_SNDBUF, (char*)&snd_size, &sz);

		if (ret == SOCKET_ERROR) {
			return SOCKET_ERROR;
		}

		return snd_size;
	}

	int SocketOption::GetRecvBufferSize() const {
		int recv_size;
		int sz = sizeof(int);
		int ret = getsockopt(m_Socket, SOL_SOCKET, SO_RCVBUF, (char*)&recv_size, &sz);

		if (ret == SOCKET_ERROR) {
			return SOCKET_ERROR;
		}

		return recv_size;
	}

	int SocketOption::GetLingerTimeout() const {
		LINGER linger;
		int sz = sizeof(LINGER);
		ZeroMemory(&linger, sizeof(LINGER));

		int ret = getsockopt(m_Socket, SOL_SOCKET, SO_LINGER, (char*)&linger, &sz);

		if (ret == SOCKET_ERROR) {
			return SOCKET_ERROR;
		}

		return linger.l_linger;
	}

	int SocketOption::GetMaximumSegmentSize() const {
		int mss;
		int sz = sizeof(int);
		int ret = getsockopt(m_Socket, IPPROTO_TCP, TCP_MAXSEG, (char*)&mss, &sz);

		if (ret == SOCKET_ERROR) {
			return SOCKET_ERROR;
		}

		return mss;
	}

	int SocketOption::IsNagleEnabled() const {
		int op;
		int sz = sizeof(int);
		int ret = getsockopt(m_Socket, IPPROTO_TCP, TCP_NODELAY, (char*)&op, &sz);

		if (ret == SOCKET_ERROR)
			return SOCKET_ERROR;

		return op != FALSE ? FALSE : TRUE;
	}

	int SocketOption::IsReuseAddressEnabled() const {
		int op;
		int sz = sizeof(int);
		int ret = getsockopt(m_Socket, SOL_SOCKET, SO_REUSEADDR, (char*)&op, &sz);

		if (ret == SOCKET_ERROR)
			return SOCKET_ERROR;

		return op == FALSE ? FALSE : TRUE;
	}

	int SocketOption::IsExclusiveReuseAddressEnabled() const {
		int op;
		int sz = sizeof(int);
		int ret = getsockopt(m_Socket, SOL_SOCKET, SO_EXCLUSIVEADDRUSE, (char*)&op, &sz);

		if (ret == SOCKET_ERROR)
			return SOCKET_ERROR;

		return op == 0 ? 0 : 1;
	}

	int SocketOption::IsLingerEnabled() const {
		int op;
		int sz = sizeof(int);
		int ret = getsockopt(m_Socket, SOL_SOCKET, SO_DONTLINGER, (char*)&op, &sz);

		if (ret == SOCKET_ERROR)
			return SOCKET_ERROR;

		return op != FALSE ? FALSE : TRUE;
	}

	int SocketOption::IsKeepAliveEnabled() const {
		int op;
		int sz = sizeof(int);
		int ret = getsockopt(m_Socket, SOL_SOCKET, SO_KEEPALIVE, (char*)&op, &sz);

		if (ret == SOCKET_ERROR)
			return SOCKET_ERROR;

		return op != FALSE ? TRUE : FALSE;
	}

} // namespace JNetwork