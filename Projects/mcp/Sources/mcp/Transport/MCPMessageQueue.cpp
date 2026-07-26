#include <mcp/PCH/Core.h>
#include <mcp/Transport/MCPMessageQueue.h>

NS_MCP_BEGIN

//////////////////////////////////////////////////////////////////////////////////////////
void MCPMessageQueue::Enqueue(std::string _message)
{
    std::lock_guard<std::mutex> lock(mutex_);
    messages_.push(std::move(_message));
}

//////////////////////////////////////////////////////////////////////////////////////////
bool MCPMessageQueue::TryDequeue(std::string& _outMessage)
{
    std::lock_guard<std::mutex> lock(mutex_);
    if (messages_.empty())
        return false;
    _outMessage = std::move(messages_.front());
    messages_.pop();
    return true;
}

//////////////////////////////////////////////////////////////////////////////////////////
void MCPMessageQueue::Swap(std::queue<std::string>& _outQueue)
{
    std::lock_guard<std::mutex> lock(mutex_);
    std::swap(messages_, _outQueue);
}

//////////////////////////////////////////////////////////////////////////////////////////
int MCPMessageQueue::Count() const
{
    std::lock_guard<std::mutex> lock(mutex_);
    return static_cast<int>(messages_.size());
}

NS_MCP_END
