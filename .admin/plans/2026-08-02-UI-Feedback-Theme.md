# 2026-08-02 UI 피드백: pressed 대비 / 기본 border / 테마 연동 / 테마 테스트 UI

## 배경
- pressed(28,31,36)가 윈도우 배경(26,28,33)과 거의 동일 → 클릭 피드백이 안 보임
- 버튼/타이틀 윈도우에 border가 기본 적용되어야 함 (border는 theme에 영향받도록)
- 테마/스킴 전환을 테스트할 수 있는 UI 1개 추가 필요

## 변경 목록

### 1. pressed 대비 강화 — `UIRuntimeTheme.cpp`
- EngineDefaults: pressedTop/Bottom (28,31,36) → (16,18,23) (윈도우 배경보다 확실히 어둡게)

### 2. 기본 border — `UIControlBakeRecipes.cpp`
- ButtonRecipe: sliceInsets 14→8, minimumSize 28→16
- 20x20 소형 버튼도 Scale9+border(테마 금색 1px)가 적용됨 (기존 flatFallback은 16px 미만만)

### 3. 윈도우 크롬 theme 연동 — `UIRuntimeTheme.h`, `UIRuntimeTheme.cpp`, `Window.cpp`, `UIThemeMapper.cpp`
- `UIWindowTokens` 추가: windowBackground / titleBarBackground / titleBarForeground / borderColor / borderWidth
- EngineDefaults: bg(26,28,33) titleBar(31,33,38) fg(235,237,242) borderColor(88,91,99) borderWidth(1)
- Window.cpp: 하드코딩 `DefaultTone`/`ResolveDefaultTone` 제거 → 활성 테마 `window` 토큰 사용
  (customWindowBackground_ 등 명시 설정 우선은 유지)
- UIThemeMapper: JSON `window.*` 그룹 읽기 + ApplyLightScheme에서 window 토큰도 light 변환

### 4. 테마 테스트 UI — 신규
- `ui_theme_test.xml` (WindowStyle=Title → 타이틀바/창 border 확인용)
  - 컨트롤 갤러리: 소형 버튼(20x20), 일반 버튼, 체크박스, 토글, 슬라이더, 프로그레스바, 텍스트박스, 스크롤바
  - 테마 전환 버튼: engine-default / modern-dark-gold / amoled / dracula / aurora-light-silver / github
  - 스킴 버튼: Dark / Light / 현재 상태 라벨
  - 버튼: ui_title_demo 열기
- `UI_ThemeTest.h/.cpp` (REGISTER_WINDOW "ui_theme_test")
- `theme/engine-default.json` (신규, flat 기본값 + window 그룹)
- `theme/modern-dark-gold.json` (window 그룹 추가)
- `Scene_Login.cpp`: `g_cUIMgr.Show("ui_title_demo")` → `Show("ui_theme_test")`

## 파일 수: 11 (수정 7 + 신규 4) → 5개 초과로 사용자 확인 필요

## 검증
1. 빌드 EXIT=0
2. 실행: pressed 시 (16,18,23)으로 창 배경보다 확실히 어두워짐
3. 20x20 버튼에도 테두리 표시
4. 테마/스킴 버튼으로 전환 시 버튼/윈도우 크롬 색 변화 확인
5. 로그: entries=80 유지, Entry not found 없음
