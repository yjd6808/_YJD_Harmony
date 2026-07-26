#include <mcp/PCH/Core.h>
#include <mcp/Transport/MCPFrameBuffer.h>

NS_MCP_BEGIN

//////////////////////////////////////////////////////////////////////////////////////////
void MCPFrameBuffer::Append(const char* _pData, int _len)
{
    buffer_.insert(buffer_.end(), _pData, _pData + _len);
}

//////////////////////////////////////////////////////////////////////////////////////////
bool MCPFrameBuffer::TryExtractFrame(std::string& _outPayload)
{
    if (buffer_.size() < kHeaderSize)
        return false;

    int payloadLen = 0;
    payloadLen |= (static_cast<unsigned char>(buffer_[0]) << 24);
    payloadLen |= (static_cast<unsigned char>(buffer_[1]) << 16);
    payloadLen |= (static_cast<unsigned char>(buffer_[2]) << 8);
    payloadLen |= static_cast<unsigned char>(buffer_[3]);

    if (payloadLen <= 0 || payloadLen > 1024 * 1024)
    {
        Reset();
        return false;
    }

    int totalSize = kHeaderSize + payloadLen;
    if (buffer_.size() < totalSize)
        return false;

    _outPayload.assign(buffer_.begin() + kHeaderSize, buffer_.begin() + totalSize);
    buffer_.erase(buffer_.begin(), buffer_.begin() + totalSize);
    return true;
}

//////////////////////////////////////////////////////////////////////////////////////////
void MCPFrameBuffer::Reset()
{
    buffer_.clear();
}

//////////////////////////////////////////////////////////////////////////////////////////
int MCPFrameBuffer::GetBufferedSize() const
{
    return static_cast<int>(buffer_.size());
}

//////////////////////////////////////////////////////////////////////////////////////////
std::string MCPFrameBuffer::MakeFrame(const std::string& _payload)
{
    int len = static_cast<int>(_payload.size());
    std::string frame;
    frame.reserve(kHeaderSize + len);
    frame.push_back(static_cast<char>((len >> 24) & 0xFF));
    frame.push_back(static_cast<char>((len >> 16) & 0xFF));
    frame.push_back(static_cast<char>((len >> 8) & 0xFF));
    frame.push_back(static_cast<char>(len & 0xFF));
    frame += _payload;
    return frame;
}

NS_MCP_END
