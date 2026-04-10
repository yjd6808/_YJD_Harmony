# sgs_lobby (로비 서버)

## 개요
로비 화면에서 **캐릭터 관리 및 게임 서버 입장**을 처리하는 서버.  
총 17개 파일.

## 소스 구조

```
Sources/sgs_lobby/
├── PCH/Core.h
├── LobbyCore.h/cpp                      - 서버 초기화/종료 (ServerCore 상속)
├── UnauthenticatedSessionManager.h/cpp  - 인증 전 세션 관리
├── Q_LOBBY.h/cpp                        - 로비 DB 쿼리 (캐릭터 목록 조회 등)
├── R_AUTHENTICATION.h/cpp               - 인증 응답 처리
├── R_LOBBY.h/cpp                        - 로비 요청 수신 처리
├── S_AUTHENTICATION_IS.h/cpp            - 인증 서버에 인증 정보 송신
├── S_LOBBY.h/cpp                        - 클라이언트에 로비 정보 송신
└── main.cpp
```

## 처리 흐름
1. 중앙서버(sgs_center)에 연결
2. 인증 서버에서 인증 확인 수신 (`R_AUTHENTICATION`)
3. 인증 전 세션 관리 (`UnauthenticatedSessionManager`)
4. 로비 요청 처리: 캐릭터 목록 조회, 생성, 삭제 (`R_LOBBY`, `Q_LOBBY`)
5. 클라이언트에 결과 송신 (`S_LOBBY`)

## 의존성
- `sgs` → `sg` → `jc`, `jnet`
- `jdb` (DB 쿼리)
