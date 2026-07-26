#include <mcp/PCH/Core.h>
#include <mcp/Protocol/MCPConstants.h>
#include <mcp/Protocol/MCPCapabilities.h>
#include <mcp/Protocol/MCPServer.h>
#include <mcp/Protocol/MCPSession.h>
#include <mcp/Transport/MCPTransport.h>
#include <mcp/Transport/MCPMessageQueue.h>
#include <mcp/Tools/MCPToolRegistry.h>

NS_MCP_BEGIN

//////////////////////////////////////////////////////////////////////////////////////////
MCPSession::MCPSession(MCPServer* _pServer, MCPTransport* _pTransport)
    : server_(_pServer)
    , transport_(_pTransport)
    , recvQueue_(std::make_unique<MCPMessageQueue>())
    , initialized_(false)
{
    dispatcher_.Register(MCPProtocol::Methods::kInitialize, [this](const Json::Value& _params) -> Json::Value {
        JsonRpcMessage req = JsonRpcMessage::MakeRequest("", _params, "");
        return HandleInitialize(req).GetResult();
    });

    dispatcher_.Register(MCPProtocol::Methods::kListTools, [this](const Json::Value&) -> Json::Value {
        return HandleListTools(Json::Value());
    });

    dispatcher_.Register(MCPProtocol::Methods::kCallTool, [this](const Json::Value& _params) -> Json::Value {
        return HandleCallTool(_params);
    });
}

//////////////////////////////////////////////////////////////////////////////////////////
MCPSession::~MCPSession()
{
}

//////////////////////////////////////////////////////////////////////////////////////////
MCPMessageQueue* MCPSession::GetRecvQueue()
{
    return recvQueue_.get();
}

//////////////////////////////////////////////////////////////////////////////////////////
void MCPSession::Send(const std::string& _payload)
{
    transport_->SendFrame(_payload);
}

//////////////////////////////////////////////////////////////////////////////////////////
void MCPSession::Disconnect()
{
    transport_->SendFrame("{}");
}

//////////////////////////////////////////////////////////////////////////////////////////
void MCPSession::ProcessMessages()
{
    std::queue<std::string> messages;
    recvQueue_->Swap(messages);

    while (!messages.empty())
    {
        std::string raw = std::move(messages.front());
        messages.pop();

        JsonRpcMessage msg = JsonRpcMessage::Parse(raw);
        if (!msg.IsValid())
            continue;

        if (msg.GetType() == JsonRpcMessage::Type::Notification)
        {
            if (msg.GetMethod() == MCPProtocol::Methods::kNotificationsInitialized)
                initialized_ = true;
            continue;
        }

        JsonRpcMessage response = dispatcher_.Dispatch(msg);
        Send(response.ToJson());
    }
}

//////////////////////////////////////////////////////////////////////////////////////////
JsonRpcMessage MCPSession::HandleInitialize(const JsonRpcMessage& _request)
{
    Json::Value result;
    result["protocolVersion"] = MCPProtocol::kVersion;
    result["capabilities"]    = server_->GetCapabilities().ToJson();
    result["serverInfo"]["name"]    = MCPProtocol::kProtocolName;
    result["serverInfo"]["version"] = MCPProtocol::kVersion;
    return JsonRpcMessage::MakeResponse(result, _request.GetId());
}

//////////////////////////////////////////////////////////////////////////////////////////
Json::Value MCPSession::HandleListTools(const Json::Value&)
{
    Json::Value result;
    result["tools"] = server_->GetToolRegistry().ListAll();
    return result;
}

//////////////////////////////////////////////////////////////////////////////////////////
Json::Value MCPSession::HandleCallTool(const Json::Value& _params)
{
    std::string name  = _params.get("name", "").asString();
    Json::Value args  = _params.get("arguments", Json::Value());

    MCPTool* pTool = server_->GetToolRegistry().Find(name);
    if (!pTool)
        throw std::runtime_error("Tool not found: " + name);

    return pTool->handler(args);
}

NS_MCP_END
