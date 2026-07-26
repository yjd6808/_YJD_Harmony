#pragma once

#include <vector>
#include <unordered_map>
#include <jc/Sync/NormalLock.h>
#include <jc/Primitives/Atomic.h>
#include <jnet/IOCP/IOCP.h>
#include <mcp/Namespace.h>
#include <mcp/Protocol/MCPCapabilities.h>
#include <mcp/Tools/MCPToolRegistry.h>
#include <mcp/JsonRpc/JsonRpcDispatcher.h>

NS_MCP_BEGIN

class MCPSession;
class MCPTransport;
class MCPTcpServer;

class MCPServer
{
public:
    static MCPServer* Create();
    void Destroy();

    bool Run(int _port);
    bool Start(int _port);
    void PollEvents();
    void Stop();
    void RequestShutdown();

    MCPCapabilities& GetCapabilities();
    MCPToolRegistry& GetToolRegistry();
    JsonRpcDispatcher& GetDispatcher();
    bool IsRunning() const;
    bool IsShutdownRequested() const;

    jnet::IOCPPtr GetIOCP() { return iocp_; }

    void OnTransportConnected(MCPTransport* _pTransport);
    void OnTransportDisconnected(MCPTransport* _pTransport);

private:
    MCPServer();
    ~MCPServer();

    MCPSession* CreateSession(MCPTransport* _pTransport);
    void RemoveSession(MCPSession* _pSession);

    jnet::IOCPPtr                        iocp_;
    MCPTcpServer*                        tcpServer_;
    std::unordered_map<MCPTransport*, MCPSession*> sessionMap_;
    std::vector<MCPSession*>             pendingDelete_;
    jc::NormalLock                       sessionLock_;
    MCPCapabilities                      capabilities_;
    MCPToolRegistry                      toolRegistry_;
    JsonRpcDispatcher                    dispatcher_;
    jc::AtomicBool                       running_;
    jc::AtomicBool                       shutdownRequested_;
};

NS_MCP_END
