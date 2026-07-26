# MCP × jnet 코드 아키텍처

## 1. 계층별 의존성 방향

```
MCPServer  ←  MCPSession  ←  MCPTransport  ←  jnet::TcpServer
                                        └── jnet::TcpSession
                                        └── jnet::SessionContainer
```

- `MCPServer`가 `MCPTransport`를 소유
- `MCPTransport`가 `jnet::TcpServer`를 소유
- `jnet::TcpServer`가 `accept()` 시 `jnet::TcpSession` 생성
- 각 `TcpSession`에 `MCPFrameBuffer` 1:1 연결

---

## 2. MCPTransport — jnet 연동 상세

```cpp
// ─── MCPTransport.h ───
#pragma once
#include <jnet/Host/TcpServer.h>
#include <jnet/Host/TcpSession.h>
#include <jnet/EventListener/SessionEventListener.h>

class MCPFrameBuffer;

struct MCPTransportListener
{
    virtual ~MCPTransportListener() = default;
    virtual void OnConnected(_u32 sessionId) = 0;
    virtual void OnDisconnected(_u32 sessionId) = 0;
    virtual void OnMessage(_u32 sessionId, const jc::String& jsonMessage) = 0;
};

class MCPTransport : public jnet::SessionEventListener
{
public:
    // Mode A: 자체 IOCP + 블로킹
    void Run(const jnet::IPv4EndPoint& ep);

    // Mode B: 외부 IOCP 공유 + PollEvents
    void StartAsync(const jnet::IPv4EndPoint& ep, jnet::NetGroup* pExternalGroup = nullptr);
    void Stop();
    void PollEvents();     // 큐에 쌓인 메시지들을 리스너로 전달

    void SendMessage(_u32 sessionId, const jc::String& json);
    void BroadcastMessage(const jc::String& json);

    void SetListener(MCPTransportListener* pListener) { pListener_ = pListener; }

private:
    // SessionEventListener 구현 (jnet 콜백)
    void OnConnected(jnet::Session* pSession) override;
    void OnDisconnected(jnet::Session* pSession) override;
    void OnSent(jnet::Session* pSession, int bytes) override {}
    void OnReceivedRaw(jnet::Session* pSession, _u16 size, char* data) override;

    jnet::TcpServer*           pServer_ = nullptr;       // jnet TCP 서버
    jnet::SessionContainer*    pSessionContainer_ = nullptr;
    MCPTransportListener*      pListener_ = nullptr;

    // SessionId → FrameBuffer 매핑
    jc::HashMap<_u32, MCPFrameBuffer*> frameBuffers_;
    jc::NormalLock frameBuffersLock_;

    // Mode B: Polling 큐
    struct QueuedMessage { _u32 sessionId; jc::String json; };
    jc::ArrayQueue<QueuedMessage*> messageQueue_;
    jc::ArrayQueue<QueuedMessage*> swapQueue_;
    jc::NormalLock queueLock_;

    bool isPollingMode_ = false;
};
```

### 2.1 Mode A (Run) — 직접 콜백

```
IOCP Worker Thread
    │
    │ TcpSession::OnReceivedRaw() 호출
    │
    ▼
MCPTransport::OnReceivedRaw()
    │
    ├── frameBuffers_[sessionId]->Append(data, size)
    ├── while (message = frameBuffer.TryExtractMessage())
    │   └── pListener_->OnMessage(sessionId, message)  ← 같은 쓰레드에서 직접 호출
    │
    ▼
MCPSession::OnMessage() 에서 JSON-RPC 처리 + Tool 실행
    (모두 IOCP 워커스레드에서 실행)
```

### 2.2 Mode B (PollEvents) — 큐잉 후 메인스레드 처리

```
IOCP Worker Thread                          Main Thread (WorldScene::update)
    │                                                │
    │ OnReceivedRaw()                                │
    │  → FrameBuffer.Append()                       │
    │  → TryExtractMessage()                         │
    │  → Queue.Enqueue(sessionId, json)              │
    │                                                │
    │      ┌──────────────────────┐                  │
    │      │   MessageQueue       │                  │
    │      │   (thread-safe)      │                  │
    │      └──────────────────────┘                  │
    │                                                │
    │                                     MCPServer::PollEvents()
    │                                         │
    │                                     Lock + Swap Queue
    │                                         │
    │                                     while (queue not empty)
    │                                         pListener_->OnMessage()
    │                                             │
    │                                         MCPSession::OnMessage()
    │                                         → JSON-RPC 처리
    │                                         → Tool 실행 (메인스레드)
```

---

## 3. MCPFrameBuffer — TCP 프레이밍

```cpp
// ─── MCPFrameBuffer.h ───
#pragma once

// TCP 위에서 MCP 메시지를 length-prefixed JSON으로 프레이밍
//
// 송신 형식:
//   [4bytes: payload length (uint32, LE)]
//   [N bytes: UTF-8 JSON-RPC message]
//
// 수신 처리:
//   Append(data, len) 로 데이터를 내부 버퍼에 축적
//   TryExtractMessage() 로 완전한 메시지가 도착했는지 확인

class MCPFrameBuffer
{
    static const int HEADER_SIZE = 4;

public:
    void Append(const char* data, int len);
    
    // 완전한 메시지가 있으면 true 반환, outMessage에 JSON 문자열 저장
    bool TryExtractMessage(jc::String& outMessage);

    // 송신용: JSON 문자열 → length-prefixed 바이너리
    static void FrameMessage(const jc::String& json, jc::Vector<char>& outBytes);

private:
    jc::Vector<char> buffer_;    // 수신 버퍼 (raw bytes)
    int readPos_ = 0;
};
```

---

## 4. MCPSession — 연결별 프로토콜 처리

```cpp
// ─── MCPSession.h ───
#pragma once

class MCPSession : public MCPTransportListener
{
public:
    void Bind(_u32 sessionId, MCPTransport* pTransport, MCPToolRegistry* pTools);
    void Unbind();

    // MCP 프로토콜 핸들러 (외부에서 메시지 주입)
    void HandleMessage(const jc::String& jsonMessage);

private:
    // MCP handshake
    void HandleInitialize(const Json::Value& params, _u64 msgId);
    void HandlePing(_u64 msgId);
    void HandleToolsList(_u64 msgId);
    void HandleToolsCall(const Json::Value& params, _u64 msgId);

    // 응답 전송
    void SendResult(_u64 msgId, const Json::Value& result);
    void SendError(_u64 msgId, int code, const char* message);
    void SendNotification(const char* method, const Json::Value& params);

    _u32 sessionId_ = 0;
    MCPTransport* pTransport_ = nullptr;
    MCPToolRegistry* pToolRegistry_ = nullptr;

    enum State { Connected, Initializing, Ready, Disconnected };
    State state_ = Connected;
};
```

### 프로토콜 처리 흐름

```cpp
void MCPSession::HandleMessage(const jc::String& jsonMessage)
{
    Json::Value root;
    Json::Reader reader;
    if (!reader.parse(jsonMessage, root))
    {
        SendError(0, -32700, "Parse error");  // JSON-RPC ParseError
        return;
    }

    // 필수 필드 검사
    if (!root.isMember("method"))
    {
        SendError(0, -32600, "Invalid Request");
        return;
    }

    jc::String method = root["method"].asCString();
    _u64 msgId = root.get("id", Json::Value::null).asUInt64();
    bool isNotification = !root.isMember("id");
    Json::Value params = root["params"];

    // method dispatch
    if (method == "initialize")         HandleInitialize(params, msgId);
    else if (method == "ping")          HandlePing(msgId);
    else if (method == "tools/list")    HandleToolsList(msgId);
    else if (method == "tools/call")    HandleToolsCall(params, msgId);
    else if (method == "notifications/initialized")
    {
        state_ = Ready;  // 클라이언트 준비 완료
        return;          // notification은 응답 없음
    }
    else SendError(msgId, -32601, "Method not found");
}
```

---

## 5. MCPServer — 퍼블릭 API (두 모드)

```cpp
// ─── MCPServer.h ───
#pragma once

class MCPServer
{
public:
    // Factory
    static MCPServer* Create();
    void Destroy();

    // Mode A: Worker Thread (자체 IOCP, 블로킹)
    //   mcpd 용도: Run() 호출 시 Ctrl+C까지 블로킹
    void Run(int port);

    // Mode B: Polling (싱글스레드 호스트)
    //   sgcl 용도: Start() 후 매 프레임 PollEvents() 호출
    void Start(int port);
    void Stop();
    void PollEvents();

    // Tool 등록
    void RegisterTool(const MCPTool& tool);

    _u32 GetSessionCount() const;

private:
    MCPServer();
    ~MCPServer();

    MCPTransport*       pTransport_ = nullptr;
    MCPToolRegistry*    pToolRegistry_ = nullptr;
    MCPSession*         pSession_ = nullptr;   // 단일 세션 관리 (첫 연결)
};
```

### 사용 예시

```cpp
// ─── mcpd/main.cpp (Mode A) ───
#include <jnet/Config.h>
#include <mcp/Protocol/MCPServer.h>

int main(int argc, char** argv)
{
    jnet::Config::InitializeJNet(argc, argv);

    MCPServer* pServer = MCPServer::Create();
    pServer->RegisterTool(MCPTool("echo", "...", handler));
    pServer->RegisterTool(MCPTool("info", "...", handler));

    pServer->Run(9100);         // ← 블로킹, 자체 IOCP 스레드

    pServer->Destroy();
    jnet::Config::FinalizeJNet();
    return 0;
}
```

```cpp
// ─── sgcl 내장 (Mode B) ───
// WorldScene.h
class WorldScene {
    MCPServer* pMCPServer_;
};

// WorldScene::init()
pMCPServer_ = MCPServer::Create();
pMCPServer_->RegisterTool(MCPTool("get_state", "...", handler));
pMCPServer_->Start(9100);        // ← non-blocking

// WorldScene::UpdateNet()
void WorldScene::UpdateNet(float _dt)
{
    g_cNet.PollNetEvents();       // 기존 게임 네트워크
    pMCPServer_->PollEvents();    // MCP 메시지 큐 처리 (메인스레드)
}

// WorldScene::~WorldScene()
pMCPServer_->Stop();
pMCPServer_->Destroy();
```

---

## 6. MCPTool — Tool 정의

```cpp
// ─── MCPTool.h ───
#pragma once

struct MCPToolResult
{
    Json::Value content;    // [{type: "text", text: "..."}]
    bool isError = false;

    static MCPToolResult Text(const char* text);
    static MCPToolResult Error(const char* msg);
};

using MCPToolHandler = std::function<MCPToolResult(const Json::Value& args)>;

class MCPTool
{
public:
    MCPTool() = default;
    MCPTool(const char* name, const char* description,
            Json::Value inputSchema, MCPToolHandler handler);

    Json::Value ToJson() const;  // tools/list 응답용

    const jc::String& GetName() const { return name_; }
    MCPToolResult Call(const Json::Value& args) const;

private:
    jc::String name_;
    jc::String description_;
    Json::Value inputSchema_;
    MCPToolHandler handler_;
};
```

---

## 7. 의존성 그래프 (코드 레벨)

```
mcpd.exe
  └── main.cpp
       ├── #include <jnet/Config.h>
       ├── #include <mcp/Protocol/MCPServer.h>     ← 퍼블릭 API만 include
       └── link: mcp.lib + jnet.lib + jc.lib + jsoncpp.lib

mcp.lib
  ├── MCPServer.h/.cpp
  │    └── #include <mcp/Protocol/MCPSession.h>
  │
  ├── MCPSession.h/.cpp
  │    ├── #include <mcp/Transport/MCPTransport.h>
  │    ├── #include <mcp/JsonRpc/JsonRpcMessage.h>
  │    ├── #include <mcp/JsonRpc/JsonRpcDispatcher.h>
  │    └── #include <mcp/Tools/MCPToolRegistry.h>
  │
  ├── MCPTransport.h/.cpp
  │    ├── #include <jnet/Host/TcpServer.h>           ← jnet 직접 사용
  │    ├── #include <jnet/Host/TcpSession.h>
  │    ├── #include <jnet/EventListener/SessionEventListener.h>
  │    ├── #include <jnet/NetGroup.h>
  │    └── #include <mcp/Transport/MCPFrameBuffer.h>
  │
  ├── MCPFrameBuffer.h/.cpp
  │    └── #include <jc/Container/Vector.h>           ← jc만 사용
  │
  ├── MCPMessageQueue.h/.cpp
  │    ├── #include <jc/Container/ArrayQueue.h>
  │    └── #include <jc/Sync/NormalLock.h>
  │
  ├── MCPToolRegistry.h/.cpp
  │    ├── #include <mcp/Tools/MCPTool.h>
  │    └── #include <jc/Container/HashMap.h>
  │
  ├── JsonRpcMessage.h/.cpp
  │    ├── #include <json/value.h>                ← JsonCpp
  │    └── #include <json/reader.h>
  │
  └── JsonRpcDispatcher.h/.cpp
       ├── #include <mcp/JsonRpc/JsonRpcMessage.h>
       └── #include <jc/Container/HashMap.h>
```

---

## 8. jnet 클래스 활용 요약

| jnet 클래스 | mcp 내 사용 | 역할 |
|---|---|---|
| `jnet::TcpServer` | `MCPTransport` 멤버 | TCP 연결 수락, 세션 생성 |
| `jnet::TcpSession` | `MCPTransport`가 소유 | accept된 각 연결, 데이터 송수신 |
| `jnet::SessionContainer` | `MCPTransport` 멤버 | 세션 ID 생성/관리/조회 |
| `jnet::SessionEventListener` | `MCPTransport`가 상속 | jnet → mcp 콜백 (OnConnected, OnReceivedRaw 등) |
| `jnet::NetGroup` | `MCPTransport::StartAsync()` 외부 주입 | IOCP + 버퍼풀 공유 (Mode B) |
| `jnet::IPv4EndPoint` | `MCPTransport::Run/StartAsync` 파라미터 | 서버 주소+포트 |
| `jnet::Config` | `mcpd::main()` | jnet 초기화/종료 |
| `jnet::SendHelper` | (미사용) | MCP는 자체 송신 로직 |

### 핵심 인터페이스: SessionEventListener

jnet이 세션 이벤트를 mcp에 전달하는 유일한 통로:

```cpp
class SessionEventListener
{
    virtual void OnConnected(Session* pSession);
    virtual void OnDisconnected(Session* pSession);
    virtual void OnSent(Session* pSession, int bytes);
    virtual void OnReceivedRaw(Session* pSession, _u16 size, char* data);
    virtual void OnReceivedCmd(Session* pSession, ICommand* pCmd);
    virtual void OnReceivedMsg(Session* pSession, CMessage* pMsg);
    virtual void OnReceivedPacket(Session* pSession, RecvedPacket* pPacket);
};
```

MCPTransport는 `OnReceivedRaw`만 구현:
- TCP raw 데이터 수신 → `MCPFrameBuffer::Append()` → `TryExtractMessage()`
- Mode A: 리스너 직접 호출
- Mode B: 큐에 enqueue
