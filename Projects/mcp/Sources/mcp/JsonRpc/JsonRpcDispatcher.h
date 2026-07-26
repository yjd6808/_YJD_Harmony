#pragma once

#include <functional>
#include <string>
#include <unordered_map>
#include <json.h>
#include <mcp/Namespace.h>

NS_MCP_BEGIN

class JsonRpcMessage;

using JsonRpcHandler = std::function<Json::Value(const Json::Value&)>;

class JsonRpcDispatcher
{
public:
    void Register(const std::string& _method, JsonRpcHandler _handler);
    void Unregister(const std::string& _method);
    JsonRpcMessage Dispatch(const JsonRpcMessage& _request) const;

private:
    std::unordered_map<std::string, JsonRpcHandler> handlers_;
};

NS_MCP_END
