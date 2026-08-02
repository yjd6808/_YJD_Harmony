# Plan: 소형 컨트롤 9-slice 미적용 (no-slice-small-controls) (2026-08-02)

> ⚠️ **CANCELLED (2026-08-02)** — 구현 후 앱에서 UI 위치가 어긋나는 문제 발생, 사용자 요청으로 전체 롤백됨.
> 7개 파일 모두 이전 상태로 복원, 빌드 EXIT=0 후 재배포 완료 (release\sgcl\sgcl.exe 6:39:27).
> 참고: 이후 검증은 클릭 방식 대신 로그 남기는 방식 선호.

## 목표
- 테두리 금속 그라데이션(ResolveMetal/borderGradient)이 켜진 상태에서
  소형 컨트롤이 9-slice(Scale9)로 그려져 모서리/그라데이션이 어색해지는 문제 수정.
- **수정 방향**: 컨트롤이 **자기 9slice 기준 크기(=컨트롤별 베이크 레시피 크기)** 안에
  완전히 들어가면 9-slice를 적용하지 않고 **베이크된 전체 텍스처를 컨트롤 크기로 스트레치**한다.
  - 9-slice의 목적은 "확대 시 흐려짐 방지". 컨트롤이 기준 크기보다 작거나 같으면
    텍스처를 **축소**하는 셈이므로 해상도 문제가 없음.
  - 전체 스트레치 시 대각선 금속 그라데이션·표면 그라데이션이 통째로 축소되어 부드럽게 보임.
    (평면 필러(LayerColor) 대체 — 기존 flatFallback 제거)
  - 한 축이라도 기준 크기를 넘으면 9-slice 유지 (확대 흐림 방지).

## 결정 사항 (사용자 확정)
1. 조건: **항상 적용** (그라데이션 테두리 여부와 무관).
2. 기준 크기: **컨트롤별 베이크 레시피 크기** (버튼 160x44, 프레임 80x80,
   체크박스 32x32, 토글트랙 54x28, 스크롤바 트랙 16x120 등 — `UIControlBakeRecipes`에 이미 존재).
   - 9-slice off: `size_.width <= refW && size_.height <= refH`
   - 9-slice 유지: 한 축이라도 ref 초과 (예: 32x140, 300x200 → 9slice)
3. 설정 위치: 테마 토큰은 **쓰지 않음** (컨트롤별 레시피가 곧 기준).
4. 평면 필러(flat fill) 대신 **전체 텍스처 스트레치**로 부드럽게 렌더링.

## 구현
### 1. 레시피 단일화
- `UIControlBakeRecipes::RecipeFor(UIAssetSemantic)` 추가 — semantic → 베이크 크기/인셋 매핑.
- `UITextureBakeService::SelectRecipe`(중복 스위치) 제거 후 `RecipeFor`로 대체 (단일 출처).

### 2. 바인딩 Stretch 모드
- `UIBindingKind::Stretch` 추가 (`UIThemeBinding.h`).
- `BindStretch(cc::Sprite*, key, slot)` 추가.
- `Refresh()`에서 Stretch → `sprite->initWithTexture(tex, entry->atlasRect)`
  (Scale9 인셋 없이 전체 텍스처 사용) (`UIThemeBinding.cpp`).

### 3. BrushVisual 노드 종류 전환
- flatFallback(LayerColor)/sliceMinWidth_/flatColor_ 제거 → `stretchMode_`로 대체.
- `ComputeStretchMode()`: 비고정 테마 브러시이고
  `size_.width <= RecipeFor(semantic).width && size_.height <= RecipeFor(semantic).height`이면 true.
- `Rebuild()`: stretchMode_면 anchor=ZERO인 일반 `Sprite`, 아니면 기존 `Scale9Sprite`.
- `ApplyThemeBinding()`: stretchMode_면 BindStretch, 아니면 BindScale9.
- `SyncNodeKind()`: 크기가 기준을 넘으면 stretch ↔ scale9 전환(재빌드).
- `ApplySize()`는 양쪽 모두 `setContentSize(size_)` (Sprite는 텍스처를 자동 스트레치).

## 변경 파일 (7개 — AGENTS.md 5개 초과, 확인 필요)
| 파일 | 변경 |
|---|---|
| `Theme/Baker/UIControlBakeRecipes.h` | RecipeFor 선언 |
| `Theme/Baker/UIControlBakeRecipes.cpp` | RecipeFor 구현 |
| `Theme/UITextureBakeService.cpp` | SelectRecipe → RecipeFor |
| `Theme/UIThemeBinding.h` | UIBindingKind::Stretch + BindStretch |
| `Theme/UIThemeBinding.cpp` | BindStretch/Refresh Stretch 처리 |
| `UI/Core/BrushVisual.h` | flatFallback 관련 멤버 → stretchMode_ |
| `UI/Core/BrushVisual.cpp` | 스트레치 노드/바인딩/전환 로직 |

> 테마 JSON 79개는 수정하지 않음. 테마 토큰 미추가(사용자 선택: 컨트롤별 레시피가 기준).

## 검증
1. 빌드 (cwd=repo, `Scripts\BuildProject\sgcl.bat -C Debug -P x64`) EXIT=0.
2. 앱 실행 → Modern Dark Gold(그라데이션)로 전환 → `ui_theme_test`의
   btn_small20(20x20)/btn_normal(110x34) 확인: 9-slice 4분할 어색함 없이 부드러운 그라데이션.
   세로형 컨트롤(스크롤바 트랙 등)은 9slice 유지 확인.
3. 스크린샷 픽셀 분석(선택) + 육안 확인.
4. 완료 시 `/done no-slice-small-controls` → `.admin/histories/`, memory 갱신.
