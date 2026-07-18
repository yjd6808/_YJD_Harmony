# UI Refactoring Plan

## 1. XML 포맷 명세

### 1.1 기본 구조 및 name 중복 금지

- 모든 UI element는 `name` 속성으로 식별됨. `id`/`code` 속성은 **존재하지 않음**
- **name 중복 금지**: 동일 RootGroup 내에서 계층 구조와 상관없이 같은 `name`을 가진 element는 허용되지 않음
- RootGroup 자신의 `name`이 XML 파일명이 됨 (예: `name="Login"` → `Login.xml`)

```xml
<Group name="Login">
    <Button name="btnStart" x="100" y="200" width="200" height="50"
            sga="ui.sga" img="button" sprite="0 1 2 3" />
    <Button name="btnTerminate" x="320" y="200" width="200" height="50"
            sga="ui.sga" img="button" sprite="0 1 2 3" />
    <Group name="LoginBox" x="50" y="50" width="540" height="380">
        <Label name="title" x="0" y="10" width="540" height="40"
               text="로그인" font="NanumGothic" font_size="28" font_color="255,255,255,255" />
        <EditBox name="idInput" x="100" y="100" width="300" height="40"
                 font_size="20" font_color="0,0,0,255"
                 p_text="아이디 입력" p_font_color="128,128,128,255" />
    </Group>
</Group>
```

### 1.2 XML 속성 매핑 (JSON → XML)

| JSON Key | XML Attribute | 비고 |
|---|---|---|
| `code` | **(제거)** | code/id 체계 완전 폐기. `name`으로 식별 |
| `type` | (암시적) | 태그 이름으로 결정 (Button, Label, Sprite...) |
| `name` | `name` | 유일 식별자, RootGroup내 중복 불가 |
| `dname` | **(제거)** | Define 헤더 폐기로 불필요 |
| `size` | `width`, `height` | 별도 속성으로 분리 |
| `valign` | `valign` | VAlignment (0=Top, 1=Center, 2=Bottom) |
| `halign` | `halign` | HAlignment (0=Left, 1=Center, 2=Right) |
| `visible` | `visible` | 가시성 (기본값 true) |
| `children` | (자식 XML 노드) | 중첩 구조로 표현 |
| `sga` | `sga` | SGA 패키지명 |
| `img` | `img` | 이미지명 |
| `sprite` | `sprite` | 스프라이트 인덱스(들) |
| `linear_dodge` | `linear_dodge` | 선형 닷지 |
| `scale9` | `scale9` | 9-슬라이스 |
| `font` | `font` | 폰트 파일명 (.ttf) |
| `font_size` | `font_size` | 폰트 크기 |
| `font_color` | `font_color` | 폰트 색상 (R,G,B,A) |
| `text` | `text` | 텍스트 내용 |
| `text_wrap` | `text_wrap` | 텍스트 줄바꿈 |
| `text_valign` | `text_valign` | 텍스트 수직 정렬 |
| `text_halign` | `text_halign` | 텍스트 수평 정렬 |
| `p_text` | `p_text` | Placeholder 텍스트 |
| `p_font_color` | `p_font_color` | Placeholder 폰트 색상 |
| `p_font_size` | `p_font_size` | Placeholder 폰트 크기 |
| `max_length` | `max_length` | 최대 입력 길이 |
| `input_mode` | `input_mode` | 입력 모드 |
| `check` | `check` | 체크 상태 (CheckBox) |
| `bg_sga` | `bg_sga` | 배경 SGA (CheckBox) |
| `bg_img` | `bg_img` | 배경 이미지 (CheckBox) |
| `cross_sga` | `cross_sga` | 크로스 SGA (CheckBox) |
| `cross_img` | `cross_img` | 크로스 이미지 (CheckBox) |
| `sprite2` | `sprite2` | 토글 상태 스프라이트 (ToggleButton) |
| `direction` | `direction` | 진행 방향 (ProgressBar) |
| `track_size` | `track_size` | 트랙 크기 (ScrollBar) |

### 1.3 XML 태그 ↔ ElementType 매핑

| XML 태그 | SGUIElementType | UIElementType (C++) |
|---|---|---|
| `<Group>` | Group (0) | Group (0) |
| `<Button>` | Button (1) | Button (1) |
| `<Label>` | Label (2) | Label (2) |
| `<Sprite>` | Sprite (3) | Sprite (3) |
| `<EditBox>` | EditBox (4) | EditBox (4) |
| `<CheckBox>` | CheckBox (5) | CheckBox (5) |
| `<ToggleButton>` | ToggleButton (6) | ToggleButton (6) |
| `<ScrollBar>` | ScrollBar (7) | ScrollBar (7) |
| `<ProgressBar>` | ProgressBar (8) | ProgressBar (8) |
| `<Static>` | Static (9) | Static (9) |

### 1.4 `<data>` 엘리먼트 — OnInit 전달용 메타데이터

Group 내부에 `<data>` 엘리먼트를 추가하여 툴에서 정의한 임의의 키-값 데이터를 XML 로딩 시점에 전달할 수 있음. 이 데이터는 `CDataMap`으로 변환되어 `OnInit(const CDataMap&)`에 전달됨.

```xml
<Group name="Popup" width="400" height="300">
    <data>
        <elem name="popupType" type="s32" value="1"/>
        <elem name="timeout" type="f32" value="5.0"/>
        <elem name="title" type="string" value="알림"/>
    </data>
    <Label name="titleLabel" x="0" y="10" width="400" height="40"
           text="타이틀" font="NanumGothic" font_size="24" font_color="255,255,255,255" />
    ...
</Group>
```

지원 타입: `s32`, `u32`, `s64`, `u64`, `f32`, `f64`, `string`, `bool`

---

## 2. 변경 대상 파일 목록

### 2.1 SteinsGate-Tools.UI (C# 툴 프로젝트) — 변경 파일

#### 모델 계층

| 파일 | 변경 내용 |
|---|---|
| `Project/Model/Main/SGUIElement.cs` | JSON 키 상수 제거, XML 속성명 상수로 대체. `ToJObject()`/`ParseJObject()` → `ToXElement()`/`ParseXElement()`. `Code`/`GroupCode`/`IsMaster`/`DefineName` 속성 **제거**. `Selected`/`Picked`/`DeleteSelf()` 단순화 (RootGroup 참조 변경). 모든 `GroupMaster` → `RootGroup` 참조 변경. |
| `Project/Model/Main/SGUIGroup.cs` | `ToXElement()`/`ParseXElement()` 구현. `code_` 필드 **제거**. `AddChild()` 단순화. |
| `SGUIGroupMaster.cs` → **`SGUIRootGroup`** | **클래스명 변경**. `codeAssigner_`/`groups_`/`elements_` **모두 제거**. `AddGroup()`/`RemoveGroup()`/`GetElementByCode()` **제거**. name 중복 검사 로직 추가. 마스터 개념 → 루트그룹 개념. |
| `Project/Model/Main/SGUIButton.cs` | XML 직렬화로 전환. `DefineName` 관련 코드 제거 |
| `Project/Model/Main/SGUILabel.cs` | 동일 |
| `Project/Model/Main/SGUISprite.cs` | 동일 |
| `Project/Model/Main/SGUIEditBox.cs` | 동일 |
| `Project/Model/Main/SGUICheckBox.cs` | 동일 |
| `Project/Model/Main/SGUIToggleButton.cs` | 동일 |
| `Project/Model/Main/SGUIProgressBar.cs` | 동일 |
| `Project/Model/Main/SGUIScrollBar.cs` | 동일 |
| `Project/Model/Main/SGUIStatic.cs` | 동일 |
| `Project/Model/Main/SGUIElementType.cs` | 변경 없음 (enum 유지) |
| `Project/Model/Setting/Setting.cs` | `OutputJsonPath` → `OutputXmlLayoutBasePath`로 변경. **`OutputDefineHeaderFilePath`/`OutputDefineHeaderFileName` 제거** (헤더 출력 폐기). uimeta 관련 설정만 유지. |

#### 파일 시스템 계층

| 파일 | 변경 내용 |
|---|---|
| `Project/FileSystem/SGUIFileSystem.cs` | 완전 재작성. JSON 키 상수 모두 제거. `.xml`/`.uimeta` 파일 단위 Load/Save. |
| `Project/FileSystem/SGUILoader.cs` | 완전 재작성. XML 중첩 구조 직접 파싱. `ParseXElement()` 호출. 코드 할당 로직 불필요. |
| `Project/FileSystem/SGUISaver.cs` | 완전 재작성. XML 직렬화. 중첩 트리 순회. 개별 `.xml` 파일 저장. |
| ~~`Project/FileSystem/SGUIExporter.cs`~~ | **파일 제거** — Define 헤더 생성 불필요 |
| ~~`Project/FileSystem/SGUIConverter.cs`~~ | **(계획에만 있었음, 신규 불필요)** |

**신규 파일:**

| 파일 | 설명 |
|---|---|
| `Project/FileSystem/SGUIMetaManager.cs` | `.uimeta` 파일 로드/저장/관리. 메타데이터: name, xml_path, last_modified. |
| `Project/FileSystem/SGUIProjectManager.cs` | 프로젝트 단위 관리. 디렉토리 열기 → 모든 `.uimeta` 스캔, 폴더 트리 구성. |

#### 뷰/뷰모델/컨트롤

| 파일 | 변경 내용 |
|---|---|
| `Project/View/MainView.xaml` | 트리뷰 ItemsSource 변경. 모든 `GroupMaster.*` → `RootGroup.*` 바인딩 변경. 폴더 트리 표시. |
| `Project/View/MainView.xaml.cs` | `GroupMaster` 참조 → `RootGroup` 변경. |
| `Project/View/SettingView.xaml` | **헤더 출력 관련 PropertyGrid 필드 제거** (`OutputDefineHeaderFilePath`, `OutputDefineHeaderFileName`). |
| `Project/ViewModel/MainViewModel.cs` | `GroupMaster` → `RootGroup`. `SGUIMetaManager` 기반 다중 루트 관리. `CurrentRootGroup` 속성 추가. `FolderTreeItems` 컬렉션 추가. |
| `Project/Customize/Control/UIElementTreeView.cs` | 폴더 노드 표시 지원. `GroupMaster` → `RootGroup` 참조 변경. |
| `Project/Customize/Control/UIElementItemsControl.cs` | `GroupMaster` → `RootGroup` 참조 변경. |

#### 명령 파일 (8개 파일 `GroupMaster`→`RootGroup` rename)

`CreateUIElement.cs`, `DeleteUIElement.cs`, `AddUIElement.cs`, `PickUIElement.cs`, `SelectUIElement.cs`, `PositionUIElement.cs`, `ClipboardOperateUIElement.cs`, `UnpickUIElement.cs`, `FileUIToolDataLoadAsync.cs`, `FileUIToolDataSaveAsync.cs`, `FileUIToolDataBackupAsync.cs`, `FileGameDataSaveAsync.cs`, `FileGameHeaderExportAsync.cs`

| 명령 파일 | 변경 내용 |
|---|---|
| `FileGameHeaderExportAsync.cs` | **파일 제거** — Define 헤더 익스포트 폐기 |
| `FileGameDataSaveAsync.cs` | **파일 제거** — GameData JSON 저장 불필요 |
| 나머지 명령 | 모든 `GroupMaster` → `RootGroup` 참조 변경 |

#### 기타

| 파일 | 변경 내용 |
|---|---|
| `Project/Constant.cs` | `GroupCodeInterval`, `CodeAssignerCapacity` **제거**. `.xml`, `.uimeta` 확장자 상수 추가. |
| `Project/Resource/R.cs` | 폴더 아이콘 키 추가. |
| `Project/Command/MainCommandCenter.cs` | 명령 목록 정리. `OpenUIProject`, `SaveCurrentGroup`, `SaveAllGroups` 등. `FileGameDataSaveAsync`/`FileGameHeaderExportAsync` 명령 제거. |
| `Project/Model/Backup/BackupFile.cs` | XML 로드 지원. |
| `Project/ModelTemplate/Main/` | 폴더 노드 DataTemplate 추가. |
| `Project/ModelSelector/Main/UIElementTreeViewSelector.cs` | 폴더 템플릿 선택 로직 추가. |

### 2.2 sgcl (C++ 런타임 프로젝트)

#### UI 시스템

| 파일 | 변경 내용 |
|---|---|
| `Classes/sgcl/Game/Contents/UIManager.h` | `Show(const char* _name, const CDataMap& _param = CDataMap())` 추가. `UILayer*` 멤버 추가. `REGISTER_UI` 팩토리 맵 추가. **`masterUIGroups_`/`uiElementMap_`/`RegisterRootGroup()` 제거**. 모든 `GetRootGroup`/`GetElement`/`GetButton`/`GetLabel` 등 전역 lookup 메서드 **제거**. |
| `Classes/sgcl/Game/Contents/UIManager.cpp` | `Show()` 구현. XML 로드 → RootGroup 생성. 팩토리 맵 관리. **`UIRootGroupManager` 제거**. `Init()`에서 JSON 기반 Init 로직 전부 제거. |
| `Classes/sgcl/Game/UI/UIRootGroup.h` | `OnInit(const CDataMap& _param)` 추가. `FindElementByName<T>(const char* _name)` 템플릿 메서드 추가. |
| `Classes/sgcl/Game/UI/UIRootGroup.cpp` | `FindElementByName()` 구현 (name 기반 재귀 탐색). |
| `Classes/sgcl/Game/UI/UIGroup.h` | XML 기반 `InitFromXml()` 메서드 추가. `InitChildren()` **제거**. `FindElementByName()` 계층 탐색 메서드 추가. |
| `Classes/sgcl/Game/UI/UIGroup.cpp` | `InitFromXml()` 구현 (tinyxml2로 XML 파싱, 자식 element 생성). 자식 생성 후 `nameMap_`에 name 등록. |
| `Classes/sgcl/Game/UI/UIElement.h` | **`code_` 필드 제거**. `GetName()` 추가 (XML `name` 속성값 저장). `InitFromXmlElement(XMLElement*)` 정적 팩토리. |
| `Classes/sgcl/Layer/Layer_UI.h` | 변경 최소화. `GetUILayer()` 유지. |

#### UI 구현체

기존 UI 구현체: REGISTER_UI 매크로 추가 + `g_cUIMgr.GetLabel(code)` → `this->FindElementByName<UILabel>("name")`로 변경

| 파일 | 변경 내용 |
|---|---|
| `UI_Login.h` | `REGISTER_UI(Login, UI_Login)` 매크로 추가. 모든 `UIElement*` 멤버의 코드 기반 lookup 제거 (OnInit에서 name으로 찾음). |
| `UI_Login.cpp` | `OnInit(const CDataMap&)`. `g_cUIMgr.GetLabel(UI_LOGIN_LABEL_SOURCE)` → `this->FindElementByName<UILabel>("labelSource")`. 모든 `g_cUIMgr.GetXxx(code)` → `FindElementByName<Xxx>(name)` |
| `UI_Inventory.h/.cpp` | 동일 |
| `UI_ChannelSelect.h/.cpp` | 동일 |
| `UI_Popup.h/.cpp` | 동일 |
| `UI_Test.h/.cpp` | 동일 |

#### 디스크립터 매니저

| 파일 | 변경 내용 |
|---|---|
| `DescMgr_UI.h` | XML 로더로 변경. tinyxml2 기반. JSON 키 상수 제거. `ReadElement*()` 시그니처 유지 (내부만 XML 속성 읽기로 변경). |
| `DescMgr_UI.cpp` | `Load()` XML 파싱으로 재작성. |

#### 씬

| 파일 | 변경 내용 |
|---|---|
| `Scene_Login.cpp` | `pUILayer_->AddUIGroup(GROUP_UI_LOGIN)` → `g_cUIMgr.Show("Login")` |
| `Scene_World.h` | `GetUILayer()` 유지. |
| `Scene_World.cpp` | 소멸자/`onExit()`에서 `g_cUIMgr.SetUILayer(nullptr)` 호출. |

#### 제거할 파일

| 파일 | 설명 |
|---|---|
| `UIRootGroupManager.h/.cpp` | JSON 기반 RootGroup 일괄 생성 제거 |
| `Define/Define_UI.h` | 전역 코드 상수(`GROUP_MASTER`, `GROUP_UI_LOGIN` 등) 제거 |
| `Define/Define_.h` (이미 삭제됨) | — |

#### 신규 파일

| 파일 | 설명 |
|---|---|
| `Game/UI/UIRegister.h` | `REGISTER_UI(name, class)` 매크로. 정적 초기화에서 UIManager 팩토리 맵에 등록. |
| `Game/UI/UIXmlLoader.h/.cpp` | tinyxml2 XML 파서 유틸리티. UIElementInfo 구조체로 변환. |

### 2.3 SG 프로젝트

| 파일 | 변경 내용 |
|---|---|
| `Sources/sg/Struct/SteinsGate_UI.h` | `UIElementInfo`에서 `code_` 필드 **제거**, `name_` 필드 추가. `UIGroupInfo`에 `CDataMap` 멤버 추가 (XML `<data>` 파싱 결과). `UIGroupElemInfo` 단순화. |

---

## 3. REGISTER_UI 매크로 시스템 설계

### 3.1 매크로 정의 (UIRegister.h)

```cpp
#pragma once
#include "sgcl/Game/UI/UIRootGroup.h"
#include "jc/Container/DataMap.h"

using UIFactoryFunc = UIRootGroup* (*)(UIGroupInfo* _pInfo);

#define REGISTER_UI(name, className)                                    \
    static UIRootGroup* Create##name##UI(UIGroupInfo* _pInfo) {         \
        return dbg_new className(_pInfo);                                \
    }                                                                    \
    namespace {                                                          \
        struct Register##name {                                          \
            Register##name() {                                           \
                UIManager::Get()->RegisterUIFactory(#name,               \
                    &Create##name##UI);                                   \
            }                                                            \
        } g_register_##name;                                             \
    }
```

### 3.2 UIManager 추가 멤버

```cpp
using UIFactoryMap = jc::HashMap<jc::String, UIFactoryFunc>;

void RegisterUIFactory(const char* _name, UIFactoryFunc _factory);
void SetUILayer(UILayer* _pLayer);

// name으로 XML 로드 + 등록된 팩토리로 생성 → UILayer에 추가
UIRootGroup* Show(const char* _name, const CDataMap& _param = CDataMap());

private:
    UIFactoryMap uiFactoryMap_;
    UILayer* pUILayer_ = nullptr;
```

### 3.3 Show() 동작 흐름

```
g_cUIMgr.Show("Login")
  → 1. 레이아웃 베이스 디렉토리에서 "Login.xml" 검색
  → 2. XML 파일 로드 → UIXmlLoader 파싱
  → 3. XML 최상위 <Group>의 name="Login" 확인
  → 4. XML <data> 엘리먼트 → CDataMap 변환
  → 5. factoryMap_에서 "Login" 검색
  → 6. 있으면: UI_Login* = factory("Login")(pInfo) 생성
  → 7. 없으면: UIRootGroup(pInfo) 기본 생성
  → 8. pRootGroup->InitFromXml() 호출 (XML 기반 자식 element 생성, name 등록)
  → 9. pRootGroup->OnInit(xmlDataMap) 호출
  → 10. pRootGroup->Load() (텍스처 등 로드)
  → 11. pUILayer_->addChild(pRootGroup, zOrder)
  → 12. pRootGroup->OnAdded()
  → 13. return pRootGroup
```

### 3.4 C++ element lookup 방식

```cpp
// UIRootGroup 내에 name → element 맵 유지
// OnInit에서 다음과 같이 사용
void UI_Login::OnInit(const CDataMap& _param) {
    pLabelSource_ = FindElementByName<UILabel>("labelSource");
    pBtnStart_ = FindElementByName<UIButton>("btnStart");
    pEditBoxId_ = FindElementByName<UIEditBox>("idInput");
    // ...
}
```

- `FindElementByName<T>()`는 RootGroup 내부의 `nameMap_`에서 name으로 1회 조회
- name 조회는 O(1) (HashMap), 계층 구조 무관
- XML 로딩 시 모든 element의 name이 `nameMap_`에 등록되며, 중복 시 assert/에러

### 3.5 툴에서의 name 중복 검사

- XML 저장 시점: DFS 순회하면서 name 중복 검사 (중복 시 저장 거부 + 에러 메시지)
- 툴 UI에서 element name 수정 시: RootGroup 내 name 중복 검사 (중복 시 수정 거부)
- 이름 규칙: RootGroup 내 모든 element의 name은 유일

---

## 4. UIMeta 파일 포맷

**Login.uimeta:**
```json
{
    "name": "Login",
    "xml_path": "Login\\Login.xml",
    "last_modified": "2026-07-18 12:00:00"
}
```

- `xml_path`는 XML 저장 베이스 디렉토리 기준 상대 경로
- **헤더 경로 정보 없음** (Define 헤더 생성 폐기)
- 저장 파일: `.xml` + `.uimeta` (2개 파일)

---

## 5. 폴더 트리 구조 (툴 UI)

### 5.1 디스크 구조
```
XML 레이아웃 베이스 디렉토리/
├── Login/
│   ├── Login.xml
│   └── Login.uimeta
├── ChannelSelect/
│   ├── ChannelSelect.xml
│   └── ChannelSelect.uimeta
└── Popup/
    ├── OkPopup/
    │   ├── OkPopup.xml
    │   └── OkPopup.uimeta
    └── Popup.uimeta
```

### 5.2 트리뷰 표시
```
📁 XML 레이아웃 베이스 디렉토리
  📁 Login
    🖼️ Login (RootGroup)
  📁 ChannelSelect
    🖼️ ChannelSelect
  📁 Popup
    📁 OkPopup
      🖼️ OkPopup
    🖼️ Popup
```

---

## 6. Setting.cs 변경

### 6.1 기존 속성
| 속성 | 변경 |
|---|---|
| `SgaDirectory` | 유지 |
| `OutputDefineHeaderFilePath` | **제거** (헤더 출력 폐기) |
| `OutputDefineHeaderFileName` | **제거** |
| `OutputJsonPath` | → `OutputXmlLayoutBasePath`로 대체 |
| `AutoExpandWhenSelected` | 유지 |
| `ShowLogViewWhenProgramLaunched` | 유지 |
| `LogViewPositionWhenProgramLaunched` | 유지 |
| `ZoomLevel` | 유지 |

### 6.2 신규 속성
| 속성 | 타입 | 기본값 | 설명 |
|---|---|---|---|
| `OutputXmlLayoutBasePath` | string | `""` | XML 레이아웃 파일 저장 베이스 디렉토리 |

---

## 7. 구현 순서

### Phase 1: C# 모델 XML 직렬화
1. `SGUIElement.cs` 및 모든 서브클래스: `ToXElement()`/`ParseXElement()` 추가, `Code`/`DefineName` 제거
2. `SGUIGroupMaster.cs` → `SGUIRootGroup` rename + `codeAssigner`/`groups_`/`elements_`/`AddGroup`/`RemoveGroup` 제거 + name 중복 검사 추가
3. `SGUILoader.cs` → XML 로더 재작성
4. `SGUISaver.cs` → XML 세이버 재작성
5. `SGUIFileSystem.cs` → XML 기반 재작성
6. `SGUIMetaManager.cs` 신규
7. `SGUIProjectManager.cs` 신규
8. ~~`SGUIExporter.cs`~~ → **제거**
9. `SGUIElement.cs`에서 `GroupMaster` → `RootGroup` 전면 변경
10. `Constant.cs`에서 `GroupCodeInterval`/`CodeAssignerCapacity` 제거

### Phase 2: 툴 UI/UX + Setting 정리
1. `Setting.cs`: `OutputDefineHeaderFilePath`/`OutputDefineHeaderFileName` 제거, `OutputXmlLayoutBasePath` 추가
2. `SettingView.xaml`: 헤더 필드 제거
3. `MainViewModel.cs` `GroupMaster` → `RootGroup`, `SGUIMetaManager` 기반 관리
4. 모든 명령 파일 `GroupMaster` → `RootGroup` rename
5. `FileGameHeaderExportAsync.cs` / `FileGameDataSaveAsync.cs` 명령 제거
6. `UIElementTreeView.cs` 폴더 트리 표시 + name 기반 선택
7. `MainView.xaml` XAML 바인딩 전면 변경
8. `R.cs` 폴더 아이콘, `Constant.cs` XML 확장자
9. `MainCommandCenter.cs` 명령 정리

### Phase 3: tinyxml2 서드파티 추가
1. `vcpkg install tinyxml2`
2. `ThirdParty/tinyxml2/` 프로젝트 생성 (JsonCpp195 구조 참고)
3. sgcl에 종속성 추가

### Phase 4: C++ XML 파싱 및 UIGroup
1. `UIXmlLoader.h/.cpp` 신규 (tinyxml2 기반)
2. `DescMgr_UI.h/.cpp` XML 파싱 재작성. `SteinsGate_UI.h` `name_`/`CDataMap` 추가, `code_` 제거
3. `UIGroup.h/.cpp` `InitFromXml()` + `nameMap_` 추가. `InitChildren()` 제거
4. `UIElement.h` `code_` 제거, `name_` 추가, `InitFromXmlElement()` 추가
5. `UIRootGroupManager.h/.cpp` 제거
6. `Define_UI.h` 제거

### Phase 5: C++ UIMgr Show + REGISTER_UI
1. `UIRegister.h` 신규
2. `UIManager.h/.cpp` `Show()`, `SetUILayer()`, 팩토리 맵. `uiElementMap_`/`masterUIGroups_`/전역 getter 제거
3. `Scene_World.cpp` UILayer null 처리
4. `Scene_Login.cpp` `g_cUIMgr.Show("Login")`
5. `UIRootGroup.h/.cpp` `OnInit(const CDataMap&)`, `FindElementByName<T>()`
6. 모든 UI_Implementation: REGISTER_UI 추가 + `g_cUIMgr.GetXxx(code)` → `FindElementByName<Xxx>("name")`

### Phase 6: 통합 테스트
1. 툴에서 Login.xml 저장/로드
2. C++에서 Login.xml 로드 후 element name 기반 lookup 검증
3. REGISTER_UI → Show 플로우 검증
4. name 중복 검증 (툴 + C++)

---

## 8. 의존성

- `System.Xml.Linq` (C#, 기본 내장)
- `Newtonsoft.Json` (C#, uimeta JSON 포맷용)
- `tinyxml2` (C++, vcpkg 설치)

---

## 9. 리스크 및 고려사항

1. **기존 ui.json 호환성 무시** — 다시 저장하면 XML 형식으로 저장
2. **code/id 체계 완전 폐기** — 모든 element를 `name`으로 식별. RootGroup 내 name 중복 불가
3. **Define 헤더 폐기** — `SGUIExporter.cs` 제거. `OutputDefineHeader*` 설정 제거. 전역 코드 상수(`GROUP_MASTER` 등) 제거
4. **name 중복 검사** — 툴 저장 시점 + C++ XML 로딩 시점 모두 검사 필요
5. **C++ UIManager 단순화** — `uiElementMap_`/`masterUIGroups_`/전역 getter 모두 제거. 팩토리 맵 + Show()만 남음
6. **UI_Implementation 코드 변경** — 모든 `g_cUIMgr.GetXxx(defineCode)` → `this->FindElementByName<Xxx>("name")`로 변경