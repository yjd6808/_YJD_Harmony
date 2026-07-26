#pragma once

#include <functional>
#include <string>
#include <mcp/Namespace.h>

NS_MCP_BEGIN

struct MCPToolInputSchema
{
    std::string type = "object";
    Json::Value properties;
    Json::Value required;

    Json::Value ToJson() const;
};

struct MCPToolDefinition
{
    std::string          name;
    std::string          description;
    MCPToolInputSchema   inputSchema;

    Json::Value ToJson() const;
};

using MCPToolHandler = std::function<Json::Value(const Json::Value&)>;

struct MCPTool
{
    MCPToolDefinition definition;
    MCPToolHandler    handler;
};

NS_MCP_END
