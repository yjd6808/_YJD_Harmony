#pragma once

#include <mutex>
#include <queue>

NS_MCP_BEGIN

class MCPMessageQueue
{
public:
    void Enqueue(std::string _message);
    bool TryDequeue(std::string& _outMessage);
    void Swap(std::queue<std::string>& _outQueue);
    int  Count() const;

private:
    mutable std::mutex   mutex_;
    std::queue<std::string> messages_;
};

NS_MCP_END
