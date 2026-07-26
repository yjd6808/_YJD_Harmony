#pragma once

#include <jnet/Host/TcpServer.h>
#include <mcp/Transport/MCPTransport.h>

NS_MCP_BEGIN

class MCPServer;

class MCPTcpServer : public jnet::TcpServer
{
public:
    MCPTcpServer(MCPServer* _pOwner, const jnet::IOCPPtr& _pIocp, const jc::MemoryPoolAbstractPtr& _pAlloc);
    virtual jnet::TcpSession* CreateSession() override;

private:
    MCPServer* owner_;
};

NS_MCP_END
