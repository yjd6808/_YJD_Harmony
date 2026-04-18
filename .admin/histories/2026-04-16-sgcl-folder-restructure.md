# sgcl 프로젝트 폴더 구조 재정비

**날짜:** 2026-04-16  
**결과:** 빌드 성공 (경고 1개, 오류 0개)

---

## 작업 개요

sgcl 프로젝트의 물리 폴더명에서 언더스코어(`_`) 접두사를 제거하고,
Const/ 및 Define/ 폴더를 신규 생성하여 관련 파일들을 이동했다.
이후 모든 #include 경로, vcxproj, vcxproj.filters를 일괄 수정했다.

---

## 변경된 폴더 구조

| 이전 경로 | 변경 후 경로 |
|---|---|
| `Classes/sgcl/_API/` | `Classes/sgcl/API/` |
| `Classes/sgcl/_Core/` | `Classes/sgcl/Core/` |
| `Classes/sgcl/_Layer/` | `Classes/sgcl/Layer/` |
| `Classes/sgcl/_Net/` | `Classes/sgcl/Net/` |
| `Classes/sgcl/_Scene/` | `Classes/sgcl/Scene/` |
| `Classes/sgcl/_Struct/` | `Classes/sgcl/Struct/` |
| `Classes/sgcl/_Util/` | `Classes/sgcl/Util/` |
| `Classes/sgcl/_Util/_DescMgr/` | `Classes/sgcl/Util/DescMgr/` |
| (없음) | `Classes/sgcl/Const/` (신규) |
| (없음) | `Classes/sgcl/Define/` (신규) |

---

## Const/ 폴더 이동 파일 (4개)

- Const.h
- Const_HOST.h
- Const_NETGROUP.h
- Const_TIMEOUT.h

---

## Define/ 폴더 이동 파일 (10개)

- Define.h
- Define_.h
- Define_Action.h
- Define_ActorListener.h
- Define_Animation.h
- Define_Effect.h
- Define_Event.h
- Define_Popup.h
- Define_Projectile.h
- Define_UI.h

---

## 수정된 파일 목록

### #include 경로 수정 (PowerShell 일괄 치환 + 수동 보완)

총 약 80여 개 파일 수정. 주요 수동 보완 대상:

| 파일 | 수정 내용 |
|---|---|
| `Classes/sgcl/Net/NetCore.h` | `"sgcl/NetClientGroup.h"` → `"sgcl/Net/NetClientGroup.h"` |
| `Classes/sgcl/SteinsGateApp.cpp` | `"_API/sgapiClient.h"` → `"API/sgapiClient.h"` |
| `Classes/sgcl/Net/CommandSynchronizer.cpp` | `"_Net/CommandSynchronizer.h"` → `"Net/CommandSynchronizer.h"` |
| `Classes/sgcl/Net/ConnectionSynchronizer.cpp` | `"_Net/ConnectionSynchronizer.h"` → `"Net/ConnectionSynchronizer.h"` |
| `Classes/sgcl/Scene/Scene_Base.cpp` | `"_Scene/Scene_Base.h"` → `"Scene/Scene_Base.h"` |
| `Classes/sgcl/Scene/Scene_ChannelSelect.cpp` | `"_Scene/Scene_ChannelSelect.h"` → `"Scene/Scene_ChannelSelect.h"` |
| `Classes/sgcl/GameScene.cpp` | `"_Scene/Scene_Game.h"` → `"Scene/Scene_Game.h"` |

### vcxproj / vcxproj.filters

- `Project/sgcl.vcxproj` — 모든 파일 경로 수정
- `Project/sgcl.vcxproj.filters` — 파일 경로 및 필터명 수정

---

## 빌드 결과

```
경고 1개 (sg/Util/DescMgr/DescMgr_MapPhysics.h BOM 누락 - 이번 작업과 무관)
오류 0개
```

---

## 미해결 사항

- `Classes/sgcl/Net/NetClientGroup.h/cpp` — vcxproj에 미포함된 미사용 중복 파일.
  루트 버전(`Classes/sgcl/NetClientGroup.h/cpp`)과 내용이 다름. 별도 확인 필요.
