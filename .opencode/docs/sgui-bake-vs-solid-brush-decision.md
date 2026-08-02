# sgui — 텍스처 베이크 탈피 판단 (런타임 단색 브러시 전환 비용/메모리 분석)

> 작성일: 2026-08-02
> 배경: `sgui-wpf-brush-model.md`의 후속. "현재 texture theme bake 시스템을 버리고
> 단색(런타임 브러시) 시스템으로 가는 방향이 비용/메모리 측면에서 어떠한가"를 판단.
> 목표: **WPF와 거의 동일한 동작 모델**(브러시=값, 도형=런타임 래스터화)

---

## 0. 요약 판단 (Verdict)

**전환 권장.** 표면/테두리/모서리를 "베이크 텍스처 + 9-slice"에서
"런타임 브러시(단색/그라데이션) + SDF 셰이더"로 바꾸는 것이

- **VRAM/디스크 메모리**: 약 0.6MB/테마 → ~0.1MB (아이콘 제외 시 서피스 메모리 0) — **5~10배 절감**
- **테마 변경 비용**: 수십~수백 ms의 메인 스레드 풀베이크 → **0ms (속성 교체만)**
- **코드 유지비**: 베이크 서브시스템 14~16개 파일 삭제, 캐시/해시/버전 체계 전면 제거
- **WPF 일치도**: 브러시=값, 모서리=런타임 도형, 상태 전환=속성 교체 — WPF와 사실상 동일

단, **2가지 전제 조건**을 반드시 지킬 것:
1. **SVG 아이콘만 베이크 유지** (런타임 벡터 렌더러가 없으므로 nanosvg 래스터화 필요)
2. **소프트 섀도/글로스는 근사치 수용** (유일한 화질 손실 지점 — 5장 참조)

---

## 1. 현재 베이크 시스템의 실제 비용 (수치 근거)

### 1.1 베이크 산출량 (확인된 사실)

- `UIThemeManager::BuildThemeVariants` (UIThemeManager.cpp:131-160)
  → **20 시맨틱 × 4 상태(Normal/Hover/Pressed/Disabled) = 80 텍스처/테마 1세트**
- 각 항목이 레시피 해상도 그대로 **개별 Texture2D로 생성** (아틀라스 배칭 없음, UITextureBakeService.cpp:168-186)
- 베이크 과정: 4x 수퍼샘플(레시피×4 해상도 워크버퍼) → SDF/블러/그라데이션 패스
  → Lanczos 다운샘플 (UITextureBaker.cpp:183-196)

### 1.2 VRAM 메모리 추정

레시피 해상도 기준 RGBA8888 크기:

| 시맨틱 | 크기 | 바이트 |
|---|---|---|
| Button, EditBox | 160×44 | 28.2KB × 2 |
| Frame | 80×80 | 25.6KB |
| ScrollBarTrack | 16×120 | 7.7KB |
| ProgressTrack | 96×20 | 7.7KB |
| ToggleTrack | 54×28 | 6.0KB |
| ProgressGauge, SliderTrack, SliderFill | 96×12 | 4.6KB × 3 |
| CheckBox, Radio | 32×32 | 4.1KB × 2 |
| ToggleKnob, SliderThumb, ScrollBarThumb | 24×24 | 2.3KB × 3 |
| 아이콘 3종 | 24×24 | 2.3KB × 3 |
| RadioDot, ProgressCap | 14×14 | 0.8KB × 2 |
| CheckMark | 12×12 | 0.6KB |

**상태 1세트 ≈ 141KB → 4상태 ≈ 0.55MB/테마세트**
- GL 텍스처 오브젝트 오버헤드(각 80개), 세대 전환 시 이전 세트가 deferred release로
  짧게 중첩 → 순간 1~1.5MB
- `UITextureCacheKey`는 styleHash×recipeHash×stateHash 키 → **스튜디오에서 슬라이더를
  움직일 때마다 새 해시 → 새 텍스처가 캐시에 적체** (편집 세션 중 2~3배 누적 가능)
- `persistCache` 디스크 캐시 + Light/Dark × DPI 배수까지 고려하면 **수 MB까지 확장 가능**

### 1.3 CPU 비용 (확인된 사실)

- `UITextureBakeService::Update` → 디바운스 0.15s 후 **메인 스레드에서 동기 실행**
  (UITextureBakeService.cpp:55-70, 백그라운드 스레드 아님)
- 1회 풀베이크 = 80텍스처 × (4x 워크버퍼: Button 기준 640×176 = 11.3만 픽셀) ×
  (마스크/표면/글로스/섀도/테두리/림 + 가우시안 블러 2회 패스)
  → 추정 **수십~수백 ms 히치** (단일 코어 연산량 수억 회)
- 텍스처 업로드(glTexImage2D × 80) 포함
- **테마 스튜디오에서 편집할 때마다 반복 발생** → 실시간 미리보기 불가, 매 편집 시 프레임 드랍

---

## 2. 단색(런타임 브러시) 시스템의 비용

| 자원 | 단색/런타임 시스템 비용 |
|---|---|
| 서피스/테두리/모서리 텍스처 | **0** — 색상은 구조체 값일 뿐 |
| 그라데이션 | **0** — 셰이더 uniform (스톱 배열 전달) |
| 둥근 모서리 | 쿼드 1개 + 프래그먼트 SDF 연산(≈10ALU/px) — GPU가 프레임 해상도에서 계산 |
| 9-slice 대체 | Scale9Sprite 9쿼드 → **쿼드 1개** (드로우콜·정점수 감소) |
| 테마 변경 | **0ms** — 컬러 값만 교체, 재업로드/재베이크 없음 |
| DPI | dpiScaleQ8 캐시 불필요 — 해상도 독립 (셰이더는 픽셀 공간에서 계산) |
| 유지 필수 | SVG 아이콘 베이크 (소형 ~100KB) |
| 섀도 (선택) | 공유 소프트 섀도 텍스처 1장 256×256 = 256KB (전 요소 공유) |

---

## 3. 항목별 비교표

| 항목 | 베이크 + 9-slice (현재) | 런타임 단색 브러시 (제안) | 판정 |
|---|---|---|---|
| VRAM (서피스) | ~0.55MB/세트 + 캐시 누적 | 0 | 단색 승 |
| VRAM (아이콘) | ~7KB | ~7KB (유지) | 동일 |
| 테마 전환 시간 | 50~300ms (메인 스레드) | ~0ms | 단색 승 |
| 프레임 렌더 | 9쿼드/요소, 텍스처 샘플링 | 1쿼드/요소, SDF 프래그먼트 | 단색 승 (소폭) |
| 해상도/DPI | 베이크 시점 해상도 고정, 스트레칭 시 흐림 | 픽셀 퍼펙트 | 단색 승 |
| 작은 컨트롤 | FlatFallback 각진 사각형 (BrushVisual.cpp:120) | 라운드 유지 (구조적으로 해결) | 단색 승 |
| 소프트 섀도/글로스 블러 | 가우시안 블러 (UIGaussianBlur) | 폴오프 근사 필요 | 베이크 승 (유일) |
| 코드 규모 | 베이커 8 + 시리얼 1 + 캐시 3 + 바인딩 1 + 레시피 1 ≈ 14~16파일 | 셰이더 2~3개 + Brush 확장 | 단색 승 |
| 유지보수 복잡도 | 해시/버전/디바운스/캐시 무효화 체계 | 값 → 렌더 직결 | 단색 승 |

---

## 4. WPF와의 동작 일치도 — "WPF처럼"의 실체

WPF의 동작 모델을 코코스 관점으로 옮기면:

| WPF 동작 | 코코스에서 동일하게 되는가 |
|---|---|
| `Background/Foreground/BorderBrush` = Brush 객체 (SolidColor/LinearGradient) | ✅ BrushPtr 값으로 동일 |
| 상태 전환(IsMouseOver 등) → 브러시 교체 | ✅ `UpdateVisualState → RefreshThemeVisuals`가 색 교체만 수행 (지연 0) |
| `CornerRadius` = 런타임 도형(라운드 사각형) | ✅ SDF 셰이더 — 모서리 4방향까지 동일 |
| `BorderThickness(left,top,right,bottom)` | ✅ 셰이더 per-edge SDF로 동일 표현력 |
| 브러시 인스턴스 공유 (Freezable) | ✅ `std::shared_ptr<Brush>` 공유 — 이미 동일 |
| 도형 래스터화 = GPU | ✅ 커스텀 GLSL 셰이더 (방식만 다르고 시각 동일) |
| `DropShadowEffect` (GPU 블러) | ⚠️ 코코스 기본에 없음 — 근사 필요 (5장) |
| 텍스트 그라데이션 전경 | ⚠️ 셰이더 또는 텍스트 베이크 필요 |

**결론**: "거의 동일"은 실현 가능하다. 차이가 나는 부분은 GPU 블러 이펙트 하나뿐이며,
그것도 공유 섀도 텍스처/폴오프로 실용적 근사가 가능하다.
오히려 베이크 모델이 WPF와 다른 점(값 변경에도 텍스처 재생성)이 이번 전환으로 사라진다.

---

## 5. 잃는 것 vs 얻는 것 (화질 영향 분석)

### 잃는 것 (전환 시 근사 필요)

1. **소프트 섀도** (geometry.shadow + UIGaussianBlur)
   - 현재: 둥근 마스크를 가우시안 블러 후 낮은 알파로 드롭
   - 대안: ① 공유 소프트 섀도 텍스처 1장(256×256 방사형 폴오프)을 라운드 마스크와 곱해
     스트레치 — 모든 요소가 같은 섀도 품질, 비용 0, 가장 추천
     ② SDF 폴오프 섀도(경계만 smoothstep) — 작은 섀도(radius 1~4px)는 이걸로 충분
   - 실제 영향: 섀도 반경이 작아(기본 1.0, 최대 수 px) 체감 차이 미미
2. **글로스 블러** (gloss.blurRadius)
   - 현재 RenderSoftGloss 자체가 타원 폴오프(`1 - dist²`) 기반이고 블러는 감쇠 보조
   - 대안: 셰이더에서 동일 폴오프 계산 (블러 생략해도 시각 차이 미미)
3. **금속 테두리 그라데이션** (borderGradient)
   - 대안: 셰이더 각도 램프(현재 ResolveMetalColor의 q 계산과 동일 수식) — 동일 구현 가능
4. **depth/inset 표면** — 내부 엣지 그라데이션(SDF 안쪽 옵셋)으로 동일 구현 가능

### 얻는 것 (베이크 방식에 없던 이득)

- **9-slice 저해상도 텍스처(160×44) 스트레칭이 사라진다** — 고해상도 모니터에서
  현재보다 엣지가 더 선명해진다 (SDF는 프레임 해상도에서 계산)
- 수퍼샘플/다운샘플/알파 엣지 검증(ValidateAlphaEdges) 같은 보정 체계 불필요
- DPI 독립 — dpiScaleQ8 캐시 차원 제거
- 작은 컨트롤의 "각진 FlatFallback" 문제가 구조적으로 소멸 (라운드가 항상 보장)

---

## 6. 코드 정리 효과

### 삭제 가능 (기능 이전/포기)

| 파일 | 역할 |
|---|---|
| `Baker/UIPixelBuffer.*`, `UIMaskBuffer.*` | 픽셀 워크버퍼 → 셰이더로 대체 |
| `Baker/UIShapeRasterizer.*`, `UIGradientRasterizer.*` | CPU 래스터화 → SDF 셰이더 (수식 이전) |
| `Baker/UIGaussianBlur.*`, `UIColorMath.*` | 셰이더로 대체 (가우시안은 폴오프 근사) |
| `Baker/UITextureBaker.*` | **아이콘 전용으로 축소** (nanosvg 경로만 유지) |
| `Baker/UIControlBakeRecipes.*` | 삭제 (슬라이스 인셋 불필요) |
| `Serialization/UITextureBinaryWriter.*` | 삭제 (디스크 캐시 불필요) |
| `UITextureCache.*`, `UITextureEntry.*`, `UITextureBakeService.*` | 삭제 (텍스처 캐시 체계 전부) |
| `UIThemeBinding.*` | 삭제 (바인딩 불필요 — 브러시가 직접 참조) |
| `UIThemeTypes.h`의 `UIAssetKey/UITextureCacheKey/UIThemeRevision(textureRevision)` 해시 체계 | 삭제 |
| `BrushVisual`의 Scale9/FlatFallback 분기 | 1/3로 축소 (쿼드 1개 + 셰이더) |

→ **약 14~16개 파일 제거**, `UIThemeManager`는 토큰 해석기로 단순화

### 남는 것

- `Brush.h` (3종 → +DynamicBrush), `BrushVisual.*` (축소), `UITypes.h` (CornerRadius 활성화)
- `UIRuntimeTheme`, `UIStyleResolver`, `UIStyleOverride` (스타일 해석은 그대로 — 이게 곧 WPF 스타일 시스템)
- `UIThemeManager` (해석 + 셰이더 파라미터 전달만)
- 아이콘용 미니 베이커 (nanosvg)
- 셰이더 2~3개: `roundRectSDF`(서피스+테두리), `gradientRect`(멀티스톱/방향/라디얼), `textGradient`(전경)

### 신규 리스크 (주의)

- 코코스 커스텀 GLSL 셰이더 첫 도입: premultiplied alpha, blend state, 배치(batch) 순서,
  uniform 갱신 빈도 — `BrushVisual`이 셰이더 상태를 잘 관리해야 함
  → 시작은 내장 `LayerColor/LayerGradient`(셰이더 0개)로 안전하게 전환 후 SDF 셰이더 도입

---

## 7. 이 방향이 틀리는 경우 (베이크를 계속 써야 하는 조건)

1. **PNG/아트 리소스 기반 테마를 도입할 계획** — 디자이너가 만든 질감 텍스처를
   9-slice로 쓰려면 베이크/아틀라스 파이프라인이 필수. 이 문서의 판단은
   "테마 = 100% 절차적(코드/JSON로 생성)" 전제에서만 유효
2. 미세한 블러 품질(글로스/섀도)이 브랜드 기준인 경우 — 셰이더 근사로 대체 불가라면 유지
3. 셰이더 도입이 불가능한 타겟 (GL 1.x 등 구형 환경) — 코코스 데스크톱(GL 3.x+) 기준으론 무관

---

## 8. 권장 전환 전략 (이전 문서 로드맵 수정판)

| 단계 | 내용 | 셰이더 | 리스크 | 검증 |
|---|---|---|---|---|
| **A** | 서피스를 런타임 브러시로: `ThemeBrush`를 "스타일 해석 → UIColorF 2색"으로 축소, `BrushVisual`이 LayerColor/LayerGradient 사용. 베이커는 아이콘만 유지, 9-slice 경로 임시 유지 | 0개 | 낮음 | 기존 화면과 색상 동일성 비교 |
| **B** | SDF 셰이더 도입: 둥근 모서리 4방향 + 테두리(uniform→per-edge). Scale9Sprite 제거, 해시/캐시/버전 체계 삭제 | 1개 | 중간 | 스튜디오에서 radius 4방향/테두리 4변 편집 비교 |
| **C** | 그라데이션 확장(스톱/방향/라디얼) + 전경 브러시 + 텍스트 그라데이션 | 2~3개 | 중간 | WPF 예제 XAML 스타일 재현 |
| **D** | 섀도/글로스 근사 + 베이크 잔여 파일 삭제 + 9-slice 잔재 정리 | 3개 | 낮음 | 전체 테마 화면 비교 회귀 테스트 |

각 단계 종료 시 `sgcl` 빌드(x64) + 테마 스튜디오 실사 확인.
A단계만으로도 메모리/히치 문제의 대부분이 해소되므로, **A단계 후 중간 평가**를 권장한다.

---

## 9. 결론

- **비용/메모리 모두 런타임 단색 브러시가 압도적으로 유리**:
  VRAM 5~10배 절감, 테마 변경 히치 제거, 코드 14~16파일 삭제
- **WPF 동작 모델과의 일치도가 오히려 더 높아진다**: 브러시=값, 상태=속성 교체,
  모서리/테두리=런타임 도형 — 베이크 모델은 WPF와 반대로 "값 변경에도 텍스처 재생성"이
  일어났다
- **유일한 예외와 손실**: SVG 아이콘(베이크 유지), 소프트 섀도/글로스(근사) — 둘 다
  실용적으로 해결 가능
- **권장 결론: 전환하되 Phase A부터 순차 진행**, 기존 베이크 코드는 아이콘 용도로
  축소 유지 (시맨틱 질감 테마 도입 가능성 대비)
