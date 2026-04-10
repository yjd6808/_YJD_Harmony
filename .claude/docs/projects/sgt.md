# sgt (SteinsGate Tool Common)

## 개요
게임 **도구 프로젝트들(sgtc, sgts)의 공용 기반 라이브러리** (DLL).  
jnet과 sgcc를 통합해 도구용 단일 의존성으로 제공.

## 소스 구조

```
Sources/sgt/
├── PCH/
├── Core.h/cpp       - jnet::Core + sgcc::Core 통합 헤더
└── DLLExport.h      - DLL 내보내기 정의
```

## 의존성
- `jnet` (네트워크)
- `sgcc` (수학/타입)

## 빌드 산출물
- `sgt.dll` (Dynamic Library)
