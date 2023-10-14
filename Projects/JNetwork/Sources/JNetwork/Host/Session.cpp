/*
 * 작성자 : 윤정도
 */

#include <JNetwork/Network.h>
#include <JNetwork/Winsock.h>

#include <JNetwork/Host/Session.h>
#include <JNetwork/Buffer/CommandBuffer.h>
#include <JNetwork/Packet/RecvPacket.h>

#include <JNetwork/IOCPOverlapped/IOCPOverlappedRecv.h>
#include <JNetwork/IOCPOverlapped/IOCPOverlappedSend.h>
#include <JNetwork/IOCPOverlapped/IOCPOverlappedSendTo.h>
#include <JNetwork/IOCPOverlapped/IOCPOverlappedRecvFrom.h>

#include <JCore/Primitives/RefCountObjectPtr.h>

USING_NS_JC;

NS_JNET_BEGIN

Session::Session(
	const IOCPPtr& iocp, 
	const MemoryPoolAbstractPtr& bufferAllocator, 
	int recvBufferSize, 
	int sendBufferSize
)
	: Host(iocp)
	, m_iHandle(-1)
	, m_spBufferAllocator(bufferAllocator)
	, m_spSendBuffer(MakeShared<CommandBuffer>(m_spBufferAllocator, sendBufferSize))
	, m_spRecvBuffer(MakeShared<CommandBuffer>(m_spBufferAllocator, recvBufferSize)) {

}

Session::~Session() = default;

void Session::Initialize() {
	const int eState = m_eState.Load();

	if (eState == eConnectWait || eState == eConnected) {
		_NetLogWarn_("초기화 되지 않았거나 혹은 연결이 끊긴 대상만 초기화를 진행할 수 있습니다.");
		return;
	}

	m_LocalEndPoint = {};
	m_RemoteEndPoint = {};

	m_spRecvBuffer->MoveReadPos(0);
	m_spRecvBuffer->MoveWritePos(0);

	m_spSendBuffer->MoveReadPos(0);
	m_spSendBuffer->MoveWritePos(0);

	m_eState = eInitailized;
}

bool Session::Bind(const IPv4EndPoint& bindAddr) {
	if (!m_Socket.IsValid()) {
		_NetLogError_("바인드에 실패했습니다. INVALID_SOCKET 입니다.");
		WSASetLastError(WSA_INVALID_HANDLE);
		return false;
	}

	const int iBindRet = m_Socket.Bind(bindAddr);
	if (iBindRet == SOCKET_ERROR) {
		_NetLogError_("%s %s 바인드 실패 (%u)", TypeName(), bindAddr.ToString().Source(), Winsock::LastError());
		return false;
	}

	_NetLogDebug_("%s %s 바인드 완료", TypeName(), bindAddr.ToString().Source());
	m_LocalEndPoint = bindAddr;
	return true;
}


bool Session::Disconnect() {
	const int ePrevState = m_eState;

	if (ePrevState == eDisconnected)
		return true;

	m_eState = eDisconnected;
	m_bIocpConneced = false;

	// 굳이 오류 처리를 할 필요가 있나
	// WSAENOTSOCK: 소켓 할당이 안된 
	// WSAENOTCONN: 연결안된 소켓

	m_Socket.ShutdownBoth();
	m_Socket.Close();
	m_Socket.Invalidate();

	// 여기서 Disconnected를 호춡토록 구현했었는데 잘못된 로직이었다.
	// 예를들어서 TcpClient가 Connect 시도중 Disconnect를 시도하면
	// Connect 오버랩이 실패하면서 한번더 Disconnect를 호출하게되어버림.

	return true;
}


bool Session::SendAsync(ISendPacket* packet) {
	JCORE_REF_COUNT_GUARD(packet);
	WSABUF buf = packet->GetWSABuf();
	Int32UL uiSendBytes = 0;
	IOCPOverlapped* pOverlapped = dbg_new IOCPOverlappedSend(this, m_spIocp.GetPtr(), packet);

	// 0으로 즉시 성공하더라도 IOCP 워커에서 오버랩이 처리되므로 여기서 삭제를 해줄필요가 없다.
	const int iSendRet = m_Socket.SendEx(&buf, &uiSendBytes, pOverlapped);

	if (iSendRet == SOCKET_ERROR) {
		Int32U uiErrorCode = Winsock::LastError();
		if (uiErrorCode != WSA_IO_PENDING) {
			_NetLogError_("SendAsync 실패 (%u)", uiErrorCode);
			packet->Release();
			pOverlapped->Release();
			return false;
		}
	} 
	
	return true;
}

bool Session::SendAsync(const CommandBufferPtr& buffer) {
#ifdef DebugMode
	DebugAssertMsg(buffer->IsValid(), "보내고자하는 커맨드 버퍼 데이터가 이상합니다.");
#endif
	return SendAsync(dbg_new CommandBufferPacket(buffer));
}

CommandBufferPacket* Session::GetCommandBufferForSending() {
	JCORE_LOCK_GUARD(m_SendBufferLock);

	if (m_spSendBuffer->GetCommandCount() == 0)
		return nullptr;

	CommandBufferPtr spNewSendBuffer = MakeShared<CommandBuffer>(m_spBufferAllocator, m_spSendBuffer->GetBufferRequestSize());
	CommandBufferPtr spOldSendBuffer = m_spSendBuffer;

	m_spSendBuffer = spNewSendBuffer;

	CommandBufferPacket* pWrappedPacket = dbg_new CommandBufferPacket(spOldSendBuffer);
	
#ifdef DebugMode
	if (!spOldSendBuffer->IsValid()) {
		_NetLogError_("무야! 보내고자하는 커맨드 센드 버퍼 데이터가 이상합니다.");
		return nullptr;
	}
#endif
	return pWrappedPacket;
}

void Session::FlushSendBuffer() {
	CommandBufferPacket* pWrappedPacket = GetCommandBufferForSending();
	if (pWrappedPacket) SendAsync(pWrappedPacket);
}

bool Session::SendToAsync(ISendPacket* packet, const IPv4EndPoint& destination) {

	if (!destination.IsValidRemoteEndPoint()) {
		_NetLogError_("유효한 목적지 주소가 아닙니다.");
		return false;
	}

	JCORE_REF_COUNT_GUARD(packet);
	WSABUF buf = packet->GetWSABuf();
	Int32UL uiSendBytes = 0;
	IOCPOverlapped* pOverlapped = dbg_new IOCPOverlappedSendTo(this, m_spIocp.GetPtr(), packet);

	const int iResult = m_Socket.SendToEx(&buf, &uiSendBytes, pOverlapped, destination);
	if (iResult == SOCKET_ERROR) {
		Int32U uiErrorCode = Winsock::LastError();
		if (uiErrorCode != WSA_IO_PENDING) {
			DebugAssertMsg(false, "SendToAsync 실패 (%d)", uiErrorCode);
			packet->Release();
			pOverlapped->Release();
			return false;
		}
	}

	return true;
}

bool Session::SendToAsync(const CommandBufferPtr& buffer, const IPv4EndPoint& destination) {
#ifdef DebugMode
	DebugAssertMsg(buffer->IsValid(), "보내고자하는 커맨드 버퍼 데이터가 이상합니다.");
#endif
	return SendToAsync(dbg_new CommandBufferPacket(buffer), destination);
}

bool Session::SendToAsync(ISendPacket* packet) {
	return SendToAsync(packet, m_RemoteEndPoint);
}


bool Session::RecvAsync() {

	WSABUF buf = m_spRecvBuffer->GetRemainBuffer();
	Int32UL uiReceivedBytes = 0;
	IOCPOverlapped* pOverlapped = dbg_new IOCPOverlappedRecv(this, m_spIocp.GetPtr());

	const int iResult = m_Socket.ReceiveEx(&buf, &uiReceivedBytes, pOverlapped);

	if (iResult == SOCKET_ERROR) {
		Int32U uiErrorCode = Winsock::LastError();
		if (uiErrorCode != WSA_IO_PENDING) {
			_NetLogError_("RecvAsync 실패 (%u)", uiErrorCode);
			pOverlapped->Release();
			return false;
		}
	}
	return true;
}

bool Session::RecvFromAsync() {

	if (!m_Socket.IsBinded()) {
		_NetLogError_("소켓이 바인딩된 상태여야 수신이 가능합니다. 상대방에게 먼저 송신하여 오토 바인딩해주거나 수동 바인딩을 해주세요.");
		return false;
	}

	WSABUF buf = m_spRecvBuffer->GetRemainBuffer();
	Int32UL uiReceivedBytes = 0;
	IOCPOverlappedRecvFrom* pRecvFromOverlapped = dbg_new IOCPOverlappedRecvFrom(this, m_spIocp.GetPtr());

	const int iResult = m_Socket.ReceiveFromEx(
		&buf,
		&uiReceivedBytes,
		pRecvFromOverlapped,
		&m_RemoteEndPoint.InternetAddr
	);

	if (iResult == SOCKET_ERROR) {
		Int32U uiErrorCode = Winsock::LastError();
		if (uiErrorCode != WSA_IO_PENDING) {
			DebugAssertMsg(false, "RecvFromAsync 실패 (%d)", uiErrorCode);
			pRecvFromOverlapped->Release();
			return false;
		}
	}

	return true;
}

void Session::SendAlloc(ICommand* cmd) {
	JCORE_LOCK_GUARD(m_SendBufferLock);

	const int iCmdSize = cmd->CmdLen;
	if (m_spSendBuffer->GetWritePos() + iCmdSize >= MAX_MSS) {
		FlushSendBuffer();
	}

	DebugAssertMsg(iCmdSize <= m_spSendBuffer->GetRemainBufferSize(), "버퍼의 남은 공간에 넣을 커맨드가 너무 큽니다. (CmdSize: %d, RemainBufferCapacity: %d)", iCmdSize, m_spSendBuffer->GetRemainBufferSize());
	m_spSendBuffer->Alloc(cmd);
}

void Session::Received(Int32UL receivedBytes) {
	m_spRecvBuffer->MoveWritePos(receivedBytes);

	for (;;) {
		const int iReadableBufferSize = m_spRecvBuffer->GetReadableBufferSize();
		// 패킷의 헤더 크기만큼 데이터를 수신하지 않았으면 모일때까지 기달
		if (iReadableBufferSize < PacketHeaderSize_v)
			return;

		// 패킷 헤더 길이 + 패킷 길이 만큼 수신하지 않았으면 다시 모일때까지 기다린다.
		IRecvPacket* packet = m_spRecvBuffer->Peek<IRecvPacket*>();
		const int iPacketLength = packet->GetPacketLength();
		if (iReadableBufferSize < (PacketHeaderSize_v + iPacketLength)) {
			return;
		}

		m_spRecvBuffer->MoveReadPos(PacketHeaderSize_v);
		NotifyPacket(packet);

		for (int i = 0; i < packet->GetCommandCount(); i++) {
			ICommand* pCmd = m_spRecvBuffer->Peek<ICommand*>();
			CmdLen_t uiCmdLen = pCmd->GetCommandLen();

			NotifyCommand(pCmd);

			if (m_spRecvBuffer->MoveReadPos(uiCmdLen) == false) {
				_NetLogWarn_("커맨드 크기가 이상합니다.");
				m_spRecvBuffer->ResetPosition();
				return;
			}
		}

		if (m_spRecvBuffer->GetReadPos() == m_spRecvBuffer->GetWritePos()) {
			// 만약 수신한 데이터를 모두 읽었으면 포지션을 그냥 0으로 옮긴다.
			m_spRecvBuffer->ResetPosition();
		} else {
			// 읽은 위치만큼은 이제 다시 쓰일일이 없으므로 버퍼를 앞으로 당긴다. 
			// WritePos 이후로 데이터를 쌓을 수 있도록하기 위해
			m_spRecvBuffer->Pop(m_spRecvBuffer->GetReadPos(), true);
		}
	}
}



void Session::WaitForZeroPending() {
	int iPrevPendingCount = 0;
	int iEqualCount = 0;

	while (true) {

		const int iPending = m_iOveralappedPendingCount;

		if (iPending == 0)
			break;

		if (iPending < 0) {
			_NetLogWarn_("멍미 펜딩 카운트가 움수 인뎁쇼 (%d)", iPending);
			break;
		}

		if (iPrevPendingCount == iPending)
			iEqualCount++;

		if (iEqualCount >= 1'000'000) {
			iEqualCount = 0;
			_NetLogWarn_("펜딩 카운트 기달 %d", iPending);
		}

		iPrevPendingCount = iPending;
	}
}

NS_JNET_END 