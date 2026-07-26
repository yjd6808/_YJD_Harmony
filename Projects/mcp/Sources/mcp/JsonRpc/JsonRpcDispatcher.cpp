#include <mcp/PCH/Core.h>
#include <mcp/JsonRpc/JsonRpcError.h>
#include <mcp/JsonRpc/JsonRpcMessage.h>
#include <mcp/JsonRpc/JsonRpcDispatcher.h>

NS_MCP_BEGIN

//////////////////////////////////////////////////////////////////////////////////////////
void JsonRpcDispatcher::Register(const std::string& _method, JsonRpcHandler _handler)
{
    handlers_[_method] = std::move(_handler);
}

//////////////////////////////////////////////////////////////////////////////////////////
void JsonRpcDispatcher::Unregister(const std::string& _method)
{
    handlers_.erase(_method);
}

//////////////////////////////////////////////////////////////////////////////////////////
JsonRpcMessage JsonRpcDispatcher::Dispatch(const JsonRpcMessage& _request) const
{
    std::string method = _request.GetMethod();
    auto it = handlers_.find(method);

    if (it == handlers_.end())
    {
        auto err = JsonRpcError::Make(JsonRpcErrorCode::MethodNotFound, "Method not found: " + method);
        return JsonRpcMessage::MakeError(err, _request.GetId());
    }

    try
    {
        Json::Value result = it->second(_request.GetParams());
        return JsonRpcMessage::MakeResponse(result, _request.GetId());
    }
    catch (const std::exception& e)
    {
        auto err = JsonRpcError::Make(JsonRpcErrorCode::InternalError, e.what());
        return JsonRpcMessage::MakeError(err, _request.GetId());
    }
}

NS_MCP_END
