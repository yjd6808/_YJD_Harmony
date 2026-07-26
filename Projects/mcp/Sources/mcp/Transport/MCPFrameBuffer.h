#pragma once

#include <vector>

NS_MCP_BEGIN

class MCPFrameBuffer
{
    static const int kHeaderSize = 4;

public:
    void Append(const char* _pData, int _len);
    bool TryExtractFrame(std::string& _outPayload);
    void Reset();
    int  GetBufferedSize() const;

    static std::string MakeFrame(const std::string& _payload);

private:
    std::vector<char> buffer_;
};

NS_MCP_END
