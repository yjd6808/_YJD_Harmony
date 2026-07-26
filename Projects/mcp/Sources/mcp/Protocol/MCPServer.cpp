#include <mcp/PCH/Core.h>
#include <mcp/Protocol/MCPConstants.h>
#include <mcp/Protocol/MCPCapabilities.h>
#include <mcp/Protocol/MCPSession.h>
#include <mcp/Protocol/MCPServer.h>
#include <mcp/Protocol/MCPTcpServer.h>
#include <mcp/Transport/MCPTransport.h>
#include <mcp/Transport/MCPMessageQueue.h>
#include <mcp/Tools/MCPToolCollector.h>
#include <jnet/EventListener/ServerEventListener.h>
#include <jnet/IPEndPoint.h>
#include <jnet/IPAddress.h>
#include <jc/Pool/IndexedMemoryPool.h>
#include <thread>
#include <cstdio>

USING_NS_JC;
USING_NS_JNET;

NS_MCP_BEGIN

class MCPServerEventListener : public jnet::ServerEventListener
{
public:
    MCPServerEventListener(MCPServer* _pServer)
        : server_(_pServer)
    {
    }

    virtual void OnConnected(jnet::Session* _pSession) override
    {
        server_->OnTransportConnected(static_cast<MCPTransport*>(_pSession));
    }

    virtual void OnDisconnected(jnet::Session* _pSession, _u32 _errorCode) override
    {
        server_->OnTransportDisconnected(static_cast<MCPTransport*>(_pSession));
    }

private:
    MCPServer* server_;
};

//////////////////////////////////////////////////////////////////////////////////////////
MCPServer::MCPServer()
    : tcpServer_(nullptr)
    , running_(false)
    , shutdownRequested_(false)
{
}

//////////////////////////////////////////////////////////////////////////////////////////
MCPServer::~MCPServer()
{
    Stop();
}

//////////////////////////////////////////////////////////////////////////////////////////
MCPServer* MCPServer::Create()
{
    return dbg_new MCPServer();
}

//////////////////////////////////////////////////////////////////////////////////////////
void MCPServer::Destroy()
{
    delete this;
}

//////////////////////////////////////////////////////////////////////////////////////////
bool MCPServer::Run(int _port)
{
    if (!Start(_port))
        return false;

    running_ = true;

    while (!shutdownRequested_)
    {
        PollEvents();
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }

    Stop();
    return true;
}

//////////////////////////////////////////////////////////////////////////////////////////
bool MCPServer::Start(int _port)
{
    iocp_ = MakeShared<IOCP>(2);
    if (!iocp_.Exist())
        return false;

    iocp_->Run();

    auto pool = jc::MakeShared<CIndexedMemoryPool>();
    tcpServer_ = dbg_new MCPTcpServer(this, iocp_, pool);
    tcpServer_->SetEventListener(dbg_new MCPServerEventListener(this));

    IPv4EndPoint endPoint(IPv4Address::Parse("127.0.0.1"), static_cast<_u16>(_port));
    if (!tcpServer_->Start(endPoint))
        return false;

    MCPToolCollector::FlushIn(this);
    return true;
}

//////////////////////////////////////////////////////////////////////////////////////////
void MCPServer::PollEvents()
{
    // Deferred deletion from previous cycle
    {
        NormalLockGuard guard(sessionLock_);
        for (auto* pSession : pendingDelete_)
            delete pSession;
        pendingDelete_.clear();
    }

    // Collect active sessions under lock
    std::vector<MCPSession*> sessions;
    {
        NormalLockGuard guard(sessionLock_);
        sessions.reserve(sessionMap_.size());
        for (auto& pair : sessionMap_)
            sessions.push_back(pair.second);
    }

    // Process outside lock
    for (auto* pSession : sessions)
        pSession->ProcessMessages();
}

//////////////////////////////////////////////////////////////////////////////////////////
void MCPServer::Stop()
{
    if (tcpServer_)
        tcpServer_->Stop();

    {
        NormalLockGuard guard(sessionLock_);

        for (auto& pair : sessionMap_)
            delete pair.second;
        sessionMap_.clear();

        for (auto* pSession : pendingDelete_)
            delete pSession;
        pendingDelete_.clear();
    }

    delete tcpServer_;
    tcpServer_ = nullptr;

    if (iocp_.Exist())
    {
        iocp_->Join();
        iocp_ = nullptr;
    }

    running_ = false;
    shutdownRequested_ = false;
}

//////////////////////////////////////////////////////////////////////////////////////////
void MCPServer::RequestShutdown()
{
    shutdownRequested_ = true;
}

//////////////////////////////////////////////////////////////////////////////////////////
void MCPServer::OnTransportConnected(MCPTransport* _pTransport)
{
    NormalLockGuard guard(sessionLock_);
    MCPSession* pSession = CreateSession(_pTransport);
    _pTransport->SetFrameCallback([this, pSession](MCPTransport*, std::string _payload) {
        pSession->GetRecvQueue()->Enqueue(std::move(_payload));
    });
}

//////////////////////////////////////////////////////////////////////////////////////////
void MCPServer::OnTransportDisconnected(MCPTransport* _pTransport)
{
    NormalLockGuard guard(sessionLock_);
    auto it = sessionMap_.find(_pTransport);
    if (it != sessionMap_.end())
    {
        pendingDelete_.push_back(it->second);
        sessionMap_.erase(it);
    }
}

//////////////////////////////////////////////////////////////////////////////////////////
MCPSession* MCPServer::CreateSession(MCPTransport* _pTransport)
{
    MCPSession* pSession = new MCPSession(this, _pTransport);
    sessionMap_[_pTransport] = pSession;
    return pSession;
}

//////////////////////////////////////////////////////////////////////////////////////////
void MCPServer::RemoveSession(MCPSession*)
{
}

//////////////////////////////////////////////////////////////////////////////////////////
MCPCapabilities& MCPServer::GetCapabilities() { return capabilities_; }

//////////////////////////////////////////////////////////////////////////////////////////
MCPToolRegistry& MCPServer::GetToolRegistry() { return toolRegistry_; }

//////////////////////////////////////////////////////////////////////////////////////////
JsonRpcDispatcher& MCPServer::GetDispatcher() { return dispatcher_; }

//////////////////////////////////////////////////////////////////////////////////////////
bool MCPServer::IsRunning() const { return running_; }

//////////////////////////////////////////////////////////////////////////////////////////
bool MCPServer::IsShutdownRequested() const { return shutdownRequested_; }

NS_MCP_END
