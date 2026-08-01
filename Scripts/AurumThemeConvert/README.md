# AurumThemeConvert

opencode(엘렉트론 데스크톱) 테마 JSON → Aurum(sgcl 런타임) 테마 JSON 변환기

## 역할

sgcl 게임 클라이언트의 UI 테마 시스템(`UIThemeManager` / `UIRuntimeTheme`)이 사용하는
`{테마id}.{dark|light}.json` 파일들을 **opencode 공식 테마(desktop-theme)** 로부터 일괄 생성한다.

- **입력**: opencode desktop-theme JSON 38종 (`themes/*.json`)
- **출력**: Aurum 스키마 JSON 76개 (`{id}.dark.json` + `{id}.light.json`)
- **원리**: ElectronTheme-WithTest(`src/renderer/theme/{color,resolve}.ts`)에 구현된
  opencode 공식 테마 리졸버를 Python으로 포팅한 뒤, 리졸브된 토큰을 Aurum 스키마에 매핑한다.

## 파일

| 파일 | 역할 |
|---|---|
| `convert_themes.py` | 변환기 본체 (OKLCH 색상 수학 + 스케일 생성 + 스키마 매핑) |
| `README.md` | 본 문서 |

## 실행 방법

```powershell
python convert_themes.py --src <opencode 테마 폴더> --out <출력 폴더>
```

예시 (현재 사용 중):

```powershell
python convert_themes.py `
  --src "C:\Users\jdyun\Desktop\aurum-ui-theme (4)\themes" `
  --out "C:\Users\jdyun\AppData\Local\Temp\opencode\theme_out"
```

- `--src`: opencode desktop-theme JSON이 들어있는 폴더 (`.json` 전부 처리)
- `--out`: `{id}.{mode}.json` 출력 폴더 (자동 생성)

출력 후 배포 폴더로 복사:

```powershell
Copy-Item -Path "<out>\*" -Destination "D:\Programming\MyProjects\_DevCpp\UltimateHarmony_Dev\assets\res_data\theme\" -Force
```

> 주의: 배포 폴더에 있는 수동 관리 테마(`modern-dark-gold.json`, `aurora-light-silver.json`)는
> 변환 결과와 별개 파일이므로 유지된다. 배포 전 백업 권장.

## 입력 형식 (opencode desktop-theme)

```json
{
  "id": "dracula",
  "name": "Dracula",
  "dark":  { "palette": { "neutral": "#1e1e2e", "ink": "#cdd6f4", "primary": "#b4befe",
                           "success": "...", "warning": "...", "error": "...", "info": "...", "interactive": "..." },
             "overrides": { "background-base": "#...", "text-base": "#...", "text-weak": "#..." } },
  "light": { ... }
}
```

- 각 variant는 `palette`(compact) 또는 `seeds` 중 하나를 가짐 (`palette` 우선)
- `light` variant가 없는 테마는 `dark`를 폴백으로 사용
- `overrides`의 `background-base` / `text-base` / `text-weak` (hex) 지원

## 변환 파이프라인

### 1. OKLCH 색상 수학 (`color.ts` 포팅)

- `hex ↔ RGB(0~1) ↔ OKLCH` 변환 (표준 OKLab 행렬, cbrt, srgb↔linear)
- `_mix_colors(c1, c2, t)`: OKLCH 보간 (hue는 최단 경로 델타)
- `_blend(fg, bg, alpha)`: RGB 알파 블렌드 (fg를 bg 위에 alpha만큼)
- `_shift(color, l/c/h)`: OKLCH 좌표 이동

### 2. 스케일 생성 (`generate_scale` / `generate_neutral_scale`)

- **`generate_scale(seed, isDark)`**: 컬러 시드 → 12단계 톤 스케일
  - dark/light 각각 전용 곡선 (dark: 0.118→0.984 위로 상승, light: 0.993→0.27 아래로 하강)
  - chroma 승수 곡선도 스킴별 상이 → 다크는 채도 증가, 라이트는 채도 억제
- **`generate_neutral_scale(seed, isDark, ink)`**: 중성 스케일 (배경~텍스트 12단계)
  - compact(palette): 배경 앵커(seed 톤)와 ink 사이 OKLCH 보간
    - **dark**: 배경 = seed에서 sink(0.19~0.27) → 어두운 배경
    - **light**: 배경 = seed 톤 보존 (테마 고유 배경색 유지)
      - seed L < 0.5 (다크 전용 테마: catppuccin-frappe/macchiato 등): seed 그대로 유지
      - 그 외: 최소 L 0.82로 보정
  - seeds 전용: 리졸버 원본 곡선 사용 (neutralChroma = min(c, 0.068|0.04))
- **`generate_neutral_alpha_scale`**: 배경↔ink 알파 블렌드 12단계 (전경/테두리용)

### 3. 토큰 → Aurum 스키마 매핑 (`theme_from_electron`)

| Aurum 필드 | 매핑 규칙 |
|---|---|
| `meta` | name/id/scheme, `version: "2.0.0"` |
| `surface` (8색 그라데이션 쌍) | neutral 스케일 인덱스 매핑 |
| `gold` (테두리/강조) | primary 기반 (스튜디오 mix() 공식 유지: hover/highlight/bright는 흰색 mix, shadow는 검정 mix, muted는 bg mix) |
| `semantic.text` | compact: `body`(ink OKLCH shift, 대비 보정) / seeds: neutral[10] |
| `semantic.disabledText` | `text-weak` (compact: body shift) |
| `semantic.danger/success` | error/success 스케일 `[8]`(dark) / `[9]`(light) |
| `semantic.warning` | warning 시드에 hue/luma/chroma 보정(amber) 후 `[8]`/`[9]` |
| `semantic.focus` | interactive 스케일 `[8]` |
| `gloss.tint` | dark: ink+white 42% mix / light: `#FFFFFF` |
| `gloss.normalAlpha` | `round((38|24) / 255, 4)` → dark 0.149 / light 0.094 |

#### surface 상태 매핑 (dark)

```
normal  : top=scale[3], bottom=scale[1]
hover   : top=scale[4], bottom=scale[2]   (밝아짐)
pressed : top=scale[1], bottom=scale[3]   (그라데이션 역전 = 눌림)
disabled: top=scale[2], bottom=scale[1]
inset   : top=scale[1], bottom=scale[2]   (함몰면)
```

#### surface 상태 매핑 (light) — 모던 그라데이션

배경 단색 해소를 위해 상단은 테마 배경 톤(seed), 하단은 ink + **primary 틴트**로 은은한
컬러 그라데이션을 만든다.

```
normal  : top=scale[0],            bottom=blend(primary, scale[3], 0.06)
hover   : top=scale[0],            bottom=blend(primary, scale[1], 0.03)  (밝아짐)
pressed : top=blend(primary, scale[5], 0.10), bottom=blend(primary, scale[2], 0.05)
disabled: top=scale[2],            bottom=scale[5]
inset   : top=scale[6],            bottom=scale[4]
```

## 출력 형식 (Aurum 스키마)

```json
{
  "meta":     { "name": "...", "id": "...", "source": "ElectronTheme", "scheme": "dark|light", "version": "2.0.0" },
  "surface":  { "normalTop/Bottom", "hoverTop/Bottom", "pressedTop/Bottom", "disabledTop/Bottom", "insetTop/Bottom" },
  "gold":     { "border", "hover", "highlight", "bright", "shadow", "muted" },
  "gloss":    { "tint", "normalAlpha", "hoverMultiplier", "disabledMultiplier", "heightRatio", "blur", "centerX" },
  "semantic": { "text", "disabledText", "danger", "success", "warning", "focus" },
  "geometry": { "radius", "borderWidth", "innerRim", "depth", "shadow", "shadowAlpha" },
  "state":    { "hoverLift", "checkedGoldMix", "disabledContrast" }
}
```

- `gloss`/`geometry`/`state`는 `DEFAULT_THEME` (Modern Dark Gold 기준) 값을 그대로 상속
- `meta.scheme`이 `light`인 파일은 런타임에서 `UIThemeMapper::ApplyLightScheme` 변환을
  건너뛰고 파일 값이 그대로 적용됨 (light 값은 변환 시점에 이미 생성됨)

## 런타임 적용 (sgcl)

- 배포 위치: `UltimateHarmony_Dev\assets\res_data\theme\{id}.{mode}.json`
- 테스트 화면(`UI_Generic`)의 테마 버튼 39종 + 스키마 버튼(Dark/Light/System)으로
  실시간 전환 가능
- 테마 JSON 로드: `ResolveThemeJsonPath()` → `{id}.{mode}.json` 우선, 없으면 `{id}.json` 폴백

## 유의사항

- `mix()`는 스튜디오(engine.js)와 동일한 0~255 정수 공식 — `gold` 계열은 이 공식을 사용
- `_blend(fg, bg, alpha)` 인자 순서에 주의: `_blend(primary, color, 0.06)` = primary 6% 블렌드
- light 배경은 리졸버 원본(고정 순백 0.991) 대신 **테마 시드 톤 보존**으로 의도적 변경 —
  opencode 원본과 값이 다를 수 있음
- Python round()는 banker's rounding — 색상 환산에는 `int(v*255+0.5)`(half-up) 사용
