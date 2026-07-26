# MCP (Model Context Protocol) Server for UltimateHarmony

## 개요

MCP(Model Context Protocol) Server를 jnet 네트워크 라이브러리 기반으로 구현한다.
MCP는 AI 애플리케이션(Claude, OpenCode 등)이 외부 시스템과 통신하기 위한 JSON-RPC 2.0 기반 프로토콜이다.

## 목표

- **jnet**의 IOCP 기반 TcpServer/TcpSession 활용
- **JsonCpp**(ThirdParty)로 JSON-RPC 2.0 메시지 처리
- TCP transport 위에서 MCP 프로토콜 구현
- Tool 실행 시스템 구축

## 브랜치

- `dev-mcp` (sg-dev 기반, 바탕화면 `UltimateHarmony-dev-mcp` 워크트리)

## 빌드

- **Platform**: x64 전용 (x86 미지원)
- **Configuration**: Debug / Release
- **명령**: `cd Scripts; .\Build.ps1 -ProjectName <project> -C Debug -P x64 -Rebuild`
- 빌드 순서: `jc` → `jnet` → `jsoncpp` → `mcp` → `mcpd`

---

## 아키텍처

```
┌─────────────────────────────────────────────────────────────┐
│                       mcpd.exe                               │
│  (main.cpp: MCPServer 생성, Tool 등록, 실행)                │
├─────────────────────────────────────────────────────────────┤
│                    mcp.lib (MCP Protocol Library)             │
│                                                              │
│  ┌───────────────────────────────────────────────────────┐   │
│  │                 MCP Protocol Layer                    │   │
│  │  ┌──────────────┐ ┌──────────────┐ ┌─────────────┐   │   │
│  │  │  MCPServer   │ │  MCPSession  │ │Capabilities │   │   │
│  │  │  (연결관리)   │ │  (프로토콜)   │ │(협상)       │   │   │
│  │  └──────┬───────┘ └──────┬───────┘ └─────────────┘   │   │
│  │         └────────┬───────┘                            │   │
│  │                  ▼                                    │   │
│  │  ┌────────────────────────────────────────────────┐   │   │
│  │  │         MCP Tool System                        │   │   │
│  │  │  MCPTool / MCPToolRegistry / BuiltinTools     │   │   │
│  │  └────────────────────────────────────────────────┘   │   │
│  └───────────────────────────────────────────────────────┘   │
│                            │                                  │
│  ┌───────────────────────────────────────────────────────┐   │
│  │              JSON-RPC 2.0 Layer                       │   │
│  │  JsonRpcMessage (Req/Res/Notify/Error)               │   │
│  │  + JsonRpcDispatcher (method → handler mapping)      │   │
│  └───────────────────────────────────────────────────────┘   │
│                            │                                  │
│  ┌───────────────────────────────────────────────────────┐   │
│  │           TCP Transport (jnet 기반)                    │   │
│  │  jnet::TcpServer + jnet::TcpSession                   │   │
│  │  MCPFrameBuffer: Length-prefixed JSON framing         │   │
│  │  [4byte length][JSON-RPC UTF-8 message]              │   │
│  └───────────────────────────────────────────────────────┘   │
└─────────────────────────────────────────────────────────────┘
```

### 전송 계층 (TCP Framing)

MCP 표준은 stdio 또는 HTTP/SSE를 사용하지만, jnet이 TCP 기반이므로
**Length-prefixed JSON** 프레이밍을 사용한다:

```
[4bytes: payload length (uint32, LE)] [N bytes: JSON-RPC UTF-8 message]
```

### 두 가지 실행 모드

라이브러리는 호스트의 쓰레드 모델에 따라 **2가지 모드**로 동작해야 한다.

#### Mode A: Worker Thread 모드 (`Run()`)
- **용도**: mcpd 같은 standalone 서버
- jnet TcpServer가 자체 NetGroup/IOCP 생성
- IOCP 워커스레드가 I/O + 프로토콜 처리까지 전담
- `Run()` 호출 시 블로킹 (내부적으로 `IOCP::Join()`)
- Tool 핸들러는 워커스레드에서 실행되므로 **스레드 안전성 필요**

```
mcpd main()
  └── MCPServer::Run(port)
       └── MCPTransport::Start()
            ├── jnet::TcpServer (자체 IOCP 생성)
            ├── IOCP Worker 1 ── MCP I/O + Protocol 처리
            ├── IOCP Worker 2 ── MCP I/O + Protocol 처리
            └── Run() 블로킹 (IOCP::Join())
```

#### Mode B: Polling 모드 (`Start()` + `PollEvents()`)
- **용도**: sgcl 같은 싱글스레드 클라이언트
- jnet TcpServer는 기존/외부 NetGroup에 소속되어 IOCP 공유
- IOCP 워커스레드는 **I/O만 수행**, 수신 메시지는 **명령 큐**에 enqueue
- 메인스레드가 매 프레임 `PollEvents()` 호출해서 큐를 비우고 프로토콜 처리
- Tool 핸들러는 **메인스레드**에서 실행 (CommandSynchronizer 패턴과 동일)

```
sgcl WorldScene::update(_dt)
  ├── UpdateNet(_dt)
  │    ├── g_cNet.PollNetEvents()      ← 기존 네트워크 메시지 처리
  │    └── pMCPServer->PollEvents()    ← MCP 메시지 처리 (신규)
  ├── UpdateTime(_dt)
  └── UpdateScene(_dt)

MCPServer::Start(port)
  └── MCPTransport::StartAsync()
       ├── jnet::TcpServer (외부 NetGroup/IOCP 공유)
       ├── IOCP Worker 1 ── I/O only → 명령 큐에 enqueue
       ├── IOCP Worker 2 ── I/O only → 명령 큐에 enqueue
       └── 즉시 반환 (non-blocking)

MCPServer::PollEvents()         ← 메인스레드가 매 프레임 호출
  └── 명령 큐 dequeue → MCPSession::ProcessMessage() → Tool 실행
```

### 메시지 큐잉 구조 (Mode B)

```
IOCP Worker Thread                   Main Thread (cocos2d)
       │                                    │
       │  TCP data received                  │
       │  MCPFrameBuffer::Append()           │
       │  MCPFrameBuffer::TryExtract()       │
       │       │                              │
       │  EnqueueMessage(sessionId, json)     │
       │       │                              │
       │  ┌────▼─────────────────┐            │
       │  │   MessageQueue       │            │
       │  │   (thread-safe)      │            │
       │  └──────────────────────┘            │
       │                           ┌─────────▲
       │                           │
       │                    PollEvents() 호출
       │                           │
       │                    DequeueAll → Process
```

---

## 디렉토리 구조 및 파일 목록

### 신규 프로젝트: `mcp` (Static Library)

```
Projects/mcp/
├── Project/
│   ├── mcp.vcxproj
│   └── mcp.vcxproj.filters
└── Sources/
    └── mcp/
        ├── PCH/
        │   ├── Core.h
        │   └── Core.cpp
        ├── JsonRpc/
        │   ├── JsonRpcMessage.h          # JSON-RPC 2.0 메시지 타입
        │   ├── JsonRpcMessage.cpp
        │   ├── JsonRpcDispatcher.h       # 메서드 디스패처
        │   ├── JsonRpcDispatcher.cpp
        │   └── JsonRpcError.h            # 에러 코드 정의
        ├── Transport/
        │   ├── MCPFrameBuffer.h          # Length-prefixed TCP 프레이밍
        │   ├── MCPFrameBuffer.cpp
        │   ├── MCPTransport.h            # jnet TcpServer+TcpSession 통합
        │   └── MCPTransport.cpp
        ├── Protocol/
        │   ├── MCPConstants.h            # 프로토콜 상수/메서드명
        │   ├── MCPSession.h              # 연결별 프로토콜 처리
        │   ├── MCPSession.cpp
        │   ├── MCPServer.h               # 서버 생명주기 관리
        │   ├── MCPServer.cpp
        │   ├── MCPCapabilities.h          # Capability 협상
        │   └── MCPCapabilities.cpp
        └── Tools/
            ├── MCPTool.h                 # Tool 정의/핸들러
            ├── MCPTool.cpp
            ├── MCPToolRegistry.h         # Tool 등록/검색
            ├── MCPToolRegistry.cpp
            ├── MCPBuiltinTools.h         # 내장 Tool 구현
            └── MCPBuiltinTools.cpp
```

### 신규 프로젝트: `mcpd` (Executable)

```
Projects/mcpd/
├── Project/
│   ├── mcpd.vcxproj
│   └── mcpd.vcxproj.filters
└── main.cpp
```

### 수정 파일

| 파일 | 수정 내용 |
|------|-----------|
| `UltimateHarmony.sln` | mcp + mcpd 프로젝트 추가, 빌드 순서 설정 |
| `Scripts/Build.ps1` | (확인 필요) |

---

## Phase 별 상세 작업

### Phase 1: 프로젝트 셋업 (Step 1)

**1.1** `Projects/mcp/Project/mcp.vcxproj` 생성
  - StaticLibrary, v143, x64 only
  - C++20 (`stdcpplatest`), UTF-8
  - PCH: `PCH/Core.h` (Use/Create)
  - Include 경로: `$(SolutionDir)Projects\mcp\Sources`, `$(SolutionDir)Projects\jc\Sources`, `$(SolutionDir)Projects\jnet\Sources`, `$(SolutionDir)ThirdParty\JsonCpp195\include`
  - 링크: `jnet.lib`, `jc.lib`, `jsoncpp.lib`, `ws2_32.lib`, `Mswsock.lib`

**1.2** `Projects/mcp/Project/mcp.vcxproj.filters` 생성
  - 필터: `PCH`, `JsonRpc`, `Transport`, `Protocol`, `Tools`

**1.3** `Projects/mcp/Sources/mcp/PCH/Core.h/cpp` 생성
  - Core.h: `jc/PCH/Core.h`, `jnet/PCH/Core.h`, `<json/json.h>`, `<memory>`, `<string>` 등
  - Core.cpp: `#include "Core.h"`

**1.4** `Projects/mcpd/Project/mcpd.vcxproj` 생성
  - ConsoleApplication, v143, x64 only
  - mcp.lib 링크

**1.5** `Projects/mcpd/Project/mcpd.vcxproj.filters` 생성

**1.6** 솔루션 `.sln` 파일에 mcp, mcpd 프로젝트 추가
  - 빌드 순서: jc → jnet → jsoncpp(?) → mcp → mcpd

> **Verify**: `Build.ps1 -ProjectName mcp -C Debug -P x64` 빌드 성공 (PCH only)

### Phase 2: JSON-RPC 2.0 코어 (Step 2)

**2.1** `JsonRpcError.h` — JSON-RPC 2.0 표준 에러 코드
  - ParseError(-32700), InvalidRequest(-32600), MethodNotFound(-32601), InvalidParams(-32602), InternalError(-32603)
  - MCPErrorCode 커스텀 에러 범위

**2.2** `JsonRpcMessage.h/.cpp` — JSON-RPC 메시지 모델
  - `JsonRpcMessage` 기본 클래스 (jsonrpc version)
  - `JsonRpcRequest` (id, method, params)
  - `JsonRpcResponse` (id, result)
  - `JsonRpcErrorResponse` (id, code, message, data)
  - `JsonRpcNotification` (method, params)
  - 각각 `Serialize()` / `Deserialize()` 메서드 (Json::Value ↔ class)

**2.3** `JsonRpcDispatcher.h/.cpp` — 메서드 디스패처
  - `RegisterHandler(method, handler)` — 핸들러 등록
  - `Dispatch(request)` — 요청 → 핸들러 실행 → 응답 생성
  - 핸들러 시그니처: `JsonRpcResponse(const JsonRpcRequest&)`
  - Notification은 반환 없음

> **Verify**: 개별 단위 빌드

### Phase 3: TCP 전송 계층 (Step 3)

**3.1** `MCPFrameBuffer.h/.cpp` — TCP 프레이밍
  - `Append(data, len)` — 수신 데이터 추가
  - `TryExtractMessage()` — 완전한 메시지 추출 (length prefix 확인)
  - `FrameMessage(jsonStr)` — 송신용 메시지 프레이밍 (4byte length + json)

**3.2** `MCPMessageQueue.h/.cpp` — 스레드 안전 메시지 큐 (Pooling 모드용)
  - `MCPQueuedMessage`: sessionId + json 문자열을 담는 홀더
  - Lock 기반 메시지 큐 (NormalLock + ArrayQueue)
  - `Enqueue(sessionId, json)` — IOCP 워커스레드 호출
  - `DequeueAll(handler)` — 메인스레드 호출, 큐를 swap하여 전체 처리 (CommandSynchronizer 패턴)

**3.3** `MCPTransport.h/.cpp` — jnet 기반 TCP 전송
  - `MCPTransportListener` 인터페이스: `OnConnected`, `OnDisconnected`, `OnMessage`, `OnError`
  - 내부적으로 `jnet::TcpServer` + `jnet::SessionContainer` 사용
  - 각 `jnet::TcpSession`에 `MCPFrameBuffer` 연결
  - **Mode A (Worker Thread)**: 수신 데이터 → FrameBuffer → 완전한 메시지 → 리스너 직접 통지 (같은 쓰레드)
  - **Mode B (Polling)**: 수신 데이터 → FrameBuffer → 완전한 메시지 → `MCPMessageQueue::Enqueue()` (큐잉)
  - `SendMessage(sessionId, jsonStr)` — 메시지 송신
  - `PollEvents()` — 큐에 쌓인 메시지들을 리스너로 디스패치

> **Verify**: mcpd 테스트로 TCP 연결/메시지 송수신 확인

### Phase 4: MCP 프로토콜 코어 (Step 4)

**4.1** `MCPConstants.h` — MCP 프로토콜 상수
  - 메서드명: `initialize`, `ping`, `tools/list`, `tools/call`, `resources/list`, `resources/read`, `prompts/list`, `prompts/get`
  - 알림명: `notifications/initialized`, `notifications/tools/list_changed`
  - MCP 전용 에러 코드

**4.2** `MCPCapabilities.h/.cpp` — Capability 협상
  - `ServerCapabilities`: 지원 tools, resources, prompts, logging 등
  - `ClientCapabilities`: 클라이언트가 지원하는 기능
  - `InitResult`: `Initialize()` 결과 (protocolVersion, capabilities, serverInfo)

**4.3** `MCPSession.h/.cpp` — 연결별 세션
  - MCPTransport 리스너 구현
  - 상태 관리: Connected → Initializing → Ready → Disconnected
  - JSON-RPC 메시지 수신 → MCPConstants에 따라 디스패치
  - Initialize 핸들링, Ping/Pong, 에러 처리

**4.4** `MCPServer.h/.cpp` — 서버 관리자
  - `Run(port)` — **Mode A**: 자체 IOCP 생성, Run 내부 블로킹 (mcpd용)
  - `Start(port)` — **Mode B**: IOCP 생성, 메시지는 큐에 적재, 즉시 반환 (sgcl용)
  - `Stop()` — 서버 종료
  - `PollEvents()` — **Mode B 전용**: 큐에 쌓인 MCP 메시지 처리 (메인스레드가 매 프레임 호출)
  - `RegisterTool(name, handler)` — tool 등록
  - `GetSessionCount()` — 연결된 세션 수
  - MCPSession 생성/관리

> **Verify**: mcpd로 initialize/ping 테스트

### Phase 5: Tool 시스템 (Step 5)

**5.1** `MCPTool.h/.cpp` — Tool 정의
  - `MCPToolSchema`: JSON Schema 기반 input/output 정의
  - `MCPTool`: name, description, inputSchema, handler
  - 핸들러: `Json::Value(const Json::Value& arguments)`
  - `MCPToolResult`: content(array), isError(bool)

**5.2** `MCPToolRegistry.h/.cpp` — Tool 레지스트리
  - `RegisterTool(tool)` — tool 등록
  - `ListTools()` → `tools/list` 응답 생성
  - `CallTool(name, args)` → `tools/call` 실행

**5.3** `MCPBuiltinTools.h/.cpp` — 내장 Tool
  - `echo`: 입력을 그대로 반환 (테스트용)
  - `info`: 서버 정보 반환
  - `ping`: 지연 시간 측정

> **Verify**: tools/list 및 tools/call echo 테스트

### Phase 6: 서버 데몬 + sgcl 연동 예제 (Step 6)

**6.1** `mcpd/main.cpp` — mcpd 진입점 (Mode A: Run)
```cpp
jnet::Config::InitializeJNet(argc, argv);
MCPServer* pServer = MCPServer::Create();
pServer->RegisterTool(/* echo */);
pServer->RegisterTool(/* info */);
pServer->Run(9100);     // ← 블로킹, 자체 IOCP 스레드
jnet::Config::FinalizeJNet();
```

**6.2** `mcpd/main_poll.cpp` — sgcl 연동 예제 (Mode B: Start + PollEvents)
```cpp
// WorldScene::init()에서
pMCPServer_ = MCPServer::Create();
pMCPServer_->RegisterTool(/* echo */);
pMCPServer_->Start(9100);  // ← non-blocking, 즉시 반환

// WorldScene::UpdateNet()에서 (매 프레임)
void WorldScene::UpdateNet(float _dt) {
    g_cNet.PollNetEvents();
    pMCPServer_->PollEvents();  // ← MCP 메시지 처리
}
```

> **Verify**: `Build.ps1 -ProjectName mcpd -C Debug -P x64` 전체 빌드 성공

### Phase 7: 통합 테스트 (Step 7)

**7.1** mcpd 실행 → TCP 클라이언트로 connect
**7.2** initialize 요청/응답 확인
**7.3** tools/list → tool 목록 반환 확인
**7.4** tools/call echo → 응답 확인
**7.5** ping/pong 확인
**7.6** 잘못된 요청 → 에러 응답 확인

---

## MCP 프로토콜 상세

### Initialize (핸드셰이크)

```json
// Request (Client → Server)
{"jsonrpc":"2.0","id":1,"method":"initialize","params":{
  "protocolVersion":"2024-11-05",
  "capabilities":{},
  "clientInfo":{"name":"test-client","version":"1.0.0"}
}}

// Response (Server → Client)
{"jsonrpc":"2.0","id":1,"result":{
  "protocolVersion":"2024-11-05",
  "capabilities":{"tools":{}},
  "serverInfo":{"name":"mcpd-ultaharmony","version":"1.0.0"}
}}
```

### tools/list

```json
// Request
{"jsonrpc":"2.0","id":2,"method":"tools/list","params":{}}

// Response
{"jsonrpc":"2.0","id":2,"result":{
  "tools":[
    {"name":"echo","description":"Echo input back","inputSchema":{
      "type":"object","properties":{"text":{"type":"string"}}
    }},
    {"name":"info","description":"Server information","inputSchema":{
      "type":"object","properties":{}
    }}
  ]
}}
```

### tools/call

```json
// Request
{"jsonrpc":"2.0","id":3,"method":"tools/call","params":{
  "name":"echo",
  "arguments":{"text":"hello"}
}}

// Response
{"jsonrpc":"2.0","id":3,"result":{
  "content":[{"type":"text","text":"hello"}],
  "isError":false
}}
```

---

## 참고: 기존 프로젝트 규약

| 항목 | 규칙 |
|------|------|
| 네임스페이스 | `mcp` (NS_MCP_BEGIN / NS_END 패턴) |
| PCH | `PCH/Core.h` → `Core.cpp` |
| 인코딩 | UTF-8 |
| C++ 표준 | C++20 (`stdcpplatest`) |
| 경고 수준 | Level3 |
| 전처리기 | `STATIC_LIB_MCP` (라이브러리 빌드 시) |
| 메모리 | `dbg_new` / `jc::MemoryPool` (jnet 패턴 준수) |

---

## 의존성 그래프

```
mcpd (executable)
  └── mcp (static lib)
       ├── jnet (static lib)
       │    ├── jc (static lib)
       │    └── ws2_32.lib / Mswsock.lib
       ├── jc (static lib)
       └── jsoncpp.lib (ThirdParty)
```

## 완료 조건

1. `mcp` 라이브러리 x64 Debug/Release 빌드 0 error
2. `mcpd` 실행 파일 x64 Debug/Release 빌드 0 error
3. mcpd 실행 후 TCP 클라이언트로 initialize/ping/tools/list/tools/call 정상 동작 확인
4. 잘못된 요청에 대해 적절한 JSON-RPC 에러 응답 반환
