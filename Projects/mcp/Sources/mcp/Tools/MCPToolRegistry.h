#pragma once

#include <unordered_map>
#include <mcp/Tools/MCPTool.h>
#include <mcp/Namespace.h>

NS_MCP_BEGIN

class MCPToolRegistry
{
public:
    void Register(const MCPToolDefinition& _def, MCPToolHandler _handler);
    void Unregister(const std::string& _name);

    MCPTool* Find(const std::string& _name);
    Json::Value ListAll() const;

private:
    std::unordered_map<std::string, MCPTool> tools_;
};

NS_MCP_END
