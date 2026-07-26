#pragma once

#include <mcp/Namespace.h>

NS_MCP_BEGIN

struct MCPCapabilities
{
    struct Tools
    {
        bool supported = true;
    } tools;

    struct Resources
    {
        bool supported = false;
    } resources;

    struct Prompts
    {
        bool supported = false;
    } prompts;

    Json::Value ToJson() const;
};

NS_MCP_END
