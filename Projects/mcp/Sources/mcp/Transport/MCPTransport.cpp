#include <mcp/PCH/Core.h>
#include <mcp/Transport/MCPFrameBuffer.h>
#include <mcp/Transport/MCPTransport.h>
#include <jnet/Packet/Packet.h>

USING_NS_JNET;

NS_MCP_BEGIN

//////////////////////////////////////////////////////////////////////////////////////////
MCPTransport::MCPTransport(
    jnet::TcpServer* _pServer,
    const jnet::IOCPPtr& _pIocp,
    const jc::MemoryPoolAbstractPtr& _pAllocator)
    : TcpSession(_pServer, _pIocp, _pAllocator, nullptr, 0, 0)
{
}

//////////////////////////////////////////////////////////////////////////////////////////
void MCPTransport::SendFrame(const std::string& _payload)
{
    std::string frame = MCPFrameBuffer::MakeFrame(_payload);
    Send(frame.data(), static_cast<int>(frame.size()));
}

//////////////////////////////////////////////////////////////////////////////////////////
void MCPTransport::SetFrameCallback(FrameCallback _cb) { frameCallback_ = std::move(_cb); }

//////////////////////////////////////////////////////////////////////////////////////////
void MCPTransport::SetConnectedCallback(LifecycleCallback _cb) { connectedCallback_ = std::move(_cb); }

//////////////////////////////////////////////////////////////////////////////////////////
void MCPTransport::SetDisconnectedCallback(LifecycleCallback _cb) { disconnectedCallback_ = std::move(_cb); }

//////////////////////////////////////////////////////////////////////////////////////////
void MCPTransport::Received(_u32l _receivedBytes)
{
    PacketBuffer* pBuf = GetRecvBuffer().GetPtr();
    pBuf->MoveWritePos(static_cast<int>(_receivedBytes));

    WSABUF wsa = pBuf->GetReadableBuffer();
    if (wsa.len > 0)
    {
        frameBuffer_.Append(wsa.buf, wsa.len);
        pBuf->ResetPosition();

        std::string payload;
        while (frameBuffer_.TryExtractFrame(payload))
        {
            if (frameCallback_)
                frameCallback_(this, std::move(payload));
            payload.clear();
        }
    }
}

//////////////////////////////////////////////////////////////////////////////////////////
void MCPTransport::OnConnected()
{
    if (connectedCallback_)
        connectedCallback_(this);
}

//////////////////////////////////////////////////////////////////////////////////////////
void MCPTransport::OnDisconnected()
{
    frameBuffer_.Reset();
    if (disconnectedCallback_)
        disconnectedCallback_(this);
}

NS_MCP_END
