#include <mcp/PCH/Core.h>
#include <mcp/Protocol/MCPTcpServer.h>
#include <mcp/Protocol/MCPServer.h>

USING_NS_JNET;

NS_MCP_BEGIN

//////////////////////////////////////////////////////////////////////////////////////////
MCPTcpServer::MCPTcpServer(MCPServer* _pOwner, const jnet::IOCPPtr& _pIocp, const jc::MemoryPoolAbstractPtr& _pAlloc)
    : TcpServer(_pIocp, _pAlloc)
    , owner_(_pOwner)
{
}

//////////////////////////////////////////////////////////////////////////////////////////
jnet::TcpSession* MCPTcpServer::CreateSession()
{
    return dbg_new MCPTransport(this, pIocp_, pBufferAllocator_);
}

NS_MCP_END
