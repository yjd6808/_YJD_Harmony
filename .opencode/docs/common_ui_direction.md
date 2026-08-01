# 공용 UI 시스템 개선 방향

## 1. 현재 문제점

### 1.1 스프라이트 지정이 모든 UI에 개별적으로 하드코딩됨
모든 Button/CheckBox/ToggleButton 등이 XML에서 `sga`, `img`, `sprite="0,1,2,3"` 같은 식으로 각 상태별 스프라이트 인덱스를 직접 지정하고 있음. 화면마다 모든 요소에 일일이 sprite index를 지정해야 하므로 UI 추가/변경 시 작업량이 큼.

### 1.2 9-slice 지원이 미미함
- `UISprite`만 `scale9` 속성 지원, `UIButton`/`UICheckBox`/`UIToggleButton` 등은 미지원
- 리사이징 시 단순 `setScale()`로 처리되어 테두리가 늘어나는 현상 발생
- `UIButton::SetUISize()`에서 sprite 전체를 스케일링하므로 9-slice 효과를 볼 수 없음

### 1.3 상태 관리 중복
- Button/CheckBox/ToggleButton/ScrollBar가 각자 `SetVisibleState()` 구현
- 상태 → 스프라이트 인덱스 매핑이 클래스마다 다름
- ToggleButton은 2가지 토글 상태 × 4가지 마우스 상태 = 8개 스프라이트를 직접 관리

### 1.4 테마/스타일 개념 부재
- 각 XML에서 직접 sga/img/sprite 인덱스를 참조하므로, 외형 변경 시 모든 XML 파일을 수정해야 함
- "버튼 스타일 A", "버튼 스타일 B" 같은 개념 없음

### 1.5 Load/Unload 패턴 중복
- 모든 UIElement 파생 클래스가 동일한 텍스처 생성/해제 패턴을 복사-붙여넣기
- `FrameTexture* texture_[eMax]` + `Sprite* sprite_[eMax]` 구조가 여러 클래스에 중복

---

## 2. AIProject 9scale_ui 리소스 분석

`common_ui_design/9scale_ui/` 는 다음과 같은 데이터 구조를 가짐:

### manifest.json (141개 에셋)
- 각 스프라이트의 9-slice inset 정보 (`sliceInsets: { left, right, top, bottom }`)
- 최소 크기 (`minimumSize: { width, height }`)
- 모든 PNG에 JSON sidecar 존재

### controls.json (35개 컨트롤 레시피)
- Panel, Button, Window, Checkbox, Toggle, ScrollBar, Slider 등 35개 컨트롤 정의
- 각 컨트롤은 `parts`로 구성 (background, foreground 등)
- 각 part는 여러 `states` 보유 (normal, hover, pressed, disabled, active)
- `stretch` 필드로 9-slice / fill / center 등의 스케일링 방식 지정
- `bounds`로 상대 위치 지정 (퍼센티지 지원)

### 예: Button 컨트롤 구조 (controls.json)
```json
"Button": {
  "parts": {
    "background": {
      "states": {
        "normal":  { "sprite": "button_normal",  "file": "..." },
        "hover":   { "sprite": "button_hover",   "file": "..." },
        "pressed": { "sprite": "button_pressed",  "file": "..." },
        "disabled":{ "sprite": "button_disabled", "file": "..." }
      },
      "stretch": "slice"
    }
  }
}
```

---

## 3. 개선 방향

### [Phase 1] 공용 UI 렌더러 코어 — `CommonUI`

#### 3.1 CommonSpriteRenderer — 스프라이트 렌더링 추상화
- `UIElement`의 `texture_[eMax]` + `sprite_[eMax]` 배열을 중앙 클래스로 통합
- 9-slice 지원: 모든 요소가 `Scale9Sprite` 또는 `Sprite`를 조건부로 사용
- `sliceInsets`, `minimumSize`를 Info 구조체에 추가
- `Load()/Unload()` 공통화: Info → Texture → Sprite 생성 로직을 상위 클래스 or 헬퍼로 이동

```cpp
class CommonSpriteRenderer {
    FrameTexture* textures_[MAX_SLOTS];
    Sprite* sprites_[MAX_SLOTS];
    bool scale9_;
    S9Insets sliceInsets_;
    
    void Load(ImagePack*, int imgIndex, int* spriteIndices, int count);
    void SetVisibleState(int state);
    void SetUISize(const cc::size& size);  // 9-slice 대응
    void Unload();
};
```

#### 3.2 CommonStyle — 스타일/테마 시스템
- XML에서 `sprite="0,1,2,3"` 대신 `style="primary_button"` 처럼 스타일 이름 참조
- 스타일 정의 (XML or JSON):
  ```xml
  <Style name="primary_button" sga="ui_common.sga" img="btn_common.img"
         sprites="0,1,2,3" scale9="true" sliceInsets="8,8,8,8"/>
  ```
- `CommonStyleManager`가 스타일 로드/캐싱 담당
- XML 파서에서 `style` 속성 감지 → `CommonStyleManager`에서 실제 sprite 인덱스 resolve

#### 3.3 CommonButton — 버튼 공통화
- 기존: 각 버튼이 4개 sprite 직접 관리
- 개선: `CommonStyle` 참조 + 자동 hover/pressed/disabled 상태 전이
- `ParseButton()`에서 `style` 속성이 있으면 스타일에서 sprite 정보 획득
- backward compatibility: `sga`/`img`/`sprite` 속성이 있으면 기존 방식대로 동작

#### 3.4 Scale9 적용 확산
- `UIButton`, `UICheckBox`, `UIToggleButton`, `UIScrollBar`에 `scale9_` 속성 추가
- XML: `<Button ... scale9="true" left="8" right="8" top="8" bottom="8"/>`
- `SetUISize()`에서 9-slice일 경우 `Scale9Sprite::setContentSize()` 호출
- 일반 sprite일 경우 `setScale()` (기존 방식)

---

### [Phase 2] 9scale_ui 리소스 통합

#### 3.5 9scale_ui 포맷 변환 도구
- `manifest.json` → Harmony SGA에 포함할 9-slice 메타데이터로 변환
- `controls.json` → `CommonStyle` 정의로 변환
- 141개 PNG를 SGA 이미지 팩으로 묶음
- sliceInsets를 Harmony 포맷으로 매핑

#### 3.6 9scale_ui 컨트롤 → Harmony 컨트롤 매핑
| 9scale_ui 컨트롤 | Harmony 대상 | 비고 |
|---|---|---|
| Button | UIButton | style="common_button" |
| Checkbox | UICheckBox | style="common_checkbox" |
| Toggle | UIToggleButton | style="common_toggle" |
| ScrollBar | UIScrollBar | style="common_scrollbar" |
| ProgressBar | UIProgressBar | style="common_progressbar" |
| TextInput | UIEditBox | 배경만 9-slice |
| Panel/UIGroup | UIGroup | 배경 스프라이트로 사용 |

---

### [Phase 3] 고도화

#### 3.7 자동 상태 머신
- 버튼의 hover/pressed/disabled 전이를 완전 자동화
- `isEnabled()` → 자동으로 disabled 상태 표시
- 터치/클릭 다운 → 자동 pressed 상태
- `SetToggleState()`가 내부적으로 스프라이트 가시성까지 관리

#### 3.8 레이아웃 시스템 (장기)
- 9scale_ui의 `bounds` 필드처럼 기본적인 상대 레이아웃 지원
- `left="50%"`, `right="10"` 같은 속성
- 그룹 내 자식들의 fill/stretch 동작

---

## 4. 제안하는 구현 순서

1. **CommonSpriteRenderer 클래스 구현** — 모든 UIElement의 스프라이트 관리 통합
2. **UIButton에 Scale9 지원 추가** (`scale9_`, `sliceInsets_`)
3. **CommonStyle/CommonStyleManager 구현** — XML에서 style 참조 가능하도록
4. **UICheckBox, UIToggleButton에 Scale9 전파**
5. **AIProject 9scale_ui → Harmony 스타일 변환 스크립트 작성**
6. **9scale_ui 리소스를 SGA로 패킹하여 실제 게임에 적용**
7. **기존 XML 마이그레이션** — 직접 sprite index → style 참조로 전환

---

## 5. Backward Compatibility 전략

- 기존 `sga`/`img`/`sprite` 속성은 계속 지원 (제거하지 않음)
- 새로운 `style` 속성이 있으면 우선 적용, 없으면 기존 방식
- `scale9` 속성 기본값 `false` (기존 동작 유지)
- CommonUI 관련 코드는 `#ifdef USE_COMMON_UI`로 감싸거나 별도 소스로 분리
