#include <mcp/PCH/Core.h>
#include <mcp/Tools/MCPTool.h>

NS_MCP_BEGIN

//////////////////////////////////////////////////////////////////////////////////////////
Json::Value MCPToolInputSchema::ToJson() const
{
    Json::Value schema;
    schema["type"] = type;
    if (!properties.isNull())
        schema["properties"] = properties;
    if (!required.isNull())
        schema["required"] = required;
    return schema;
}

//////////////////////////////////////////////////////////////////////////////////////////
Json::Value MCPToolDefinition::ToJson() const
{
    Json::Value def;
    def["name"] = name;
    def["description"] = description;
    def["inputSchema"] = inputSchema.ToJson();
    return def;
}

NS_MCP_END
