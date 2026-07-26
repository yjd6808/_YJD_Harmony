#pragma once

#include <functional>
#include <vector>
#include <mcp/Tools/MCPTool.h>

NS_MCP_BEGIN

class MCPServer;

struct PendingTool
{
    std::string     name;
    std::string     description;
    MCPToolHandler  handler;
};

class MCPToolCollector
{
public:
    static bool Add(const char* _name, const char* _desc, MCPToolHandler _handler);
    static void FlushIn(MCPServer* _pServer);
    static void Clear();

private:
    static std::vector<PendingTool>& GetPending();
};

NS_MCP_END

#define MCP_CONCAT_(a, b)    a##b
#define MCP_XCONCAT_(a, b)   MCP_CONCAT_(a, b)

#define REGISTER_MCP_TOOL(name, desc, handler) \
    static bool MCP_XCONCAT_(_sg_mcp_reg_, __COUNTER__) = \
        mcp::MCPToolCollector::Add(name, desc, handler)