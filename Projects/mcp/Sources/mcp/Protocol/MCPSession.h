#pragma once

#include <memory>
#include <queue>
#include <mcp/JsonRpc/JsonRpcMessage.h>
#include <mcp/JsonRpc/JsonRpcDispatcher.h>

NS_MCP_BEGIN

class MCPTransport;
class MCPMessageQueue;
class MCPServer;

class MCPSession
{
public:
    MCPSession(MCPServer* _pServer, MCPTransport* _pTransport);
    ~MCPSession();

    MCPMessageQueue* GetRecvQueue();
    void Send(const std::string& _payload);
    void Disconnect();
    void ProcessMessages();

private:
    JsonRpcMessage HandleInitialize(const JsonRpcMessage& _request);
    Json::Value HandleListTools(const Json::Value& _params);
    Json::Value HandleCallTool(const Json::Value& _params);

    MCPServer*              server_;
    MCPTransport*           transport_;
    std::unique_ptr<MCPMessageQueue> recvQueue_;
    JsonRpcDispatcher       dispatcher_;
    bool                    initialized_;
};

NS_MCP_END
