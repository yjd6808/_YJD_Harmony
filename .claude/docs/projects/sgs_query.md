# sgs_query (쿼리 서버)

## 개요
서버 정보 조회 및 상태 모니터링을 위한 서버. **기본 구조만 존재**, 구현 초기 단계.  
총 5개 파일.

## 소스 구조

```
Sources/sgs_query/
├── PCH/Core.h
├── QueryCore.h/cpp    - 서버 초기화/종료 (ServerCore 상속)
└── main.cpp
```

## 의도된 역할
- 게임 서버 상태/플레이어 수 조회
- 서버 정보 API 제공
- `jdb` 라이브러리 활용 (프로젝트명에서 유추)

## 의존성
- `sgs` → `sg` → `jc`, `jnet`
- `jdb` (DB 연동 예정)
