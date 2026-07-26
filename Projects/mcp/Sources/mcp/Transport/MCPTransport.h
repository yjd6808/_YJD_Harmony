#pragma once

#include <functional>
#include <jnet/Host/TcpSession.h>
#include <jnet/Host/TcpServer.h>
#include <mcp/Transport/MCPFrameBuffer.h>

NS_MCP_BEGIN

class MCPTransport : public jnet::TcpSession
{
public:
    using FrameCallback = std::function<void(MCPTransport*, std::string)>;
    using LifecycleCallback = std::function<void(MCPTransport*)>;

    MCPTransport(jnet::TcpServer* _pServer, const jnet::IOCPPtr& _pIocp, const jc::MemoryPoolAbstractPtr& _pAllocator);

    void SendFrame(const std::string& _payload);
    void SetFrameCallback(FrameCallback _cb);
    void SetConnectedCallback(LifecycleCallback _cb);
    void SetDisconnectedCallback(LifecycleCallback _cb);

protected:
    virtual void Received(_u32l _receivedBytes) override;
    virtual void OnConnected() override;
    virtual void OnDisconnected() override;

private:
    MCPFrameBuffer     frameBuffer_;
    FrameCallback      frameCallback_;
    LifecycleCallback  connectedCallback_;
    LifecycleCallback  disconnectedCallback_;
};

NS_MCP_END
