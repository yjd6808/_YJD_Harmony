/*
 * 작성자: 윤정도
 * 생성일: 3/25/2023 12:01:18 AM
 * =====================
 *
 */

#include "Center.h"
#include "CenterCoreHeader.h"
#include "CenterSession.h"

USING_NS_JC;
USING_NS_JNET;

//////////////////////////////////////////////////////////////////////////////////////////
CenterSession::CenterSession(
    TcpServer* _pServer,
    const IOCPPtr& _pIocp,
    const JCore::MemoryPoolAbstractPtr& _pBufferAllocator,
    int _recvBufferSize,
    int _sendBufferSize
)
    : TcpSession(_pServer, _pIocp, _pBufferAllocator, nullptr, _recvBufferSize, _sendBufferSize)
    , clientType_(ServerProcessType::None)
    , serverId_(InvalidValue_v)
{
}

//////////////////////////////////////////////////////////////////////////////////////////
void CenterSession::OnConnected()
{
    clientType_ = ServerProcessType::None;
}

//////////////////////////////////////////////////////////////////////////////////////////
void CenterSession::OnDisconnected()
{
}

//////////////////////////////////////////////////////////////////////////////////////////
void CenterSession::SetClientInformation(ServerProcessType_t _clientType, Int8 _serverId)
{
    clientType_ = _clientType;
    serverId_ = _serverId;
}

//////////////////////////////////////////////////////////////////////////////////////////
bool CenterSession::IsValid() const
{
    return clientType_ != ServerProcessType::None && serverId_ != InvalidValue_v;
}
