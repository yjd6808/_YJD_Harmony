#pragma once

#include <mcp/Protocol/MCPServer.h>

class MCPCore : public jc::SingletonPointer<MCPCore>
{
    friend class TSingleton;
    MCPCore();
    ~MCPCore() override;

public:
    bool Initialize();
    void Finalize();
    void PollEvents();

    mcp::MCPServer& Main();

private:
    mcp::MCPServer* pMainMCP_;
};

#define g_cMCPCore JC_DECL_SINGLETON_BODY(MCPCore)
