#include <mcp/PCH/Core.h>
#include <mcp/JsonRpc/JsonRpcError.h>

NS_MCP_BEGIN

//////////////////////////////////////////////////////////////////////////////////////////
JsonRpcError JsonRpcError::Make(JsonRpcErrorCode _code, std::string _msg, Json::Value _data)
{
    JsonRpcError err;
    err.code    = _code;
    err.message = std::move(_msg);
    err.data    = std::move(_data);
    return err;
}

//////////////////////////////////////////////////////////////////////////////////////////
Json::Value JsonRpcError::ToJson() const
{
    Json::Value root;
    root["code"]    = static_cast<int>(code);
    root["message"] = message;
    if (!data.isNull())
        root["data"] = data;
    return root;
}

NS_MCP_END
