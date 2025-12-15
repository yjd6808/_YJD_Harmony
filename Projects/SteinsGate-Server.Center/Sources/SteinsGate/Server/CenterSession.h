/*
 * 작성자: 윤정도
 * 생성일: 3/25/2023 12:01:06 AM
 * =====================
 * 인터 통신을 위해 접속한 인증,로비,게임 서버 세션
 */


#pragma once

#include <JNetwork/Host/TcpSession.h>
#include <SteinsGate/Common/ServerEnum.h>

class CenterSession : public JNetwork::TcpSession
{
public:
    struct BootState
    {
        ServerType_t ServerType;
        ServerBootState_t State;
    };

    CenterSession(
        JNetwork::TcpServer* _pServer,
        const JNetwork::IOCPPtr& _pIocp,
        const JCore::MemoryPoolAbstractPtr& _pBufferAllocator,
        int _recvBufferSize,
        int _sendBufferSize
    );

    void OnConnected() override;
    void OnDisconnected() override;

    ServerProcessType_t GetClientType() const { return clientType_; }
    Int8 GetServerId() const { return serverId_; }

    void SetClientType(ServerProcessType_t _clientType) { clientType_ = _clientType; }
    void SetClientInformation(ServerProcessType_t _clientType, Int8 _serverId);
    bool IsValid() const;
private:
    ServerProcessType_t clientType_;
    Int8 serverId_;
};
