# sgcl 프로젝트 폴더 구조 재정비 계획

## 작업일: 2026-04-16
## 참고: .admin/histories/2026-04-16-sg-folder-restructure.md

## 목표
sg/sgs와 동일하게 `_` 접두사 폴더 이름 제거, 추가로 Const/Define 파일 물리 폴더 정리

## 수행할 작업

### 1. 물리 폴더 이름 변경 (8개)
| 이전 | 이후 |
|------|------|
| Classes/sgcl/_API/ | Classes/sgcl/API/ |
| Classes/sgcl/_Core/ | Classes/sgcl/Core/ |
| Classes/sgcl/_Layer/ | Classes/sgcl/Layer/ |
| Classes/sgcl/_Net/ | Classes/sgcl/Net/ |
| Classes/sgcl/_Scene/ | Classes/sgcl/Scene/ |
| Classes/sgcl/_Struct/ | Classes/sgcl/Struct/ |
| Classes/sgcl/_Util/ | Classes/sgcl/Util/ |
| Classes/sgcl/_Util/_DescMgr/ | Classes/sgcl/Util/DescMgr/ |

### 2. 신규 폴더 생성 및 파일 이동
- `Const/` 생성 → Const.h, Const_HOST.h, Const_NETGROUP.h, Const_TIMEOUT.h 이동 (4개)
- `Define/` 생성 → Define.h, Define_.h, Define_Action.h, Define_ActorListener.h, Define_Animation.h, Define_Effect.h, Define_Event.h, Define_Popup.h, Define_Projectile.h, Define_UI.h 이동 (10개)

### 3. #include 경로 치환 패턴
| 이전 | 이후 |
|------|------|
| sgcl/_API/ | sgcl/API/ |
| sgcl/_Core/ | sgcl/Core/ |
| sgcl/_Layer/ | sgcl/Layer/ |
| sgcl/_Net/ | sgcl/Net/ |
| sgcl/_Scene/ | sgcl/Scene/ |
| sgcl/_Struct/ | sgcl/Struct/ |
| sgcl/_Util/_DescMgr/ | sgcl/Util/DescMgr/ |
| sgcl/Const_HOST.h | sgcl/Const/Const_HOST.h |
| sgcl/Const_NETGROUP.h | sgcl/Const/Const_NETGROUP.h |
| sgcl/Const_TIMEOUT.h | sgcl/Const/Const_TIMEOUT.h |
| sgcl/Const.h | sgcl/Const/Const.h |
| sgcl/Define_Action.h | sgcl/Define/Define_Action.h |
| sgcl/Define_ActorListener.h | sgcl/Define/Define_ActorListener.h |
| sgcl/Define_Animation.h | sgcl/Define/Define_Animation.h |
| sgcl/Define_Effect.h | sgcl/Define/Define_Effect.h |
| sgcl/Define_Event.h | sgcl/Define/Define_Event.h |
| sgcl/Define_Popup.h | sgcl/Define/Define_Popup.h |
| sgcl/Define_Projectile.h | sgcl/Define/Define_Projectile.h |
| sgcl/Define_UI.h | sgcl/Define/Define_UI.h |
| sgcl/Define_.h | sgcl/Define/Define_.h |
| sgcl/Define.h | sgcl/Define/Define.h |

### 4. sgcl.vcxproj 파일 경로 수정
- `_API\`, `_Core\`, `_Layer\`, `_Net\`, `_Scene\`, `_Struct\`, `_Util\`, `_Util\_DescMgr\` 경로 제거
- Const/Define 파일 이동에 따른 경로 업데이트

### 5. sgcl.vcxproj.filters 수정
- 필터명: `_API`→`API`, `_Core`→`Core`, `_Layer`→`Layer`, `_Net`→`Net`, `_Scene`→`Scene`, `_Struct`→`Struct`, `_Util`→`Util`, `_Util\_DescMgr`→`Util\DescMgr`
- `_Net\Send`→`Net\Send`, `_Net\Recv`→`Net\Recv`, `_Net\Connection`→`Net\Connection`, `_Net\Component`→`Net\Component`

## 미사용 파일 (삭제 불포함)
- `_Net/NetClientGroup.h/cpp` - vcxproj 미포함, 루트 버전과 내용 다름
  → 이번 작업에서는 삭제하지 않음 (별도 확인 필요)
