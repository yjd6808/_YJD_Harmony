#pragma once

NS_MCP_BEGIN

namespace MCPProtocol
{
    constexpr const char* kVersion = "0.1.0";
    constexpr const char* kProtocolName = "mcp";

    constexpr int kDefaultPort = 9100;
    constexpr int kRecvBufferSize = 8192;
    constexpr int kMaxFrameSize = 1024 * 1024;

    namespace Methods
    {
        constexpr const char* kInitialize        = "initialize";
        constexpr const char* kShutdown          = "shutdown";
        constexpr const char* kListTools         = "tools/list";
        constexpr const char* kCallTool          = "tools/call";
        constexpr const char* kResourcesList     = "resources/list";
        constexpr const char* kResourcesRead     = "resources/read";
        constexpr const char* kPromptsList       = "prompts/list";
        constexpr const char* kPromptsGet        = "prompts/get";
        constexpr const char* kNotificationsInitialized = "notifications/initialized";
    }
}

NS_MCP_END
