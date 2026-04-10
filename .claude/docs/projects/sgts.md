# sgts (SteinsGate Tool Server)

## 개요
게임 도구 서버 애플리케이션. **현재 미구현** (main.cpp에 `return 0`만 존재).

## 소스 구조

```
Sources/sgts/
├── PCH/
├── Scripts/          - 빌드/배포 스크립트
├── Core.h/cpp        - sgt::Core include
└── main.cpp          - 진입점 (미구현)
```

## 의존성
- `sgt` → `jnet`, `sgcc`

## 빌드 산출물
- `sgts.exe` (Application, 미구현)
