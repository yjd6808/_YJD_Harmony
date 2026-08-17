# sgcl 프로젝트 필터-폴더 구조 정합성 작업

**날짜:** 2026-04-16  
**결과:** 빌드 성공 (경고 1개, 오류 0개)

---

## 작업 개요

`sgcl.vcxproj.filters`의 필터명과 실제 물리 폴더 경로가 불일치하는 파일들을
필터명에 맞는 물리 폴더로 이동하고, vcxproj/filters 경로 및 #include를 일괄 수정.

---

## 신규 생성 폴더 (Classes/sgcl/ 기준)

| 폴더 | 필터명 |
|---|---|
| `Base/` | `Base` |
| `Game/Actor/` | `Game\Actor` |
| `Game/Texture/` | `Game\Texture` |
| `Game/Action/` | `Game\Action` |
| `Game/Action/Gunner/` | `Game\Action\Gunner` |
| `Game/Contents/` | `Game\Contents` |
| `Game/UI/` | `Game\UI` |
| `Game/UI_Implementation/` | `Game\UI_Implementation` |
| `Game/Helper/` | `Game\Helper` |
| `Game/Util/` | `Game\Util` |
| `Game/Camera/` | `Game\Camera` |
| `Game/AI/` | `Game\AI` |
| `Game/AI/Activity/` | `Game\AI\Activity` |
| `Game/AI/Activity/FallDown/` | `Game\AI\Activity\FallDown` |
| `Game/AI/Activity/Hit/` | `Game\AI\Activity\Hit` |
| `Game/AI/Activity/Idle/` | `Game\AI\Activity\Idle` |
| `Game/AI/Activity/Sit/` | `Game\AI\Activity\Sit` |
| `Game/AI/Activity/Walk/` | `Game\AI\Activity\Walk` |
| `Game/Component/` | `Game\Component` |
| `Game/Listener/` | `Game\Listener` |
| `Game/Listener/Character/` | `Game\Listener\Character` |
| `Game/Listener/Projectile/` | `Game\Listener\Projectile` |
| `Game/Listener/Monster/` | `Game\Listener\Monster` |
| `Game/Callback/` | `Game\Callback` |
| `Net/Send/` | `Net\Send` |
| `Net/Recv/` | `Net\Recv` |
| `Net/Connection/` | `Net\Connection` |
| `Net/Component/` | `Net\Component` |

---

## 이동된 파일 통계

- **물리 이동:** 212개 파일 (Global.h 미존재 1건 제외)
- **#include 치환:** 201개 파일 업데이트
- **vcxproj/filters:** 경로 자동 치환

---

## 수행 방법

PowerShell 스크립트 3단계로 실행:
1. **Stage 1** (`_Temp_MoveFiles.ps1`): 폴더 생성 + 파일 이동
2. **Stage 2** (`_Temp_UpdateVcxproj.ps1`): vcxproj/filters 경로 수정
3. **Stage 3** (`_Temp_UpdateIncludes.ps1`): #include 경로 치환

---

## 수동 보완 사항

| 파일 | 문제 | 수정 |
|---|---|---|
| `Net/Connection/C_CHAT.cpp` | `sgcl/S_Chat.h` (대소문자 불일치) | → `sgcl/Net/Send/S_CHAT.h` |

---

## 루트에 남은 파일 (필터 없음)

- `SteinsGateApp.cpp/h`
- `main.cpp`
- `GameCore.cpp/h`
- `GameCoreHeader.h`
- `Namespace.h`

---

## 미해결 사항

- `Global.h` — vcxproj.filters에 `Game\Contents` 필터로 등록되어 있으나 물리 파일 미존재 (dangling reference)
- `sgcl.vcxproj.filters`에 `<Filter Include="Util">` 중복 항목 2개 존재 (UUID만 다름)
- `_Net/NetClientGroup.h/cpp` — vcxproj 미포함 미사용 중복 파일 (이전 세션에서 확인된 사항)

---

## 빌드 결과

```
경고 1개 (sg/Util/DescMgr/DescMgr_MapPhysics.h BOM 누락 - 이번 작업과 무관)
오류 0개
```
