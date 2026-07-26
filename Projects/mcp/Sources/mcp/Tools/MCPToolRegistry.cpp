#include <mcp/PCH/Core.h>
#include <mcp/Tools/MCPToolRegistry.h>

NS_MCP_BEGIN

//////////////////////////////////////////////////////////////////////////////////////////
void MCPToolRegistry::Register(const MCPToolDefinition& _def, MCPToolHandler _handler)
{
    MCPTool tool;
    tool.definition = _def;
    tool.handler    = std::move(_handler);
    tools_[_def.name] = std::move(tool);
}

//////////////////////////////////////////////////////////////////////////////////////////
void MCPToolRegistry::Unregister(const std::string& _name)
{
    tools_.erase(_name);
}

//////////////////////////////////////////////////////////////////////////////////////////
MCPTool* MCPToolRegistry::Find(const std::string& _name)
{
    auto it = tools_.find(_name);
    if (it == tools_.end())
        return nullptr;
    return &it->second;
}

//////////////////////////////////////////////////////////////////////////////////////////
Json::Value MCPToolRegistry::ListAll() const
{
    Json::Value arr(Json::arrayValue);
    for (const auto& pair : tools_)
        arr.append(pair.second.definition.ToJson());
    return arr;
}

NS_MCP_END
