#pragma once

#include <mcp/Namespace.h>
#include <mcp/Protocol/MCPServer.h>

class MCPCore : public jc::SingletonPointer<MCPCore>
{
    friend class TSingleton;
    MCPCore();
    ~MCPCore();

public:
    bool Initialize();
    void Finalize();
    void PollEvents();

    mcp::MCPServer& Main();

private:
    mcp::MCPServer* pMainMCP_;
};

#define g_cMCPCore JC_DECL_SINGLETON_BODY(MCPCore)
