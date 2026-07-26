# MCP 세션 맵 동기화 완료

## 변경 내용

### 문제
`MCPServer::sessionMap_`이 멀티스레드 환경에서 동기화 없이 접근:
- `PollEvents()` (메인 스레드): map iterate + ProcessMessages()
- `OnTransportConnected()` (IOCP 워커): map insert
- `OnTransportDisconnected()` (IOCP 워커): map erase + session delete

### 해결

**MCPServer.h** (`Projects/mcp/Sources/mcp/Protocol/MCPServer.h`):
- `#include <jc/Sync/NormalLock.h>`, `#include <jc/Primitives/Atomic.h>` 추가
- `std::vector<MCPSession*> pendingDelete_` — 지연 삭제 리스트
- `jc::NormalLock sessionLock_` — 세션 맵 보호 뮤텍스
- `bool running_` → `jc::AtomicBool running_`
- `bool shutdownRequested_` → `jc::AtomicBool shutdownRequested_`

**MCPServer.cpp** (`Projects/mcp/Sources/mcp/Protocol/MCPServer.cpp`):
- `PollEvents()`: lock → copy map → unlock → process → lock → delete pending → unlock
- `OnTransportConnected()`: lock → create → insert → unlock
- `OnTransportDisconnected()`: lock → move to pendingDelete_ → erase → unlock
- `Stop()`: pendingDelete_ cleanup 추가

### 결과
- 모든 빌드 통과 (mcp.lib + mcpr.exe)
- 6개 테스트 전부 통과 (6 passed, 0 failed)
- 멀티스레드 데이터 레이스 제거 완료
