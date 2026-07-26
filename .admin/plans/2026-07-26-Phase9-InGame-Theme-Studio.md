# Phase 9: In-game Theme Studio

> 생성일: 2026-07-26
> 브랜치: sg-dev (워크트리: UltimateHarmony-dev)

## 목표
게임 내에서 Theme 토큰을 실시간 편집/미리보기/적용할 수 있는 설정 화면 구현

## 작업 목록

### 1. UIThemePropertyDescriptor 시스템
- UIThemePropertyDescriptor 구조체 (token, label, editor type, min/max/step)
- UIPropertyEditorType enum (Slider, ColorPicker, Dropdown, Toggle)
- Property Descriptor Table (Runtime Theme 모든 토큰 등록)

### 2. UIThemeInspectorRow
- Slider Editor (float 토큰: radius, borderWidth, glossAlpha 등)
- ColorPicker Editor (color 토큰: surfaceTop, metalBorder 등)
- Dropdown Editor (enum 토큰: scheme 등)
- Reset 버튼 (토큰별 Override 초기화)
- Draft Binding (조작 시 Draft 갱신 + Preview 요청)

### 3. UIThemeStudio (메인 화면)
- Theme Dropdown (Engine Default 포함)
- Scheme Selector (Light/Dark/System)
- Surface Section
- Metal Border Section
- Soft Gloss Section
- Geometry & Depth Section
- Semantic Section
- State Section
- Preview Catalog (모든 컨트롤 타입 상태별 렌더링)
- Build Status / Cache Stats
- Reset Token / Reset Theme
- Cancel / Apply 버튼

### 4. Draft/Edit Session
- BeginEdit() → 현재 Theme 복사본 생성
- CommitDraft() → Draft를 Active Theme으로 적용
- CancelPreview() → Draft 폐기
- RequestDebouncedPreview() → Bake Service에 Preview 요청

### 5. UIThemeEditHistory (Undo/Redo)
- Draft Snapshot Stack (최대 80개)
- Pointer Down에서 Checkpoint
- Drag 중 같은 Entry 갱신

### 6. Preview Catalog
- Button Normal/Hover/Pressed/Disabled
- Checkbox Unchecked/Checked/Disabled
- Toggle Off/On/Disabled
- Progress Neutral/Success/Warning
- 실제 Production Control 인스턴스 사용

### 7. Build Status UI
- Idle / Baking / Uploading / Applied 상태 표시
- Cache Hit/Miss 통계
- 메모리 사용량

### 8. Token/Theme Reset
- 개별 Token Reset → ClearUserOverride()
- 전체 Theme Reset → EngineDefaults()로 복원

## 완료 기준
- 설정 조작 중 기존 UI 사용 가능
- Apply 후 재시작해도 동일 Theme 유지
- Cancel 즉시 기존 TextureSet 복귀
- Slider 연속 입력 중 Crash/Hitch 없음 (Debounce + Cancel)

## 선행 조건
- Phase 1-7 코드가 sg-dev 브랜치에 구현 완료 (✅)
- sgcl Compile 0 error (✅)
- Full Build 필요 시 진행
