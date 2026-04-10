# sgs_auth (인증 서버)

## 개요
클라이언트 **로그인 인증 및 토큰 발급**을 담당하는 서버.  
총 15개 파일.

## 소스 구조

```
Sources/sgs_auth/
├── PCH/Core.h
├── AuthCore.h/cpp               - 서버 초기화/종료 (ServerCore 상속)
├── AuthenticationManager.h/cpp  - 토큰 관리 싱글톤 (5분 만료)
├── Q_LOGIN.h/cpp                - 로그인 DB 쿼리
├── R_AUTHENTICATION.h/cpp       - 인증 요청 수신 처리
├── S_AUTHENTICATION.h/cpp       - 클라이언트에 인증 결과 송신
├── S_AUTHENTICATION_IS.h/cpp    - 로비 서버에 인증 정보 릴레이
└── main.cpp
```

## 처리 흐름
1. 중앙서버(sgs_center)에 연결
2. 클라이언트 인증 요청 수신 (`R_AUTHENTICATION`)
3. DB에서 계정 확인 (`Q_LOGIN`)
4. `AuthenticationManager`가 토큰 발급 (5분 TTL)
5. 클라이언트에 결과 송신 (`S_AUTHENTICATION`)
6. 로비 서버에 인증 정보 릴레이 (`S_AUTHENTICATION_IS`)

## 의존성
- `sgs` → `sg` → `jc`, `jnet`
