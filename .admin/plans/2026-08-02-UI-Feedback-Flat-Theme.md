# 플랜: 2026-08-02 UI 피드백 개선 + Flat 테마 (UI-Feedback-Flat-Theme)

## 배경 (사용자 발견 3가지 이슈)

1. 버튼에 마우스를 올려도 hover/clicked 상태 변화를 느낄 수 없음
2. 테마를 완전 flat으로; 타이틀바 색과 버튼 색이 너무 비슷해 어색함
3. 아이콘(버튼 글리프)이 버튼 중앙에서 우측으로 이탈

## 근본 원인 분석 (완료)

### 이슈 1 — hover/clicked 피드백 없음
- **원인 A**: `UIThemeManager::BuildThemeVariants()`가 **Normal 상태만** 베이크
  (UIThemeManager.cpp:189 `variant.state = UIVisualState::Normal`).
  Scale9 버튼(110x34 등 일반 버튼)의 Hover/Pressed 텍스처 키가 텍스처셋에 없음
  → `UIThemeBinding::Refresh`에서 `ApplyMissingTexture` (UIThemeBinding.cpp:35) → 시각 변화 없음.
  이벤트 전달 경로(InputDispatcher::UpdateHoverChain → ueMouseEnter/Leave → UpdateVisualState)
  와 상태 매핑(Control::MapVisualState: Pressed→Hover→Focused)은 정상 동작.
- **원인 B**: 평면 필러(flat fallback) 색은 상태별로 전환되지만 hover 델타가
  +13 RGB (45,48,53 → 58,61,68)로 너무 작아 인지 불가.
- **해결**: ① BuildThemeVariants를 상태 루프(Normal/Hover/Pressed/Disabled)로 확장
  — cacheKey.stateHash(베이커 line 157)와 styleHash가 상태별로 분리되므로 키 충돌 없음.
  ② EngineDefaults의 상태별 색 델타 확대.

### 이슈 2 — flat 테마 + 타이틀바/버튼 색 대비
- 현재 EngineDefaults(UIRuntimeTheme.cpp): gloss.normalAlpha=0.188, innerRim 0.75, depth 0.4,
  shadow 1.0 — 메탈릭 느낌. surfaceTop/Bottom이 그라데이션.
- 타이틀바 톤(Window.cpp:49-54): titleBarBackground (0.16,0.17,0.20)=(41,43,51) vs
  버튼 surface normalTop (45,48,53) — 거의 동일색 → 어색함 확인.
- **해결**: EngineDefaults flat화 + 타이틀바를 버튼과 명확히 구분되는 어두운 색으로.

### 이슈 3 — 아이콘/글리프 우측 이탈
- btn_small20: 버튼 Padding(12,6) 기본값이 20x20 버튼의 콘텐츠 슬롯을 (0,8)로 붕괴
  (12+12=24 > 20). 라벨 "S"(8x22)가 x∈[12,20]에 배치 → 중앙(10) 대비 6px 우측 이탈.
  덤프 확인: `btn_small20 ... <unnamed> z=0 pos=(12.0,6.0) cs=(0.0,8.0)`.
- 캡션 버튼(28x28): 슬롯 (4,16), 라벨(8,22)이 컨테이너를 넘쳐 2px 우측 + 3px 하단 이탈.
  참고: `window-minimize/maximize/close.svg`가 `assets/res_data/gfx/mdi`에 **없어**
  아이콘 스프라이트가 생성되지 않음(폴백 텍스트만 표시) — 아이콘 추가 시 대비해
  BrushVisual 호스트 중앙 배치도 함께 수정.
- **해결**: ① btn_small20 XML `Padding="0,0"`. ② 캡션 버튼 `SetPadding(0,0)`.
  ③ `BrushVisual::ApplySize` fixedSize → 호스트 콘텐츠 크기 중앙 기준(호스트 0이면 자기 크기 폴백)
  + `Window::FlushLayout`에서 레이아웃 확정 후 `iconVisuals_` ApplySize() 재호출(호스트 크기 확정 시점).

## 변경 파일 (5개 — AGENTS.md 제약으로 사용자 확인 필요)

| 파일 | 변경 내용 |
|---|---|
| `Projects/sgcl/Classes/sgcl/Game/UI/Theme/UIThemeManager.cpp` | BuildThemeVariants 상태 루프 4종 |
| `Projects/sgcl/Classes/sgcl/Game/UI/Theme/UIRuntimeTheme.cpp` | flat 값(gloss 0, shadow 0 등) + hover/pressed 델타 확대 |
| `Projects/sgcl/Classes/sgcl/Game/UI/Controls/Window.cpp` | 타이틀바 톤, 캡션 버튼 Padding 0, FlushLayout 아이콘 재중앙 |
| `Projects/sgcl/Classes/sgcl/Game/UI/Core/BrushVisual.cpp` | fixedSize 호스트 중앙 배치 |
| `assets/res_data/layout/ui_title_demo.xml` | btn_small20 Padding="0,0" |

## 변경 상세

### 1. UIRuntimeTheme::EngineDefaults (flat + 피드백)
```
surface: normalTop=normalBottom=(45,48,53), hover=(66,70,78) (+21), pressed=(28,31,36) (-17),
         disabled=(58,60,64), inset=(30,33,38)
gloss.normalAlpha = 0.0f  (flat)
geometry: radius 4 유지, borderWidth 1.0, innerRim 0, depth 0, shadow 0
state.hoverLift = 0
```

### 2. UIThemeManager::BuildThemeVariants
- `UIVisualState states[] = { Normal, Hover, Pressed, Disabled }`
- 각 semantic × state: `resolver_.Resolve(elemType, state, theme, {})` → variant.state = state
- recipeHash는 semantic 유지, styleHash는 상태별 상이 → 캐시 키 분리 (베이커는
  resolvedStyleHash+recipeHash+stateHash로 캐시 키 구성 확인 완료)

### 3. Window.cpp
- ResolveDefaultTone dark: titleBarBackground (0.16,0.17,0.20) → (0.12,0.13,0.15)=(31,33,38)
- BuildCaptionButton: `pBtn->SetPadding(Thickness(0.0f, 0.0f));` (SetMargin 다음)
- FlushLayout: Arrange 후 `for iconVisuals_: iconVisuals_[idx]->ApplySize();` (호스트 크기 확정 후 재중앙)

### 4. BrushVisual::ApplySize (fixedSize)
```cpp
const float hostW = pHost_ ? pHost_->getContentSize().width : 0.0f;
const float hostH = pHost_ ? pHost_->getContentSize().height : 0.0f;
pNode_->setPosition((hostW > 0.0f ? hostW : size_.width) * 0.5f,
                    (hostH > 0.0f ? hostH : size_.height) * 0.5f);
```

### 5. ui_title_demo.xml
- btn_small20에 `Padding="0,0"` 추가 → 슬롯 (20,20) → "S" 중앙 (10,10)

## 검증
1. 빌드 (cwd=repo, Build.ps1 sgcl Debug x64) EXIT=0
2. 앱 재실행 → ui_title_demo 덤프:
   - btn_small20 콘텐츠 pos=(10,0) 근처 + Label 중앙
   - 캡션 버튼 콘텐츠 pos=(10,3) (슬롯 (28,28), 라벨 (8,22))
   - 로그의 BakeDefaultTextureSet variants=80 (20 semantic × 4 상태)
3. 화면 확인: hover 시 버튼 색 변화, 타이틀바와 버튼 색 구분, "S" 중앙
