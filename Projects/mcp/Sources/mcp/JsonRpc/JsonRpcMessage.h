#pragma once

#include <mcp/JsonRpc/JsonRpcError.h>

NS_MCP_BEGIN

class JsonRpcMessage
{
public:
    enum class Type { Request, Notification, Response, Error };

    static JsonRpcMessage MakeRequest(const std::string& _method, const Json::Value& _params, const std::string& _id);
    static JsonRpcMessage MakeNotification(const std::string& _method, const Json::Value& _params);
    static JsonRpcMessage MakeResponse(const Json::Value& _result, const std::string& _id);
    static JsonRpcMessage MakeError(const JsonRpcError& _error, const std::string& _id);
    static JsonRpcMessage Parse(const std::string& _json);

    Type         GetType() const;
    std::string  GetMethod() const;
    Json::Value  GetParams() const;
    Json::Value  GetResult() const;
    JsonRpcError GetError() const;
    std::string  GetId() const;
    std::string  ToJson() const;
    bool         IsValid() const;

    void SetMethod(const std::string& _method);
    void SetParams(const Json::Value& _params);
    void SetId(const std::string& _id);

private:
    JsonRpcMessage();

    Type          type_;
    std::string   method_;
    Json::Value   params_;
    Json::Value   result_;
    JsonRpcError  error_;
    std::string   id_;
    bool          valid_;
};

NS_MCP_END
