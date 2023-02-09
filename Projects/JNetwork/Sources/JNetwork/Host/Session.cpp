/*
 * 작성자 : 윤정도
 */

#include <JNetwork/Network.h>
#include <JNetwork/Winsock.h>

#include <JNetwork/Packet/RecvPacket.h>
#include <JNetwork/Packet/SendPacket.h>

#include <JNetwork/Host/Session.h>
#include <JNetwork/Buffer/StaticBuffer.h>

#include <JNetwork/IOCPOverlapped/IOCPOverlappedRecv.h>
#include <JNetwork/IOCPOverlapped/IOCPOverlappedSend.h>



using namespace JCore;

NS_JNET_BEGIN

Session::Session(const IOCPPtr& iocp, int recvBufferSize, int sendBufferSize)
	: Host(iocp)
	, m_pRecvBuffer(MakeShared<StaticBuffer<6000>>())
	, m_pSendBuffer(MakeShared<StaticBuffer<6000>>()) {
}

Session::~Session() = default;

void Session::Initialize() {

	DebugAssertMsg(m_eState == eNone || m_eState == eDisconnected , "초기화 되지 않았거나 혹은 연결이 끊긴 대상만 초기화를 진행할 수 있습니다.");

	m_LocalEndPoint = {};
	m_RemoteEndPoint = {};

	m_pRecvBuffer->MoveReadPos(0);
	m_pRecvBuffer->MoveWritePos(0);

	m_pSendBuffer->MoveReadPos(0);
	m_pSendBuffer->MoveWritePos(0);

	m_eState = eInitailized;
}

bool Session::Bind(const IPv4EndPoint& bindAddr) {
	DebugAssertMsg(m_Socket.IsValid(), "유효하지 않은 소켓입니다.");

	if (m_Socket.Bind(bindAddr) == SOCKET_ERROR) {
		DebugAssertMsg(false, "소켓 바인드 실패 (%u)", Winsock::LastError());
		return false;
	}

	m_LocalEndPoint = bindAddr;
	return true;
}

bool Session::Connect(const IPv4EndPoint& remoteAddr) {

	DebugAssertMsg(m_Socket.IsValid(), "연결에 실패했습니다. INVALID_SOCKET 입니다.");

	if (m_Socket.Connect(remoteAddr) == SOCKET_ERROR) {
		DebugAssertMsg(false, "연결에 실패했습니다. (%u)", Winsock::LastError());
		return false;
	}
	m_RemoteEndPoint = remoteAddr;
	return true;
}

bool Session::Disconnect() {

	if (m_eState == eDisconnected)
		return true;

	m_eState = eDisconnected;
	m_bIocpConneced = false;

	// 굳이 오류 처리를 할 필요가 있나
	// WSAENOTSOCK: 소켓 할당이 안된 
	// WSAENOTCONN: 연결안된 소켓

	m_Socket.ShutdownBoth();
	m_Socket.Close();
	m_Socket.Invalidate();

	

	Disconnected();

	return true;
}


bool Session::SendAsync(ISendPacket* packet) {

	packet->AddRef();
	WSABUF buf = packet->GetWSABuf();
	Int32UL uiSendBytes = 0;
	IOCPOverlapped* pOverlapped = new IOCPOverlappedSend(this, m_spIocp.GetPtr(), packet);

	const int iResult = m_Socket.SendEx(&buf, &uiSendBytes, pOverlapped);
	if (iResult == SOCKET_ERROR) {
		Int32U uiErrorCode = Winsock::LastError();
		if (uiErrorCode != WSA_IO_PENDING) {
			DebugAssertMsg(false, "SendAsync() 실패 (%u)", uiErrorCode);
			pOverlapped->Release();
			packet->Release();
			return false;
		} 
	}
	
	return true;
}

bool Session::RecvAsync() {

	WSABUF buf = m_pRecvBuffer->GetRemainBuffer();
	Int32UL uiReceivedBytes = 0;
	IOCPOverlapped* pOverlapped = new IOCPOverlappedRecv(this, m_spIocp.GetPtr());

	const int iResult = m_Socket.ReceiveEx(&buf, &uiReceivedBytes, pOverlapped);
	if (iResult == SOCKET_ERROR) {
		Int32U uiErrorCode = Winsock::LastError();
		if (uiErrorCode != WSA_IO_PENDING) {
			DebugAssertMsg(false, "RecvAsync() 실패 (%u)", uiErrorCode);
			return false;
		}
	}
	return true;
}



void Session::Received(Int32UL receivedBytes) {
	m_pRecvBuffer->MoveWritePos(receivedBytes);

	
	for (;;) {
		// 패킷의 헤더 크기만큼 데이터를 수신하지 않았으면 모일때까지 기달
		if (m_pRecvBuffer->GetReadableBufferSize() < PACKET_HEADER_SIZE)
			return;

		// 패킷 헤더 길이 + 패킷 길이 만큼 수신하지 않았으면 다시 모일때까지 기다린다.
		const IRecvPacket* packet = m_pRecvBuffer->Peek<IRecvPacket*>();
		if (m_pRecvBuffer->GetReadableBufferSize() < (PACKET_HEADER_SIZE + packet->GetPacketLength())) {
			return;
		}

		m_pRecvBuffer->MoveReadPos(PACKET_HEADER_SIZE);

		for (int i = 0; i < packet->GetCommandCount(); i++) {
			ICommand* pCmd = m_pRecvBuffer->Peek<ICommand*>();

			NotifyCommand(pCmd);

			if (m_pRecvBuffer->MoveReadPos(pCmd->GetCommandLen()) == false) {
				DebugAssertMsg(false, "커맨드 크기가 이상합니다.");
				m_pRecvBuffer->Clear();
				return;
			}
		}

		if (m_pRecvBuffer->GetReadPos() == m_pRecvBuffer->GetWritePos()) {
			// 만약 수신한 데이터를 모두 읽었으면 포지션을 그냥 0으로 옮긴다.
			m_pRecvBuffer->Clear();
		} else {
			// 읽은 위치만큼은 이제 다시 쓰일일이 없으므로 버퍼를 앞으로 당긴다. 
			// WritePos 이후로 데이터를 쌓을 수 있도록하기 위해
			m_pRecvBuffer->Pop(m_pRecvBuffer->GetReadPos(), true);
		}
	}
}

void Session::WaitForZeroPending() {
	while (true) {

		int iPending = m_iOveralappedPendingCount;

		if (iPending == 0)
			break;

		DebugAssertMsg(iPending >= 0, "멍미 펜딩 카운트가 움수 인뎁쇼 (%d)", iPending);
		JCore::Thread::Sleep(1);
	}
}

NS_JNET_END 