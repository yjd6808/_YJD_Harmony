# sgtc (SteinsGate Tool Client)

## 개요
**Cocos2d-x 기반 게임 도구 클라이언트 UI 애플리케이션**.

## 소스 구조

```
Classes/sgtc/
├── PCH/
├── Core.h/cpp
├── App.h/cpp              - Cocos2d-x Application 파생
├── main.cpp               - 진입점
├── _Layer/
│   └── Layer_Game.h/cpp
└── _Scene/
    ├── Scene_Base.h/cpp
    ├── Scene_Game.h/cpp
    └── Scene_World.h/cpp
```

## 의존성
- `Cocos2d-x` (GUI/게임 엔진)
- `jnet` (네트워크)
- `sgcc` (수학/타입)

## 빌드 산출물
- `sgtc.exe` (Application)
