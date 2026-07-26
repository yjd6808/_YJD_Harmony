# MCP 세션 맵 동기화 (락 추가)

## 문제

`MCPServer::sessionMap_`이 멀티스레드 환경에서 동기화 없이 접근됨:

| 접근 위치 | 스레드 | 작업 |
|-----------|--------|------|
| `PollEvents()` | 메인 루프 스레드 | map iterate + ProcessMessages() |
| `OnTransportConnected()` | IOCP 워커 스레드 | map insert |
| `OnTransportDisconnected()` | IOCP 워커 스레드 | map erase + session delete |

## 해결 방법

1. `sessionMap_` 보호를 위한 `NormalLock` 추가
2. 모든 sessionMap_ 접근을 LockGuard로 감쌈
3. 세션 삭제를 지연(deferred deletion): `OnTransportDisconnected`에서는 map에서만 제거하고, `PollEvents()`에서 실제 delete 수행
4. `running_`, `shutdownRequested_` → `Atomic<bool>`으로 변경

## 변경 파일

- `MCPServer.h` — 멤버 변수 추가 (lock, pendingDelete)
- `MCPServer.cpp` — 모든 sessionMap_ 접근에 락 적용, 지연 삭제 처리
