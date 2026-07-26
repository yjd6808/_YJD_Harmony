#pragma once

NS_MCP_BEGIN

enum class JsonRpcErrorCode : int
{
    ParseError     = -32700,
    InvalidRequest = -32600,
    MethodNotFound = -32601,
    InvalidParams  = -32602,
    InternalError  = -32603,
    ServerError    = -32000,
};

struct JsonRpcError
{
    JsonRpcErrorCode code;
    std::string      message;
    Json::Value      data;

    static JsonRpcError Make(JsonRpcErrorCode _code, std::string _msg, Json::Value _data = Json::nullValue);
    Json::Value ToJson() const;
};

NS_MCP_END
