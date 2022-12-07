/*
	작성자 : 윤정도
	TcpClient의 부모 클래스
	
	2가지 역할을 수행한다.
	1. 서버측에서 연결을 수락된 클라이언트의 역할 수행한다. - TcpSession
	2. 클라이언트의 공통기능을 수행한다.					 - TcpClient

	멤버 함수, 변수는 모두 protected로 두었다.
	friend class로 허용된 녀서들만 접근하여 제어할 수 있도록함
*/

#pragma once


#include <JCore/Sync/NormalLock.h>
#include <JNetwork/Socket.h>
#include <JNetwork/Packet.h>
#include <JNetwork/Host/TcpServerEventListener.h>
#include <JNetwork/IOCP/IOCP.h>
#include <JNetwork/Buffer.h>



namespace JNetwork {

class TcpSession
{
public:
	enum class State
	{
		Uninitialized	= 0,
		Initailized		= 1,
		AcceptWait		= 2,		// 서버에서 연결 수락 대기
		ConnectWait		= 2,		// 클라에서 연결 시도 대기 - ConnectEx를 활용한 오버랩 연결 시도시에 사용할 듯?
		Accepted		= 3,		// 서버에서 연결 수략 완료
		Connected		= 3,		// 클라에서 연결 완료
		Disconnected	= 4
	};

public:
	IPv4EndPoint GetLocalEndPoint() const { return m_LocalEndPoint; }
	IPv4EndPoint GetRemoteEndPoint() const { return m_RemoteEndPoint; }
	SessionBuffer* GetReceiveBuffer() { return &m_ReceiveBuffer; }
	State GetState() const { return m_eState; }
	bool SendAsync(ISendPacket* packet);
	virtual bool Disconnect();
	
	// 커스텀 정보 저장 기능 추가
	void SetTag(void* ptr) { m_pTag = ptr; }

	template <typename T>
	T GetTag() {
		static_assert(JCore::IsPointerType_v<T>, "... T must be pointer type");
		return reinterpret_cast<T>(m_pTag);
	}
protected:
	TcpSession(IOCP* iocp, TcpServerEventListener* listener = nullptr);
	virtual ~TcpSession();
	
	
	bool ReceiveAsync();
	bool AcceptAsync(SOCKET hListeningSock, LPOVERLAPPED pOverlapped);
	bool CheckState(State state);


	Socketv4 Socket() const { return m_ClientSocket; }

	virtual bool Initialize();
	virtual void AcceptWait();												// 미리 생성된 TcpSession이 AcceptEx를 호출하기 직전에 호출될 함수 - 연결 수락 대기상태
	virtual void ConnectWait() {};											// 클라이언트가 서버와 연결을 요청시 호출될 함수			(TcpSession에서는 사용하지 않음)
	virtual bool Accepted(SOCKET listeningSocket, Int32UL receivedBytes);	// 서버가 해당세션의 연결을 수락시 최종적으로 호출될 함수
	virtual void Connected() {};											// 클라이언트가 서버와 연결됬을 시 최종적으로 호출될 함수	(TcpSession에서는 사용하지 않음)
	virtual void Received(Int32UL receivedBytes);							// 세션 또는 클라이언트가 데이터를 수신했을 때 최종적으로 호출될 함수
	virtual void Sent(ISendPacket* sentPacket, Int32UL receivedBytes);			// 세션 또는 클라이언트가 데이터를 송신했을 때 최종적으로 호출될 함수

	virtual void NotifyCommand(ICommand* cmd);								// 세션 입장에서는 ServerEventListener에 커맨드를 전달하고 클라이언트 입장에서는 ClientEventListener에 커맨드를 전달하도록 한다.
private:
	TcpServerEventListener* m_pServerEventListener;							// TcpClient 입장에서는 nullptr로 사용하지 않음 / 그래서 private으로 둠
protected:
	State m_eState;
	SessionBuffer m_ReceiveBuffer;
	Socketv4 m_ClientSocket;
	IOCP* m_pIocp;															// TcpClient 입장에서는 생성/소멸을 해줘야하는 객체이지만 TcpSession 입장에서는 TcpServer의 IOCP를 단순히 참조하는 용도이다.
	IPv4EndPoint m_RemoteEndPoint;
	IPv4EndPoint m_LocalEndPoint;
	void* m_pTag{};
	bool m_bReuseSession;
	JCore::NormalLock m_Lock;
	
	friend class TcpServer;
	friend class TcpSessionContainer;
	friend class IOCPOverlappedAccept;
	friend class IOCPOverlappedConnect;
	friend class IOCPOverlappedReceive;
	friend class IOCPOverlappedSend;
};

} // namespace JNetwork