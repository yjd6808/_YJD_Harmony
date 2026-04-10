# sgs (SteinsGate Server Common)

## 개요
**모든 게임 서버가 공유하는 서버 공통 라이브러리** (Static Library).  
컴포넌트 기반 아키텍처, 서버 간 통신 프로토콜, 세션 관리를 제공.  
총 70개 파일.

## 소스 구조

```
Sources/sgs/
├── _API/                        - 서버 API (sgapiServerBase)
├── _Net/                        - 네트워크 (NetCore, NetServer, NetGroup_Main, NetGroup_InterServ)
├── PCH/
├── ComponentCollection.h/cpp    - 컴포넌트 컨테이너
├── ServerCore.h/cpp             - 서버 초기화 기반 클래스
├── IComponent.h                 - 컴포넌트 인터페이스
├── IUpdatable.h                 - 주기 업데이트 인터페이스
├── ISessionEventHandler.h       - 세션 이벤트 핸들러
├── CmdHost*.h                   - 중앙서버 ↔ 개별서버 명령어
├── CmdRelay*.h                  - 중앙서버 경유 릴레이 명령어
├── InterServerCmdDefine.h       - 서버간 통신 명령어 정의
├── InterServerSendHelper.h/cpp  - 서버간 송신 헬퍼
├── InterServerRecvHelper.h/cpp  - 서버간 수신 헬퍼
├── AuthenticationComponent.h/cpp
├── SessionComponent.h/cpp
├── UpdatableCollection.h/cpp
└── ServerConst*.h               - 서버 상수
```

## 아키텍처 패턴

### 컴포넌트 기반
```cpp
ComponentCollection {
    HashMap<int, IComponent*>      // 컴포넌트 맵
    Vector<IUpdatable*>            // 업데이트 목록
    Vector<ISessionEventHandler*>  // 세션 이벤트 핸들러 목록
}
```

### 서버간 명령어 체계
| 타입 | 방향 | 설명 |
|------|------|------|
| `HostStatic/Dynamic` | 중앙서버 ↔ 개별서버 | 직접 통신 |
| `RelayStatic/Dynamic` | 개별서버 → 중앙서버 → 다른서버 | 릴레이 통신 |

## 핵심 클래스

| 클래스 | 역할 |
|--------|------|
| `ServerCore` | 모든 서버의 기반 클래스 |
| `NetCore` | NetGroupMgr 상속, 네트워크 관리 |
| `NetGroup_Main` | 중앙서버 네트워크 그룹 |
| `NetGroup_InterServ` | 서버간 통신 그룹 |
| `ComponentCollection` | 싱글톤, 컴포넌트 관리 |
| `AuthenticationComponent` | 인증 컴포넌트 |

## 의존성
- `sg` (게임 공통)
- `jc`, `jnet`

## 빌드 산출물
- `sgs.lib` (Static Library)
