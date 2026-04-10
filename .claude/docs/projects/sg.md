# sg (SteinsGate Common)

## 개요
**서버/클라이언트 양쪽이 공유하는 게임 공통 라이브러리** (Static Library).  
게임 데이터 구조, 명령어 정의, 상수, 리소스 로더 등을 포함.  
총 232개 파일.

## 소스 구조

```
Sources/sg/
├── _Core/         - 기본 기능 (AppConfig, AudioPlayer, CLI)
├── _Net/          - 네트워크 공통 (NetListenerHelper, NetServerListener)
├── _API/          - 공개 API (sgapiBase, sgapi_Util)
├── _Struct/       - 게임 데이터 구조 (CharBase, Item, Skill, Quest 등)
├── _Util/         - 유틸리티 (JsonUtil, TextUtil)
│   └── _DescMgr/  - Desc 로더들 (Action, AI 등)
├── _Object/
│   └── _Character/ - 캐릭터 관련
├── _Sga/          - SGA 리소스 로더 (이미지/사운드 패키징)
├── PCH/
├── Cmd*.h         - 명령어 정의 (AUTHENTICATION, CHAT 등)
├── Const*.h       - 상수 정의 (CHARACTER, ITEM, SERVER 등)
├── Config.h/cpp   - 런타임 설정
├── CommonCore.h/cpp - 공통 초기화
└── Logo.h/cpp     - 서버별 로고 출력
```

## 핵심 파일

| 파일 | 역할 |
|------|------|
| `Cmd*.h` | 클라↔서버 명령어 정의 (패킷 구조체) |
| `Const*.h` | 게임 상수 (FPS, Timeout, 아이템/캐릭터 한계값 등) |
| `_Struct/` | CharBase, Item, Skill, Quest 등 게임 데이터 구조체 |
| `_Sga/SgaLoader` | SGA 포맷 리소스 파일 로더 |
| `CommonCore` | CharCommon, CommandNameDictionary 초기화 |
| `Config` | 로그 필터링, 출력 옵션 런타임 설정 |

## 의존성
- `jc` (기반 라이브러리)
- `jnet` (네트워크)
- `jsoncpp` (JSON 파싱)

## 빌드 산출물
- `sg.lib` (Static Library)
