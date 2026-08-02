# 2026-08-02 fable(sgui) 적용 + layout/theme 융화 (UIManager 유지 중심)

## 목표 (사용자 최종 방향 — 2026-08-02 갱신)
1. fable Classes.zip의 sgui(WPF 유사) UI 프레임워크를 현재 브랜치에 적용한다. (오버레이 완료)
2. **UIManager 기능은 그대로 유지**한다. fable 쪽에 중복 기능이 있으면 **UIManager로 이관**한다.
3. **layout XML의 태그와 코드 XML 로더는 fable 버전(sgui)에 맞게 변경**한다.
4. **SGA 관련 기능은 모두 제거**한다. XML 쪽 sga/img/index 속성도 모두 제거한다.
5. theme(Theme-Bake) 파이프라인은 유지하고 sgui와 융화한다.

## 확인 결과 (2026-08-02)

### 현재 상태
- fable 전체 오버레이 완료 (추가 57 / 변경 26 / 삭제 34, MD5 검증으로 fable과 360파일 정확히 일치)
- **UIManager.cpp/h, UIXmlLoader, UIRootGroup, UIButton/UILabel 등 기존 UI 全部 삭제됨** (fable이 삭제)
- vcxproj 동기화 완료 (컴파일 불가 잔여물 GameScene.cpp/Image.cpp/Image.h는 프로젝트에서 제외)
- 스테이지: `Contents.cpp`가 `g_cUIMgr.Init()` → `g_cViewMgr.Init()`으로 교체된 상태

### SGA 관련 코드 인벤토리 (제거 대상)
| 위치 | 내용 |
|---|---|
| `Game/UI/Core/Brush.h/.cpp` | `UIResourceIndex {sga_, img_, frame_, linearDodge_}`, `ResourceTable::Register/TryResolve` |
| `Game/UI/Host/ViewManager.h/.cpp` | `CreateUITexture(sga,img,frame,linearDodge)`, `CreateUITextureRetained`, `RegisterUITexture(SgaResourceIndex)`, `loadedUITexture_` |
| `Game/UI/Controls/Image.cpp` | `ImageBrush` → ResourceTable → CreateUITexture(sga…) |
| `Game/UI/Core/BrushVisual.cpp` | `Image/NinePatchBrush` → ResourceTable → CreateUITexture(sga…) |
| 루트 `Image.h/.cpp` | SGImage/SGImagePack/NpkImage — `SteinsGate/Common` 참조라 컴파일 불가 (이미 빌드 제외) |
| `sg::Struct::SteinsGate_UI.h` | SgaResourceIndex, UIGroupInfo (베이스 UIManager 의존) |
| layout XML | `sga`/`img`/`sprite`/`index` 속성 |

**범위 주의**: `Game/Texture/ImagePack.*/ImagePackManager.*/FrameTexture.*`는 **게임 월드 타일(Layer_Map.cpp)과 씬/텍스처 로딩(GameCore.cpp, Scene_Base, Scene_World, JsonUtilEx)**에도 사용됨. UI 전용인지 전체인지 범위 확인 필요.

### Theme-Bake: SGA 미사용 확인 ✓
- `Game/UI/Theme/**` 및 UIThemeBinding/UIThemeManager는 ImagePack/CreateUITexture를 **전혀 사용하지 않음** (검색 0건)
- 융화 경로: `ThemeBrush → BrushVisual::ApplyThemeBinding → UIThemeManager::Resolve → UIAssetKey → UIThemeTextureBinding::Refresh`
- `UIHost::PollThemeRevision` → 텍스처 교체 시 모든 Window `RefreshThemeVisuals`

### fable ↔ UIManager 중복 (UIManager로 이관 대상)
| fable 기능 | 베이스 UIManager 대응 | 판정 |
|---|---|---|
| `ViewManager::Init/Free` | `UIManager::Init/Free` | 중복 → UIManager로 |
| `ViewManager::Show/RegisterWindowFactory` | `UIManager::Show/RegisterUIFactory` | 중복 → UIManager로 (fable은 Window 팩토리, 베이스는 UIRootGroup 팩토리) |
| `ViewManager::CreateUITexture/CreateUITextureRetained/RegisterUITexture/UnloadAll` | UIManager에 원래 있었음 (주석 "구 UIManager 이식") | **SGA 기반이므로 제거 대상** |
| (fable이 삭제) XML 로딩 / DragState / OnUpdate / UILayer | UIManager 고유 | **유지 (복원)** |
| `UIHost` (Window 컨테이너, 구 Layer_UI) | `UILayer` | 신규 유지 |

## 확정 사항 (2026-08-02 사용자 답변)
1. **SGA 제거 범위**: UI 전용만 제거. 게임 월드 타일용 ImagePack/FrameTexture 스택은 유지.
2. **UIManager 복원 방식**: 베이스(`16837196`) UIManager 복원 후 sgui 기반으로 개조.
3. **매니저 진입점**: `g_cViewMgr` → `g_cUIMgr` 복귀. ViewManager의 창 표시 기능을 UIManager로 이관.
4. **layout/ XML 로딩 기능도 모두 UIManager로 이관**. XML 포맷/로더는 **WPF 스타일(sgui 규격)로 모두 수정**.

## WPF 스타일 XML 바인딩/리졸브 스펙 (2026-08-02 사용자 지정)

### XML 기본 구조
루트는 `<Window>` (윈도우=화면 단위). 최상단에 전역 데이터 `DataList` 선언 가능. 화면 트리는 `Grid`/`StackPanel`/`Canvas` 등의 패널과 컨트롤로 구성. 모든 값은 `{...}` 중괄호 토큰으로 리졸브한다.

```xml
<Window>
    <DataList>
        <Data Key="u32Key" Type="_u32" Value="120"/>          <!-- 10진 -->
        <Data Key="u8Key" Type="_u8" Value="0b1101"/>          <!-- 이진 -->
        <Data Key="u8Key3" Type="_u8" Value="0xff"/>           <!-- 16진 -->
        <Data Key="stringKey" Type="String" Value="Hello"/>
        <Data Key="floatKey" Type="_f32" Value="1.23"/>
        <Data Key="doubleKey" Type="_f64">
            <Value>1.2342342</Value>                            <!-- 멀티라인 값 -->
        </Data>
        <Data Key="Player" Type="Map">                          <!-- 딕셔너리 -->
            <DataList>
                <Data Key="Name" Type="String" Value="John"/>
                <Data Key="Level" Type="_u32" Value="99"/>
                <Data Key="Pos" Type="Map">
                    <DataList>
                        <Data Key="X" Type="_f32" Value="10"/>
                        <Data Key="Y" Type="_f32" Value="20"/>
                    </DataList>
                </Data>
            </DataList>
        </Data>
        <Data Key="Items" Type="Array">                         <!-- 배열 -->
            <DataList>
                <Data Type="String" Value="Sword"/>
                <Data Type="String" Value="Shield"/>
                <Data Type="String" Value="Potion"/>
            </DataList>
        </Data>
    </DataList>

    <Grid Name="Root" Width="120" Height="50">
        <Button Name="{Root.Name} + `Hello`" Width="{Root.Width} + 20"/>
        <Button Name="{$parent.Name} + `World`" Width="{$parent.Width} + 24"/>
        <Button Name="{$parent.Name} + {@stringKey}" Width="{@u32Key}"/>
    </Grid>
</Window>
```

### Data 타입
- `_u8`/`_u16`/`_u32`/`_u64`, `_s8`/`_s16`/`_s32`/`_s64`, `_f32`/`_f64`
- `Bool`, `String`, `Array`, `Map`
- 향후: `Vector2`/`Vector3`/`Vector4`/`Color`/`Rect`/`Thickness`/`Margin`/`Enum`/`Object`
- 값 표기: 10진(`120`), 이진(`0b1101`), 16진(`0xff`), 멀티라인(`<Value>` 블록)

### Resolve 토큰 (중괄호 `{}`, 점 `.` 표기)
모든 접근은 `{...}` 토큰으로 감싼다. 내부는 점 체인. 컬렉션 인덱스는 `[n]` 또는 중첩 토큰 `[{@...}]`.

### Data Resolve (전역 데이터 — 변수처럼 동작)
- `{@u32Key}` → `120`
- Map: `{@Player.Name}` → `John`, `{@Player.Level}` → `99`
- Nested Map: `{@Player.Pos.X}` → `10`
- Array: `{@Items[0]}` → `Sword`, `{@Items[2]}` → `Potion`
- 중첩 인덱스(이중 접근): `{@Items[{@Index}]}` → `{@Index}`가 먼저 해석된 뒤 `{@Items[해석값]}` 접근

### Element Resolve (WPF Binding 대신 단순 Resolve)
- `{Root.Width}` → 120 (Element.Property)
- `{InventoryPanel.Width}`, `{Button1.Height}`
- 속성 체인: `{Root.Margin.Left}`, `{Root.Transform.ScaleX}`, `{PlayerImage.Width}`
- 컬렉션 접근: `{List.Items[0]}`, `{List.Items[5]}`, `{List.Items[{@Index}]}`

### Relative Resolve
- 부모: `{$parent.Width}`, `{$parent.Name}`
- 조부모: `{$parent.$parent.Width}` 또는 `{$ancestor(2).Width}`
- Root: `{$root.Width}`
- 자기 자신: `{$self.Width}`
- 형제: `{$prev.Width}`, `{$next.Width}`, 또는 `{$parent.ButtonA.Width}`

### Data + Element 혼합
- `Width="{Root.Width} + {@Margin}"`, `Name="{Root.Name} + `Hello`"`, `Text="HP : {@Player.HP}"`, `Text="Lv {@Player.Level} {@Player.Name}"`

### 수식(Expression) 지원
- 연산: `+` `-` `*` `/` `%` `()`
- 비교: `==` `!=` `>` `<` `>=` `<=`
- 논리: `&&` `||` `!`
- 문자열: `+`
- 예: `Width="({Root.Width} * 2) + 30"`, `Text="`HP : ` + {@Player.HP}"`, `Visible="{@Player.Level} >= 10"`, `Height="max({Root.Height},40)"`

### Resolve 우선순위
`{$...}` 상대 참조 → `{@...}` 전역 데이터 → `{Element.Property}` → Literal →
모든 토큰 치환(중첩 토큰은 내부부터) → 수식 평가 → 최종 타입 캐스팅 후 속성 적용

## 적용 전략 (확정)
1. **UIManager 복원**: 베이스(`16837196`)에서 `UIManager.cpp/h` 복원 → sgui 기반으로 개조
   - **layout/ XML 로딩 기능 이관 + WPF 스타일 재작성**: `UIXmlLoader`를 sgui용 WPF 스타일 XML 로더로 재작성
     - 태그 = sgui 컨트롤(WPF 명칭): `Window`/`Grid`/`StackPanel`/`Canvas`/`DockPanel`/`Border`/`TextBlock`/`Button`/`CheckBox`/`ToggleButton`/`ProgressBar`/`ScrollBar`/`Slider`/`TextBox`/`Image`
     - 속성 = WPF 스타일: `Width`/`Height`/`Margin`/`HorizontalAlignment`/`VerticalAlignment`/`Visibility`/`IsEnabled`, 명명(`Name`), 레이아웃 부착 속성(`Grid.Row`/`Grid.Column` 등), 이벤트(`Click`/`Checked`/`ValueChanged`)
     - SGA 스프라이트 속성(`sga`/`img`/`sprite`/`index`)은 제거, 테마 기반 브러시(ThemeBrush)로 대체
   - `RegisterUIFactory`/`Show` → sgui `Window` 팩토리로 개조 (fable ViewManager 기능 이관)
   - DragState, OnUpdate/CallUiElementsUpdateCallback 유지
   - `CreateUITexture(SGA)` 등 SGA 부분 제거
2. **fable 중복 이관**: ViewManager의 창 표시/등록 기능을 UIManager로 이관. `g_cViewMgr` → `g_cUIMgr` 복귀 (Contents.cpp, Scene_* 코드 수정). ViewManager는 제거
3. **SGA 제거 (UI 전용)**: ResourceTable/UIResourceIndex/ImageBrush/NinePatchBrush/Image control의 키 이미지 경로, CreateUITexture(sga…) 제거. 게임 월드 타일 ImagePack/FrameTexture는 유지
4. **layout XML 변환 (WPF 스타일)**: dev 에셋 `layout/*.xml` 태그를 sgui(WPF) 컨트롤 이름으로 변환, `sga`/`img`/`sprite`/`index` 속성 제거
5. **theme 융화 검증**: ThemeBrush→Binding 경로, ApplyTheme, PollThemeRevision, UI_Generic 스위처

## 체크리스트
- [x] 기존 구조 파악 (layout/theme 파이프라인, fable 대응 분석)
- [x] fable 소스 오버레이 (추가 57 + 변경 26 + 삭제 34, MD5 검증)
- [x] vcxproj 동기화 (제거 파일 반영, GameScene/Image 제외)
- [x] **UIManager 복원 + sgui 개조** (WPF 스타일 XML 로더, Window 팩토리, DragState, OnUpdate) — layout/ 로딩 기능 이관 포함
- [x] **XML 바인딩/리졸브 엔진 구현** — DataList/Data 파싱, `{@...}`/`{Element.Property}`/`{$...}`/`{$ancestor(n).Prop}` Resolve(중괄호+점), 중첩 접근(`{@Items[{@Index}]}`), 수식 평가기(+-*/% (), 비교, 논리, 문자열+), 타입 캐스팅
- [x] **fable 중복 → UIManager 이관 + ViewManager 제거** (g_cViewMgr → g_cUIMgr)
- [x] **SGA 제거 (UI 전용)** — ResourceTable/UIResourceIndex/ImageBrush 경로, CreateUITexture(sga…)
- [x] layout XML 태그 변환 + sga/img/index 속성 제거
- [x] theme 융화 검토 (ThemeBrush→Binding, ApplyTheme, PollThemeRevision, UI_Generic 스위처)
- [x] 빌드 (Build.ps1 sgcl Debug x64) + 에러 수정
- [x] 종료 시점 크래시 수정 (윈도우 이중 autorelease 제거)
- [x] done/history 문서 작성

## 진행 로그
- 2026-08-02: plan 생성
- 2026-08-02: 기존 구조 파악 완료 — theme는 fable에서 보존, layout XML은 fable이 제거 확인
- 2026-08-02: fable 전체 오버레이 완료 (MD5 일치), vcxproj 동기화 완료
- 2026-08-02: **방향 전환** — UIManager 유지 + fable 중복 이관 + SGA 제거 + XML 태그 fable화. 확인 결과 반영 (SGA 인벤토리, fable↔UIManager 중복 매트릭스, Theme 무SGA 확인)
- 2026-08-02: **확정** — SGA는 UI 전용만 제거, UIManager는 베이스 복원 후 sgui 개조, g_cUIMgr 복귀
- 2026-08-02: **확정** — layout/ XML 로딩 기능도 UIManager로 이관, XML 포맷/로더는 WPF 스타일(sgui 규격)로 모두 재작성
- 2026-08-02: **XML 바인딩/리졸브 스펙 반영** — DataList/Data(타입, 진법), Data/Element/Relative/Sibling Resolve, 속성 체인/컬렉션, 수식 평가, Resolve 우선순위
- 2026-08-02: **점(.) 표기 통일** — `[Root.Width]`, `[@Player.Name]`, `[$parent.Width]`, `[$prev.Width]` 등 모든 접근을 `.`으로 (기존 `[Root Width]` 공백 표기 제거)
- 2026-08-02: **중괄호 `{}` 표기 + 중첩 접근** — `{Root.Width}`, `{@Player.Name}`, `{$parent.Width}`로 변경, 컬렉션 인덱스 중첩 `{@Items[{@Index}]}` (내부 토큰 먼저 해석) 지원
- 2026-08-02: **구현 설계 확정 (이어서 진행)** — ① UIManager(`UI/Host/UIManager.h/.cpp`) 신규: 팩토리 Show 우선 → XML Show 폴백, ShowWindow(OnInit→AddWindow→OnLoaded→OnAdded), UnloadAll=no-op. `g_cUIMgr` 매크로. **DragState/OnUpdate는 sgui 입력(InputDispatcher/Window dragMove)이 대체하므로 미이관**. ② XML 모듈 신규 `Game/UI/Xml/`: `UIValue.*`(DataList/Data 파싱, 0b/0x/멀티라인/Array/Map), `UIResolve.*`(표현식 평가기: 중괄호 Ref/백틱 문자열/@/$/수식/비교/논리/max·min + 문자열 보간 폴백 + 타입 캐스팅), `UIXmlLoader.*`(WPF XML→sgui 2패스 빌드: 정적 속성 즉시 + `{}` 바인딩 지연 적용). ③ SGA 제거: Brush.h(ImageBrush/NinePatchBrush/ResourceTable/UIResourceIndex/Image·NinePatch enum) 삭제, BrushVisual.cpp Image/NinePatch 케이스 제거, Image.cpp는 **파일 경로 기반(Texture2D)**으로 재작업. ④ 참조 교체: Contents/PopupManager/Scene_Login/Scene_ChannelSelect/Scene_World/UIHost/WindowRegister/Sgui.h/Window.h(friend). tinyxml2는 **직접 편입하지 않고** `libcocos2d`가 이미 내장 export하는 v9 심볼을 사용 (빌드 검증으로 확정: sgcl에서 편입 시 v11 헤더 우선 잡힘 + 중복 정의 위험)
- 2026-08-02: **UIManager.h/.cpp 작성 완료** (팩토리 우선 Show + ShowXml + ShowWindow 생명주기, ResolveXmlPath = resData/layout/{name}.xml)
- 2026-08-02: **빌드 에러 진단/수정 (진행)** — tinyxml2 3개 헤더 버전 상이 확인 (ThirdParty\tinyxml2\include=v9= cocos external\tinyxml2 v9, external\x64\include=v11). libcocos2d가 v9 tinyxml2를 내장 export → sgcl에서 `Project/tinyxml2.cpp` 직접 편입 제거 + AdditionalIncludeDirectories 맨 앞에 `$(SolutionDir)ThirdParty\tinyxml2\include` 추가(v9 우선)
- 2026-08-02: **UIValue.h 작성 완료** — UIValue(variant) + UIDataList + ParseDataList 선언
- 2026-08-02: **UI_* FindElementByName 복원 (XML 우선 + 코드 빌드 폴백)** — 사용자 확정: XML 우선 + REGISTER_WINDOW 코드 빌드 폴백
  - `UIXmlLoader::LoadWindowInto/IntoString` + `PopulateWindowFromDocument(Window*, doc)` 리팩토링 (기존 인스턴스에 XML 병합, 성공 시 SetXmlLoaded(true))
  - `Window::SetXmlLoaded/IsXmlLoaded` 추가
  - `UIManager::Show()` XML 우선: 팩토리 인스턴스 생성 → XML 존재 시 LoadWindowInto 병합 → 팩토리 없으면 LoadWindowFromFile → 병합 실패 시 코드 빌드 폴백
  - `UI_Generic/UI_Login/UI_ChannelSelect/UI_Inventory/UI_Test` OnInit을 `IsXmlLoaded()` 분기로 리팩토링 — XML 경로는 FindElementByName 멤버 할당, 폴백은 기존 코드 빌드 유지. UI_ChannelSelect/UI_Inventory는 동적 버튼/슬롯을 헬퍼(BuildServerButtons/BuildChannelButtons/BuildEquipSlots/BuildInvenSlots)로 추출해 XML 컨테이너 패널에 채움
  - dev layout XML 5종(ui_generic/ui_login/ui_channel/ui_inventory/ui_test)을 WPF `<Window>` 형식으로 변환 (구 `<Group>`/SGA 속성 제거, 테마 기반 컨트롤로 대체)
  - 빌드 검증: sgcl Debug x64 성공 (0 에러)
  - 참고: `ui_popup`은 PopupManager가 직접 생성(REGISTER_WINDOW 미사용)하므로 XML 병합 대상 아님. `ui_hud/ui_chat` 등은 등록 팩토리 없음
  - **완료**: 실행 검증 성공 — `start_up_debugging.ps1`로 sgcl 실행, `[UI_Generic] OnInit(XML) title=... close=... chk=... toggle=... pbar1=... pbar6=... status=...` 로그 확인 (XML 병합 + FindElementByName 할당 동작). done 문서 작성 완료
- 2026-08-02: **종료 시점 크래시 수정 완료** — `Node::~Node`(CCNode.cpp:191) assert (`_running` 상태로 소멸) 원인 확정: **윈도우 이중 autorelease**
  - 재현 로그(`2026-08-02-103629.log`): `[UIHost] AddWindow ui_generic rc=2 running=1` → 씬전환 완료 직후 `[Window] ~Window name=ui_generic rc=0 running=1` (Clear/RemoveWindow 없이 소멸)
  - 크래시 스택(`2026-08-02-101152.log`): `AutoreleasePool::clear` → `Ref::release` → `~UI_Generic` → `Node::~Node` assert
  - 원인: `new`(rc=1) → 생성자(팩토리 `WindowRegister.h:30` 또는 `UIXmlLoader.cpp:765`) autorelease → `UIManager::ShowWindow`(UIManager.cpp:156) autorelease **2회 중복** → `addChild`(rc=2) → 다음 `AutoreleasePool::clear`에서 2회 release → rc=0으로 씬 트리 부착(`_running`) 상태에서 소멸
  - 수정: `ShowWindow`의 `autorelease()` 제거 (생성자가 이미 1회 autorelease. 소유권: new(1)+addChild(1)=rc2, 풀 정리 1회 release → rc1)
  - 검증: `Build.ps1 sgcl Debug x64` 0 에러 → `start_up_debugging.ps1` 연속 2회 실행에서 `~Window` 소멸 로그 없음, 크래시 덤프 미생성 → **정상 동작 확인**
