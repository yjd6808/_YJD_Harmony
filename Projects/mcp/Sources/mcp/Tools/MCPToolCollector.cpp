#include <mcp/PCH/Core.h>
#include <mcp/Tools/MCPToolCollector.h>
#include <mcp/Protocol/MCPServer.h>
#include <mcp/Tools/MCPToolRegistry.h>

NS_MCP_BEGIN

//////////////////////////////////////////////////////////////////////////////////////////
std::vector<PendingTool>& MCPToolCollector::GetPending()
{
    static std::vector<PendingTool> s_pending;
    return s_pending;
}

//////////////////////////////////////////////////////////////////////////////////////////
bool MCPToolCollector::Add(const char* _name, const char* _desc, MCPToolHandler _handler)
{
    GetPending().push_back({ _name, _desc, std::move(_handler) });
    return true;
}

//////////////////////////////////////////////////////////////////////////////////////////
void MCPToolCollector::FlushIn(MCPServer* _pServer)
{
    auto& pending = GetPending();
    for (auto& pt : pending)
    {
        MCPToolDefinition def;
        def.name        = std::move(pt.name);
        def.description = std::move(pt.description);
        _pServer->GetToolRegistry().Register(def, std::move(pt.handler));
    }
    pending.clear();
}

//////////////////////////////////////////////////////////////////////////////////////////
void MCPToolCollector::Clear()
{
    GetPending().clear();
}

// Register built-in tools (static init — MCPToolCollector.obj is always linked via FlushIn)
REGISTER_MCP_TOOL("echo", "Echoes back the input message",
    [](const Json::Value& _params) -> Json::Value {
        Json::Value result;
        result["message"] = _params.get("message", "hello");
        return result;
    }
);

REGISTER_MCP_TOOL("ping", "Returns a pong response for connectivity check",
    [](const Json::Value&) -> Json::Value {
        Json::Value result;
        result["pong"] = true;
        return result;
    }
);

NS_MCP_END