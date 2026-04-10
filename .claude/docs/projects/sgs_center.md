# sgs_center (중앙 서버)

## 개요
게임 서버 인프라의 **허브**. 모든 서버(Auth, Lobby, Game, Query)를 관리하고 서버 간 메시지를 라우팅.  
총 15개 파일.

## 소스 구조

```
Sources/sgs_center/
├── PCH/Core.h
├── CenterCore.h/cpp         - 서버 초기화/종료 (ServerCore 상속)
├── BootStateManager.h/cpp   - 서버 부팅 상태 관리 싱글톤
├── R_SETUP.h/cpp            - 개별 서버 setup 요청 처리
├── R_PING.h/cpp             - Ping 응답 처리
├── R_MESSAGE.h/cpp          - 메시지 처리
├── S_SETUP_IS.h/cpp         - 서버간 setup 응답 송신
└── main.cpp
```

## 역할
- `NetGroup_Main`으로 연결된 모든 서버 관리
- `BootStateManager`로 각 서버의 부팅 상태 추적
- 서버간 모든 메시지 라우팅 (릴레이)

## 의존성
- `sgs` → `sg` → `jc`, `jnet`
