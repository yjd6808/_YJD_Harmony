#include <mcp/PCH/Core.h>
#include <mcp/JsonRpc/JsonRpcError.h>
#include <mcp/JsonRpc/JsonRpcMessage.h>
#include <sstream>

NS_MCP_BEGIN

//////////////////////////////////////////////////////////////////////////////////////////
JsonRpcMessage::JsonRpcMessage()
    : type_(Type::Request), valid_(false)
{
}

//////////////////////////////////////////////////////////////////////////////////////////
JsonRpcMessage JsonRpcMessage::MakeRequest(const std::string& _method, const Json::Value& _params, const std::string& _id)
{
    JsonRpcMessage msg;
    msg.type_   = Type::Request;
    msg.method_ = _method;
    msg.params_ = _params;
    msg.id_     = _id;
    msg.valid_  = true;
    return msg;
}

//////////////////////////////////////////////////////////////////////////////////////////
JsonRpcMessage JsonRpcMessage::MakeNotification(const std::string& _method, const Json::Value& _params)
{
    JsonRpcMessage msg;
    msg.type_   = Type::Notification;
    msg.method_ = _method;
    msg.params_ = _params;
    msg.valid_  = true;
    return msg;
}

//////////////////////////////////////////////////////////////////////////////////////////
JsonRpcMessage JsonRpcMessage::MakeResponse(const Json::Value& _result, const std::string& _id)
{
    JsonRpcMessage msg;
    msg.type_   = Type::Response;
    msg.result_ = _result;
    msg.id_     = _id;
    msg.valid_  = true;
    return msg;
}

//////////////////////////////////////////////////////////////////////////////////////////
JsonRpcMessage JsonRpcMessage::MakeError(const JsonRpcError& _error, const std::string& _id)
{
    JsonRpcMessage msg;
    msg.type_   = Type::Error;
    msg.error_  = _error;
    msg.id_     = _id;
    msg.valid_  = true;
    return msg;
}

//////////////////////////////////////////////////////////////////////////////////////////
JsonRpcMessage JsonRpcMessage::Parse(const std::string& _json)
{
    JsonRpcMessage msg;
    Json::CharReaderBuilder reader;
    Json::Value root;
    std::string errs;

    std::istringstream ss(_json);
    if (!Json::parseFromStream(reader, ss, &root, &errs))
        return msg;

    msg.id_ = root.get("id", "").asString();

    bool hasMethod = root.isMember("method");
    bool hasResult = root.isMember("result");
    bool hasError  = root.isMember("error");

    if (hasMethod)
    {
        msg.method_ = root["method"].asString();
        msg.params_ = root.get("params", Json::Value());
        msg.type_   = root.isMember("id") ? Type::Request : Type::Notification;
        msg.valid_  = true;
    }
    else if (hasResult)
    {
        msg.type_   = Type::Response;
        msg.result_ = root["result"];
        msg.valid_  = true;
    }
    else if (hasError)
    {
        msg.type_ = Type::Error;
        const Json::Value& e = root["error"];
        msg.error_.code    = static_cast<JsonRpcErrorCode>(e.get("code", 0).asInt());
        msg.error_.message = e.get("message", "").asString();
        msg.error_.data    = e.get("data", Json::nullValue);
        msg.valid_  = true;
    }

    return msg;
}

//////////////////////////////////////////////////////////////////////////////////////////
JsonRpcMessage::Type JsonRpcMessage::GetType() const { return type_; }
std::string JsonRpcMessage::GetMethod() const { return method_; }
Json::Value JsonRpcMessage::GetParams() const { return params_; }
Json::Value JsonRpcMessage::GetResult() const { return result_; }
JsonRpcError JsonRpcMessage::GetError() const { return error_; }
std::string JsonRpcMessage::GetId() const { return id_; }
bool JsonRpcMessage::IsValid() const { return valid_; }

//////////////////////////////////////////////////////////////////////////////////////////
void JsonRpcMessage::SetMethod(const std::string& _method) { method_ = _method; }

//////////////////////////////////////////////////////////////////////////////////////////
void JsonRpcMessage::SetParams(const Json::Value& _params) { params_ = _params; }

//////////////////////////////////////////////////////////////////////////////////////////
void JsonRpcMessage::SetId(const std::string& _id) { id_ = _id; }

//////////////////////////////////////////////////////////////////////////////////////////
std::string JsonRpcMessage::ToJson() const
{
    Json::Value root;
    root["jsonrpc"] = "2.0";

    switch (type_)
    {
    case Type::Request:
    case Type::Notification:
        root["method"] = method_;
        if (!params_.isNull())
            root["params"] = params_;
        if (type_ == Type::Request)
            root["id"] = id_;
        break;
    case Type::Response:
        root["result"] = result_;
        root["id"] = id_;
        break;
    case Type::Error:
        root["error"] = error_.ToJson();
        root["id"] = id_;
        break;
    }

    Json::StreamWriterBuilder writer;
    writer["emitUTF8"] = true;
    return Json::writeString(writer, root);
}

NS_MCP_END
