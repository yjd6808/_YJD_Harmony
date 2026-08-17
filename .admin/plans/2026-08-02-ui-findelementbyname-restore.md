# 2026-08-02 UI_* FindElementByName 복원 (XML 우선 + 코드 빌드 폴백)

## 작업 요약
dev 브랜치의 UI_* 클래스들이 XML 레이아웃 + `OnInit`에서 `FindElementByName<T>("name")` 멤버 할당 패턴으로 동작하지 않던 것을, sg-main(UltimateHarmony) 원본 패턴으로 복원했다.
사용자 확정 방식: **XML 우선 + REGISTER_WINDOW 코드 빌드 폴백**.

## 변경 사항

### sgui 프레임워크 확장
| 파일 | 내용 |
|---|---|
| `Game/UI/Controls/Window.h` | `SetXmlLoaded(bool)` / `IsXmlLoaded()` / `bXmlLoaded_` 멤버 추가 (XML 병합 여부 플래그) |
| `Game/UI/Xml/UIXmlLoader.h/.cpp` | `LoadWindowInto(Window*, filePath)` / `LoadWindowIntoString(Window*, xml)` 정적 메서드 추가. 기존 `LoadWindowFromDocument` 파싱 로직을 `PopulateWindowFromDocument(Window*, doc)` 헬퍼로 추출해 **기존 인스턴스에 XML 트리 병합** 구현 (성공 시 `SetXmlLoaded(true)`) |
| `Game/UI/Host/UIManager.cpp` | `Show()` **XML 우선**으로 변경: 팩토리 인스턴스 생성 → XML 존재 시 `LoadWindowInto` 병합 → 팩토리 없으면 `LoadWindowFromFile` → 병합 실패 시 코드 빌드 폴백 (경고 로그) |

### UI_* 클래스 리팩토링 (OnInit XML 분기 + 코드 빌드 폴백)
| 파일 | XML 경로 | 폴백 |
|---|---|---|
| `UI_Generic.cpp/.h` | FindElementByName으로 title/close/chk_a~d/toggle_x/pbar_1~6/lbl_theme_status 할당 + 이벤트 연결 + pbar 퍼센트 | 기존 BuildControlShowcase/BuildThemeTestButtons 코드 빌드 유지 |
| `UI_Login.cpp/.h` | source/developer/login_box/hangame_login/dnf_login/start/terminate/id/pw 할당 + 코드 전용 속성(SetMaxLength/SetPasswordMode/ueTextSubmit) + 콜백 | 기존 코드 빌드 유지 |
| `UI_ChannelSelect.cpp/.h` | start/terminate/refresh/teen/adult/page 할당 + server_list/channel_list 컨테이너에 동적 버튼 생성 | 헬퍼(BuildServerButtons/BuildChannelButtons) 추출해 코드 빌드에서 재사용 |
| `UI_Inventory.cpp/.h` | background/스크롤바 + equip_slots/inven_slots 컨테이너에 동적 슬롯 생성 | 헬퍼(BuildEquipSlots/BuildInvenSlots) 추출해 코드 빌드에서 재사용 |
| `UI_Test.cpp/.h` | groupO1/groupO2 + 메뉴 버튼 5개 FindElementByName + 콜백 | 기존 코드 빌드 유지 |

- 모든 OnLoaded/OnUpdate/SetTab 등에서 nullptr 안전처리 추가 (XML 경로에서 요소 누락 시 크래시 방지)
- `UI_Login`의 `Button`은 `USING_NS_CCUI`로 인한 cocos2d::ui::Button 충돌 → `sgui::Button` 명시
- `UI_ChannelSelect.h`의 `InitFromXml`은 `jc::CDataMap<>` 접두사 필요 (헤더는 using namespace 없음)

### dev layout XML WPF 변환 (`UltimateHarmony_Dev/assets/res_data/layout/`)
구 `<Group>`/SGA 형식 → WPF `<Window>` 형식, SGA 스프라이트 속성(sga/img/sprite) 제거, 테마 기반 컨트롤로 대체.
- `ui_generic.xml`: 900x480, 버튼/체크박스/토글/프로그레스바 크기 데모 유지
- `ui_login.xml`: login_box 중앙 배치, hangame/dnf 토글 + id/pw TextBox + start/terminate 버튼
- `ui_channel.xml`: Grid 4행, server_list/channel_list 빈 컨테이너 + teen/adult/page/start/refresh/terminate
- `ui_inventory.xml`: background Border + equip_slots/inven_slots 컨테이너 + 스크롤바
- `ui_test.xml`: 메뉴 버튼 5개 + groupO1/groupO2 (신규 생성)

> 참고: `ui_popup.xml`은 PopupManager가 직접 생성(REGISTER_WINDOW 미사용)하므로 XML 병합 대상 아님. `ui_hud/ui_chat/ui_chat_input/ui_select_character/ui_select_charactor.xml`은 등록 팩토리가 없어 현재 Show되지 않음.

## 검증
1. **빌드**: `MSBuild sgcl.vcxproj (Debug/x64)` — 0 에러, `Output\x64\Debug\sgcl.exe` 생성
2. **런타임**: `Scripts\start_up_debugging.ps1` 실행 (assets=UltimateHarmony_Dev) →
   `[UI_Generic] OnInit(XML) title=... close=... chk=... toggle=... pbar1=... pbar6=... status=...` 로그 확인
   → **XML 병합 + FindElementByName 할당이 실제 동작함**을 검증

## 부산물
- `sgcl.vcxproj` ClInclude .cpp 중복 167개 제거 (이전 단계, 프로젝트 로드 에러 해결)
- 계획 문서: `.admin/plans/2026-08-02-apply-fable-sgui.md` 진행 로그 갱신

---

## 추가 수정 (2026-08-02): 종료 시점 크래시 (윈도우 이중 autorelease)

최종 검증 중 `Node::~Node`(CCNode.cpp:191) `CCASSERT(!_running, ...)` 크래시를 발견해 수정했다.

### 증상
- 실행 시작(로그인 씬, ui_generic 오픈) 직후 또는 로그인 블로킹(네트워크 타임아웃) 종료 후 크래시. 재현성 있음.
- 계측 로그(`2026-08-02-103629.log`): `[UIHost] AddWindow ui_generic rc=2 running=1` → `-- 씬전환 완료` → `[Window] ~Window name=ui_generic rc=0 running=1` (RemoveWindow/Clear 없이 소멸)
- 크래시 스택(`2026-08-02-101152.log`): `cocos2d::AutoreleasePool::clear` → `cocos2d::Ref::release` → `~UI_Generic` → ... → `Node::~Node` assert → abort(0x80000003)

### 원인
윈도우가 **이중 autorelease** 되어 첫 `AutoreleasePool::clear()`에서 over-release 되었다.
- `new` → rc=1
- 생성자가 autorelease 1회 (팩토리 `WindowRegister.h:30` / `UIXmlLoader::LoadWindowFromFile` `UIXmlLoader.cpp:765`)
- `UIManager::ShowWindow`에서 autorelease **또 1회** (`UIManager.cpp:156`)
- `UIHost::AddWindow` → `addChild` → rc=2
- 다음 `AutoreleasePool::clear()`에서 2회 release → rc=0 → 씬 트리에 부착(`_running=true`)된 채 `delete` → `Node::~Node` assert

### 수정
`UIManager::ShowWindow`에서 `_pWindow->autorelease();` 제거. 생성자(팩토리/XML 로더)가 이미 1회 autorelease 하므로 소유권은
`new(1) + addChild(1) = rc 2` → 풀 정리 1회 release → rc 1(addChild 소유) → removeFromParent 시 onExit 후 rc 0으로 정상 소멸.

### 검증
1. 빌드: `Build.ps1 -ProjectName sgcl -Configuration Debug -Platform x64` — 0 에러
2. 런타임: `start_up_debugging.ps1` 연속 2회 실행 → 로그가 `-- 씬전환 완료`에서 종료되고 `[Window] ~Window` 소멸 로그·크래시 덤프 미생성 → 정상
