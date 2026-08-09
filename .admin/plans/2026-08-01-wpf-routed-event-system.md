# WPF 스타일 Routed Event + Grid/StackPanel UI 재설계

**Date:** 2026-08-01
**Branch:** `sg-main`
**작업명:** `wpf-routed-event-system`

---

## 1. 목표

현재 sgcl UI 시스템을 WPF와 유사한 구조로 전면 재설계한다.

1. **Routed Event 시스템** — `RaiseEvent()` 호출 시 버블링(Bubble)/터널링(Tunnel)/다이렉트(Direct) 규칙에 따라 Visual Tree(=부모 체인)를 따라 이벤트가 전파된다.
2. **중간 부모도 이벤트 수신** — 현재 `UIButton → UIRootGroup` 직접 전달 구조를 버리고, `UIButton → UIGroup → UIRootGroup` 순으로 상위로 전파한다.
3. **`On*` 핸들러 규칙** — 각 Routed Event마다 `UIElement::OnXxx(RoutedEventData&)` 가상 함수를 제공한다. 각 요소(중간 그룹 포함)는 이를 오버라이드하여 이벤트를 수신한다.
4. **Grid / StackPanel 레이아웃 패널** — 절대좌표 대신 자동 배치가 가능한 레이아웃 컨테이너를 추가한다.
5. **기존 UI 컨트롤(Button/CheckBox 등) 및 XML 호환** — 신규 시스템 위에서 기존 기능을 그대로 동작시킨다.

---

## 2. 현재 구조 분석

### 2.1 클래스 계층

```
UIRootGroup
  └── UIGroup
        ├── UIButton
        ├── UICheckBox
        ├── UILabel / UIStatic / UISprite
        └── ...
```

- `UIRootGroup : UIGroup : UIElement : cc::Node`
- `UIElement` 에 `pParent_`(직속 부모 UIGroup)와 `pRootGroup_`(루트) 참조가 있음
- 자식은 `_children`(cc::Vector)에 저장됨

### 2.2 현재 이벤트 흐름 (Mouse Down 예시)

```
UILayer / WorldScene 입력
  └── UIGroup::OnMouseDownInternal()  ← 자식 역순(맨 위→아래) 순회
        └── UIButton::OnMouseDownInternal()
              ├── (1) pRootGroup_->OnMouseDownTarget(this, evt)   ← 루트로 직접 점프!
              ├── (2) InvokeMouseEvent(eMouseEventDown, evt)      ← 자기 로컬 delegate
              ├── (3) OnMouseDownInternalDetail(evt)              ← 구현체 로직(상태변경 등)
              └── return bPropagate(false면 순회 중단)
```

### 2.3 현재 구조의 문제점

| 문제 | 설명 |
|------|------|
| **중간 부모 무시** | `UIGroup`은 자식에게 전달만 할 뿐, 자식 이벤트를 **자신의 이벤트로 수신하지 못함**. 오직 대상 요소와 루트만 앎 |
| **버블링 없음** | child → parent → root 순서의 전파가 없음. 이벤트가 루트로 단숨에 점프 |
| **터널링/버블링 쌍 없음** | `PreviewMouseDown(터널) → MouseDown(버블)` 같은 2단계 구조가 없음 |
| **Handled 개념 없음** | `bPropagate`(bool) 기반이라 핸들러가 "이 이벤트를 처리했다"는 상태를 라우트에 반영 불가 |
| **수신 인터페이스 비일관성** | 루트는 `OnMouseUpTarget(UIElement*, EventMouse*)`, 로컬은 `InvokeMouseEvent`, 구현체는 `OnMouseUpInternalDetail` — 3중 구조 |

---

## 3. WPF 개념 매핑

| WPF | sgcl 신규 설계 | 비고 |
|-----|---------------|------|
| `RoutedEvent` | `RoutedEvent` 구조체 | name + strategy |
| `RoutingStrategy` (Bubble/Tunnel/Direct) | `RoutingStrategy` enum | 동일 |
| `RoutedEventArgs` | `RoutedEventData` | `RoutedEvent`, `Source`, `MouseEvent`, `Handled` |
| `EventRoute` | `EventRoute` | 부모 체인을 따라 만든 전파 경로 |
| `RouteItem` | `RouteItem` | 경로상의 단일 요소 |
| `EventHandlersStore` | `EventHandlersStore` | 요소별 `RoutedEvent → 핸들러 목록` 저장소 |
| `AddHandler` / `handledEventsToo` | `AddHandler(evt, fn, handledEventsToo)` | 동일 |
| `UIElement.RaiseEvent()` | `UIElement::RaiseEvent(RoutedEventData&)` | 경로 생성 + 순차 호출 |
| 클래스 핸들러(`OnClick` 등) | `virtual void OnXxx(RoutedEventData&)` | 오버라이드 기반 |
| Visual Tree | `pParent_` 체인 (UIElement → UIGroup → … → UIRootGroup) | |

---

## 4. 핵심 클래스 설계

### 4.1 새 파일 배치

```
sgcl/Game/UI/Event/
  RoutedEvent.h            — RoutingStrategy, RoutedEvent 구조체, 전역 이벤트 정의
  RoutedEventData.h        — 이벤트 인자
  EventRoute.h             — 경로 생성/저장
  EventHandlersStore.h     — 요소별 핸들러 저장소
sgcl/Game/UI/Layout/
  UIPanel.h/.cpp           — 레이아웃 패널 공통 기반 (추상)
  UIGrid.h/.cpp            — 그리드 레이아웃
  UIStackPanel.h/.cpp      — 스택 레이아웃
```

### 4.2 RoutedEvent (이벤트 정의)

```cpp
enum class RoutingStrategy : unsigned char
{
    Tunnel,   // 루트 → 소스 (Preview*)
    Bubble,   // 소스 → 루트
    Direct    // 소스 자신만
};

struct RoutedEvent
{
    const char* name_;          // "MouseDown"
    RoutingStrategy strategy_;  // Bubble
    UIElementType_t ownerType_; // 이벤트를 발생시키는 요소 타입
};

// 전역 정의 (헤더 인라인)
namespace UIRoutedEvents
{
    inline const RoutedEvent PreviewMouseDown { "PreviewMouseDown", RoutingStrategy::Tunnel,   UIElementType::Button };
    inline const RoutedEvent MouseDown        { "MouseDown",        RoutingStrategy::Bubble,   UIElementType::Button };
    inline const RoutedEvent PreviewMouseUp   { "PreviewMouseUp",   RoutingStrategy::Tunnel,   UIElementType::Button };
    inline const RoutedEvent MouseUp          { "MouseUp",          RoutingStrategy::Bubble,   UIElementType::Button };
    inline const RoutedEvent MouseMove        { "MouseMove",        RoutingStrategy::Bubble,   UIElementType::Button };
    inline const RoutedEvent MouseEnter       { "MouseEnter",       RoutingStrategy::Direct,   UIElementType::Button };
    inline const RoutedEvent MouseLeave       { "MouseLeave",       RoutingStrategy::Direct,   UIElementType::Button };
    inline const RoutedEvent MouseScroll      { "MouseScroll",      RoutingStrategy::Bubble,   UIElementType::Button };
    inline const RoutedEvent Click            { "Click",            RoutingStrategy::Bubble,   UIElementType::Button };
    inline const RoutedEvent CheckedChanged   { "CheckedChanged",   RoutingStrategy::Bubble,   UIElementType::CheckBox };
    inline const RoutedEvent ToggleChanged    { "ToggleChanged",    RoutingStrategy::Bubble,   UIElementType::ToggleButton };
    inline const RoutedEvent Drag             { "Drag",             RoutingStrategy::Bubble,   UIElementType::Group };
    inline const RoutedEvent TextChanged      { "TextChanged",      RoutingStrategy::Bubble,   UIElementType::EditBox };
}
```

### 4.3 RoutedEventData (이벤트 인자)

```cpp
class RoutedEventData
{
public:
    const RoutedEvent* pRoutedEvent_ = nullptr;
    UIElement* pSource_ = nullptr;          // 이벤트를 Raise한 요소
    cc::EventMouse* pMouseEvent_ = nullptr; // 원본 마우스 이벤트 (무관 시 nullptr)
    void* pUserData_ = nullptr;             // Click 등에서 상태 전달 (예: bool* checked)
    bool handled_ = false;                  // 핸들러에서 true 설정 시 전파 중단

    bool Handled() const { return handled_; }
    void SetHandled(bool b) { handled_ = b; }
};
```

### 4.4 EventRoute (경로)

```cpp
struct RouteItem { UIElement* pElement_ = nullptr; };

class EventRoute
{
public:
    void Build(const RoutedEvent* _evt, UIElement* _source);
    // Tunnel  : 루트 → … → 소스
    // Bubble  : 소스 → … → 루트
    // Direct  : 소스
    const jc::Array<RouteItem>& Items() const;
};
```

- `UIElement::GetParent() → pParent_`, 루트의 부모는 `nullptr`이므로 여기서 종료
- 생성 후 `InvokeHandlers`에서 순차 순회

### 4.5 EventHandlersStore (요소별 핸들러 저장소)

```cpp
struct RegisteredHandler
{
    int id_;
    jc::Action<RoutedEventData&> fn_;
    bool handledEventsToo_;   // WPF의 handledEventsToo
};

class EventHandlersStore
{
    jc::HashMap<const RoutedEvent*, jc::Array<RegisteredHandler>> map_;
};
```

- `UIElement` 멤버로 소유 (`EventHandlersStore eventHandlers_;`)
- `AddHandler / RemoveHandler / Invoke` 제공

### 4.6 UIElement 확장 (핵심 변경)

```cpp
class UIElement : public cc::Node
{
public:
    // ── Routed Event API ──
    void RaiseEvent(RoutedEventData& _e);   // 경로 생성 + InvokeHandlers
    void AddHandler(const RoutedEvent* _evt, int _id, const jc::Action<RoutedEventData&>& _fn, bool _handledEventsToo = false);
    void RemoveHandler(const RoutedEvent* _evt, int _id);

    // ── On* 가상 핸들러 (라우트 도중 각 요소에서 자동 호출) ──
    virtual void OnPreviewMouseDown(RoutedEventData& _e);
    virtual void OnMouseDown(RoutedEventData& _e);
    virtual void OnPreviewMouseUp(RoutedEventData& _e);
    virtual void OnMouseUp(RoutedEventData& _e);
    virtual void OnMouseMove(RoutedEventData& _e);
    virtual void OnMouseEnter(RoutedEventData& _e);
    virtual void OnMouseLeave(RoutedEventData& _e);
    virtual void OnMouseScroll(RoutedEventData& _e);
    virtual void OnClick(RoutedEventData& _e);
    virtual void OnCheckedChanged(RoutedEventData& _e);
    virtual void OnToggleChanged(RoutedEventData& _e);
    virtual void OnTextChanged(RoutedEventData& _e);

    // 라우트 호출 시: (1) 등록 핸들러들 (2) 가상 On* 순서로 호출
    void InvokeHandlers(RoutedEventData& _e);

    UIGroup* GetParent() const { return pParent_; }
protected:
    EventHandlersStore eventHandlers_;
};
```

**`RaiseEvent` 내부 동작 (WPF 흐름 그대로):**

```
element->RaiseEvent(e)
  → pRoutedEvent 확인
  → EventRoute::Build(strategy에 따라 경로 생성)
  → route 순서대로:
       foreach item:
         item->InvokeHandlers(e)     // 등록 핸들러 호출
         item->OnXxx(e)              // 가상 핸들러 호출
         if e.handled_ && !handledEventsToo 등록자: 중단
```

---

## 5. 이벤트 전파 흐름 (변경 후)

### 5.1 입력 진입점 (UIGroup)

기존 `OnMouse*Internal` **순회를 히트 테스트로 유지**한다. 최심부 히트 요소를 찾으면 그 요소에서 라우트 이벤트를 발생시킨다.

```
UIGroup::OnMouseDownInternal()
  └── 자식 역순 순회 (히트 테스트)  ← 기존과 동일, 터널링 1단계
        └── 최심부 히트 요소 발견
              ├── RaiseEvent(PreviewMouseDown)   [터널: 루트 → … → 요소]
              └── RaiseEvent(MouseDown)          [버블: 요소 → … → 루트]
```

### 5.2 버블링 예시 (Click)

```
UIRootGroup
  └── UIGroup (button_panel)
        └── UIButton (btn_ok)

btn_ok->RaiseEvent(Click)
  ── 버블 경로: [btn_ok] → [button_panel] → [ui_generic(root)]

① UIButton::OnClick(e)         ← 버튼 자신이 처리 (e.SetHandled(true) 가능)
② UIGroup::OnClick(e)          ← 중간 그룹이 수신! (새 기능)
③ UIRootGroup::OnClick(e)      ← 루트가 수신
   (→ 기존 OnMouseUpTarget 호출로 폴백 가능)
```

### 5.3 터널링 + 버블링 쌍 예시 (MouseDown)

```
① PreviewMouseDown [터널]  루트 → … → btn_ok   (상위가 먼저 가로챌 수 있음)
② MouseDown        [버블]  btn_ok → … → 루트    (자식이 먼저 처리)
```

---

## 6. `On*` 핸들러 규칙

1. 모든 Routed Event는 `On<이벤트명>(RoutedEventData&)` 가상 함수와 1:1 매핑된다.
2. 기본 구현은 빈 함수 (`{}`). 처리하지 않으면 `handled_`는 `false`로 유지되어 계속 전파된다.
3. 이벤트를 "처리했음"을 나타내려면 핸들러 내에서 `_e.SetHandled(true)`를 호출한다.
4. `handled_ == true`가 되면 이후의 버블/터널 전파는 중단된다. 단, `AddHandler(..., handledEventsToo=true)`로 등록한 핸들러는 계속 호출된다.
5. 중간 `UIGroup`은 기본적으로 이벤트를 "보기만" 하고 전파를 막지 않는다. 특정 자식 영역 이벤트를 가로채려면 `OnClick` 등을 오버라이드하고 `SetHandled(true)` 하면 된다.

---

## 7. Grid / StackPanel 레이아웃 패널

### 7.1 클래스 계층

```
UIElement
  └── UIGroup            (절대좌표 배치, 기존 그룹)
        └── UIPanel      (추상: 레이아웃 패널 공통)
              ├── UIGrid
              └── UIStackPanel
```

### 7.2 UIPanel (공통 기반)

```cpp
class UIPanel : public UIGroup
{
public:
    void InitChildrenPosition() override;  // Arrange() 호출로 재정의
    virtual void Arrange() = 0;            // 자식 위치/크기 계산
protected:
    cc::size MeasureChild(UIElement* _child); // 콘텐츠 크기 측정
};
```

- `SetUISize()` 시 자동으로 `Arrange()` 재호출
- `InitFromXml()`에서 자식 생성 후 `Arrange()`

### 7.3 UIGrid

**크기 정의:**
```cpp
enum class GridUnitType { Pixel, Auto, Star };  // px / auto / *

struct RowDefinition { GridUnitType type_; float value_; };  // 40, auto, * , 2*
struct ColumnDefinition { GridUnitType type_; float value_; };
```

**자식 배치 속성:** `GridRow`, `GridColumn`, `GridRowSpan`, `GridColumnSpan` (XML: `grid_row` 등)

**배치 알고리즘:**
1. **측정 단계(Measure):** `Auto` 행/열은 자식 콘텐츠 크기의 최대값으로 결정.
2. **분배 단계(Arrange):** 남은 공간을 `*`(Star) 행/열에 가중치 비율로 분배.
3. Pixel 크기는 고정.
4. 각 자식을 셀 영역(span 포함)에 배치.

### 7.4 UIStackPanel

```cpp
enum class StackOrientation { Horizontal, Vertical };
```

- 자식을 생성 순서대로 한 축에 쌓는다.
- 가로: `x = 누적` / `y = 패널 높이 (stretch)`
- 세로: `y = 누적` / `x = 패널 너비 (stretch)`
- `spacing`(여백) 속성 지원.

### 7.5 XML 스키마

```xml
<Grid name="root_grid" x="0" y="0" width="640" height="480">
  <RowDefinitions>
    <RowDefinition height="40" />
    <RowDefinition height="*" />
  </RowDefinitions>
  <ColumnDefinitions>
    <ColumnDefinition width="2*" />
    <ColumnDefinition width="*" />
  </ColumnDefinitions>

  <StackPanel name="left_panel" orientation="vertical" grid_row="1" grid_column="0" spacing="4">
    <Button name="b1" />
    <Button name="b2" />
    <Button name="b3" />
  </StackPanel>

  <Group name="right_area" grid_row="1" grid_column="1">
    <Button name="close" />
  </Group>
</Grid>
```

- `UIGrid`/`UIStackPanel` 요소는 `UIXmlLoader`에 태그 등록 (`<Grid>`, `<StackPanel>`)
- 기존 `<Group>`은 절대좌표 컨테이너로 그대로 유지 (호환)

---

## 8. 기존 코드 호환 / 마이그레이션

| 기존 | 신규 | 마이그레이션 |
|------|------|-------------|
| `pRootGroup_->OnMouseDownTarget(this, evt)` | `RaiseEvent(MouseDown)` | `UIRootGroup::OnMouseDown(RoutedEventData&)` 내부에서 `OnMouseDownTarget(pSource_, evt)` 호출 → 기존 오버라이드 코드 무변경 동작 |
| `InvokeMouseEvent(eMouseEventDown, evt)` | `AddHandler(MouseDown, id, fn)` | `AddMouseEvent`가 내부적으로 `AddHandler` 호출하도록 래핑 |
| `OnMouseDownInternalDetail(evt)` | `OnMouseDown(RoutedEventData&)` | 구현체별 오버라이드로 이관 |
| `OnMouseUpTarget(UIElement*, EventMouse*)` | `OnMouseUp(RoutedEventData&)` | 루트에서 신규 메서드가 기존 시그니처를 호출 (하위호환) |
| `bPropagate`(bool) | `RoutedEventData::handled_` | `SetHandled()`로 치환 |

**주의 — 이중 호출 방지:**
- `RaiseEvent`가 `On*`를 호출하는 동안 기존 `OnMouse*Internal`이 또 `RaiseEvent`를 호출하지 않도록, 입력 진입점(`OnMouse*Internal`)은 **순수 히트 테스트 + 라우트 발생**만 담당한다.
- 구체 구현 로직(상태머신 eOver/ePressed 등)은 `On*` 가상 핸들러로 이동.

---

## 9. 구현 단계 (Todo 계획)

### Phase 1 — Routed Event 코어
- [ ] `sgcl/Game/UI/Event/RoutedEvent.h` — enum + 구조체 + 전역 이벤트 정의
- [ ] `sgcl/Game/UI/Event/RoutedEventData.h`
- [ ] `sgcl/Game/UI/Event/EventHandlersStore.h/.cpp`
- [ ] `sgcl/Game/UI/Event/EventRoute.h/.cpp`
- [ ] `UIElement`에 `RaiseEvent/AddHandler/RemoveHandler/InvokeHandlers/On*` 추가
- [ ] `UIRootGroup`에 `On*` 오버라이드 (기존 `On*Target` 호출로 폴백)

### Phase 2 — 입력 진입점 연결
- [ ] `UIGroup::OnMouse*Internal` → 히트 요소에서 `RaiseEvent` 호출로 변경
- [ ] `UIButton` 상태머신을 `On*`으로 이관
- [ ] `InvokeMouseEvent`/`AddMouseEvent` → `AddHandler` 래핑
- [ ] `Click`(Bubble) 이벤트 발생 (MouseUp contained 시)
- [ ] `UICheckBox`/`UIToggleButton` → `CheckedChanged`/`ToggleChanged` 발생

### Phase 3 — 레이아웃 패널
- [ ] `UIPanel` (추상) — Arrange() 정의
- [ ] `UIGrid` + Row/Column 정의 + 배치 알고리즘
- [ ] `UIStackPanel` + Orientation
- [ ] `UIXmlLoader`에 `<Grid>`, `<StackPanel>`, `<RowDefinition>`, `<ColumnDefinition>` 파싱 추가
- [ ] `ui_generic.xml` 일부를 Grid/StackPanel로 재작성하여 검증

### Phase 4 — 검증
- [ ] `UI_Generic`에 중간 그룹 `OnClick` 핸들러 추가 테스트
- [ ] 기존 UI 전체(로그인 등) 회귀 테스트
- [ ] 빌드(0 에러) + 실행 확인

---

## 10. 위험 요소 / 고려사항

1. **이중 호출** — 기존 `OnMouse*Internal` + 신규 `RaiseEvent`가 같은 이벤트를 두 번 처리하지 않도록 경계 명확화. (Phase 2에서 입력 진입점과 핸들러 분리)
2. **퍼포먼스** — 매 입력마다 Route를 새로 생성하면 비용 발생. 캐싱(요소 트리 변경 시 invalidate) 고려.
3. **좌표계** — 기존 `EventMouse`의 `cursorPosition`이 상대좌표로 재설정되는 흐름(`SG_CURSOR_POSITION_GUARD`)을 유지해야 `IsContainPoint`가 정상 동작.
4. **드래그** — 기존 드래그 로직(`Draginit`/`DragMove`)이 `OnMouseDown`에서 수행되므로, `On*` 이관 시 동일 시점에 수행되도록 보장.
5. **코드량** — 5개 파일 이상 수정이 필요하므로 **수행 전 사용자 확인 필요** (CLAUDE.md 규칙).

---

## 11. 참고 파일

- `Projects/sgcl/Classes/sgcl/Game/UI/UIElement.h/.cpp` — 핵심 확장 대상
- `Projects/sgcl/Classes/sgcl/Game/UI/UIGroup.h/.cpp` — 입력 진입점 변경 대상
- `Projects/sgcl/Classes/sgcl/Game/UI/UIRootGroup.h/.cpp` — On* 폴백 대상
- `Projects/sgcl/Classes/sgcl/Game/UI/UIButton.cpp` — 상태머신/Click 이관 대상
- `Projects/sgcl/Classes/sgcl/Game/UI/UICheckBox.cpp`, `UIToggleButton.cpp` — 라우트 이벤트 대상
- `Projects/sgcl/Classes/sgcl/Game/UI/UIXmlLoader.cpp` — Grid/StackPanel 파싱
- `Projects/sgcl/Classes/sgcl/Game/UI_Implementation/UI_Generic.cpp` — 검증 대상
