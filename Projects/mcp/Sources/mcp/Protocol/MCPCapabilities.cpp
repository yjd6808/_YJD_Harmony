#include <mcp/PCH/Core.h>
#include <mcp/Protocol/MCPCapabilities.h>

NS_MCP_BEGIN

//////////////////////////////////////////////////////////////////////////////////////////
Json::Value MCPCapabilities::ToJson() const
{
    Json::Value cap;
    cap["tools"]["supported"]     = tools.supported;
    cap["resources"]["supported"] = resources.supported;
    cap["prompts"]["supported"]   = prompts.supported;
    return cap;
}

NS_MCP_END
