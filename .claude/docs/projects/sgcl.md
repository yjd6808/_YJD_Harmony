# sgcl (SteinsGate Client)

## 개요
**Cocos2d-x 기반 메인 게임 클라이언트 애플리케이션**.  
캐릭터, 몬스터, UI, 네트워킹, 물리 등 전체 게임 로직 포함.  
총 290개 파일.

## 소스 구조

```
Classes/sgcl/
├── PCH/Core.h/cpp
├── _Core/           - 앱 설정 (Color, AppConfig_Props, CLIFunctions)
├── _Net/            - 네트워킹 (NetCore, NetClientGroup, CommandSynchronizer 등)
├── _Scene/          - 씬 (Login, ChannelSelect, Game, World 등)
├── _Layer/          - 레이어 (Layer_Game)
├── _API/            - API 클래스
├── _Struct/         - 데이터 구조
└── _Util/           - 유틸리티
```

## 주요 시스템

### 액터 시스템
| 클래스 | 역할 |
|--------|------|
| `Actor` | 게임 엔티티 기반 클래스 |
| `Character` | 플레이어 캐릭터 |
| `Monster` | 몬스터 (Goblin 등) |
| `Projectile` | 발사체 |
| `ActorManager` | 액터 관리 |

### 건너(Gunner) 캐릭터 (20개 클래스)
`GunnerIdle`, `GunnerWalk`, `GunnerRun`, `GunnerJump`, `GunnerGunShot`, `GunnerSitRecover` 등

### AI/Activity 시스템
`AIActivity` 기반: `IdleActivity`, `WalkActivity`, `RunActivity`, `AttackActivity`, `HitActivity`, `FallDownActivity` 등  
캐릭터 종류별 파생: `*_Goblin`, `*_Gunner`

### UI 시스템
`UIElement` 기반: `UIButton`, `UICheckBox`, `UIEditBox`, `UILabel`, `UISprite`, `UIProgressBar`, `UIScrollBar`, `UIToggleButton`, `UIGroup`  
화면: `UI_Login`, `UI_Inventory`, `UI_Popup`, `UI_ChannelSelect`

### 네트워킹
- `NetCore`, `NetClientGroup`
- `CommandSynchronizer`, `ConnectionSynchronizer`
- 명령어: `C_AUTH`, `C_LOBBY`, `C_GAME`, `C_CHAT`, `C_AREA`
- 응답: `S_AUTH`, `S_LOBBY`, `S_GAME`, `S_CHAT`, `S_AREA`, `S_GAME_UDP`

### 렌더링
- `ActorSprite`, `ActorPartSprite`, `ActorPartAnimation`
- `Image`, `ImagePack`, `ImagePackManager`
- `FrameTexture`, `MimicCamera`

### 물리
- `PhysicsComponent`, `MoveComponent`, `HitRecorder`

## 씬 흐름
```
Scene_Login → Scene_ChannelSelect → Scene_Game (Scene_World)
```

## 의존성
- `Cocos2d-x` (게임 엔진/렌더링)
- `jnet` (네트워크)
- `sgcc` (수학/타입)
- `sge` (링크만, 실제 코드 없음)

## 빌드 산출물
- `sgcl.exe` (메인 게임 클라이언트)
