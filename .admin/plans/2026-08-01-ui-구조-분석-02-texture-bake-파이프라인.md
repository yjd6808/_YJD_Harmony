# SGCL UI 전체 구조 분석 - 02. Texture Bake 파이프라인 (상세)

> 작성일: 2026-08-01
> 위치: `Projects/sgcl/Classes/sgcl/Game/UI/Theme/Baker/`, `UITextureBakeService.*`, `UITextureCache.*`, `UITextureEntry.h`

## 0. 전체 파이프라인 (한눈에)

```
[1] Request 생성                      UIThemeManager::BakeDefaultTextureSet / UIThemeStudio
       └─ UIThemeBakeRequest {theme, dpiScale, preview, variants[]}
[2] 디바운스 큐잉                     UITextureBakeService::EnqueueRequest → Update(0.15s) → ExecuteBake
[3] 레시피 선택                       SelectRecipe(semantic) → UIAssetRecipe {width,height,sliceInsets,minimumSize}
[4] 캐시 조회                         UITextureCache::Find(UITextureCacheKey) → 히트 시 재사용
[5] 픽셀 생성                         UITextureBaker::Bake(recipe, style, options)
       ├─ 업스케일 작업 버퍼 (supersample=4)  → workW x workH
       ├─ 시맨틱별 렌더 분기 (RenderMaterial / RenderGaugeFill / RenderCheckMark / ...)
       ├─ Lanczos 다운샘플 → 원본 크기
       └─ ValidateAlphaEdges
[6] Texture2D 생성                    initWithData(RGBA8888) → UITextureEntry
[7] TextureSet에 등록                 UITextureSet::AddEntry + UITextureCache::Insert
[8] 완료 통지                          ProcessCompleted → onBakeCompleted_ → SwapTextureSet
[9] 컨트롤 바인딩                     UIThemeTextureBinding::Refresh(activeTextureSet)
```

---

## 1. 베이크 요청/디바운스 (UITextureBakeService)

### 1-1. 요청 구조체
- `UIThemeBakeRequest` (UITextureBakeService.h:17-27): generation, theme, resolvedScheme, dpiScale, preview, persistCache, variants[]
- `UIResolvedVariantRequest` (UITextureBakeService.h:9-15): asset(UIAssetKey), state, style(UIResolvedStyle), styleHash

### 1-2. 큐잉/디바운스 (UITextureBakeService.cpp)
```cpp
EnqueueRequest(request)   // :18-31  generation>0이면 기존 pending 삭제 후 push, 디바운스 시작
CancelPending(gen)        // :33-46  이전 generation 제거
Update(dt)                // :48-63  debounceTime_=0.15s 경과 시 Back() 하나만 ExecuteBake
ExecuteBake(request)      // :83-94  BuildTextureSet → completedResults_에 PushBack
ProcessCompleted()        // :65-81  onBakeCompleted_(set) 콜백 후 release
```
- **디바운스 목적**: 스튜디오에서 슬라이더로 값을 연타할 때 매 프레임 베이크하는 것을 막고 0.15초로 모음.
- `BuildTextureSet`은 공개(sync)로도 호출 가능 — BakeDefaultTextureSet이 동기 사용.

## 2. 레시피 (UIControlBakeRecipes / UITextureBakeService::SelectRecipe)

`SelectRecipe` (UITextureBakeService.cpp:96-117)가 시맨틱별 고정 크기 레시피를 반환.
실제 슬라이스 값은 `UIControlBakeRecipes.cpp` 참조:

| 시맨틱 | 레시피 함수 | 크기 | sliceInsets | minimumSize | 용도 |
|---|---|---|---|---|---|
| Button | ButtonRecipe | 160×44 | 14,14,14,14 | 28×28 | 버튼 본체 (Scale9) |
| Frame | FrameRecipe | 80×80 | 10,10,10,10 | 20×20 | 프레임 테두리 |
| CheckBox | CheckBoxShellRecipe | 32×32 | 10,10,10,10 | 20×20 | 체크박스 셸 (Scale9) |
| CheckMark | CheckMarkRecipe | 12×12 | 없음 | - | 체크 표시 (Fixed) |
| Radio | RadioShellRecipe | 32×32 | 10,10,10,10 | 20×20 | 라디오 셸 |
| RadioDot | RadioDotRecipe | 14×14 | 없음 | - | 라디오 점 (Fixed) |
| ToggleTrack | ToggleTrackRecipe | 54×28 | 6,6,6,6 | 12×12 | 토글 트랙 (Scale9) |
| ToggleKnob | ToggleKnobRecipe | 24×24 | 없음 | - | 토글 노브 (Fixed) |
| ProgressTrack | ProgressTrackRecipe | 96×20 | 8,8,8,8 | 16×16 | 게이지 트랙 (Scale9) |
| ProgressGauge | ProgressGaugeRecipe | 96×12 | 6,0,6,0 | 12×12 | 게이지 채움 (Scale9) |
| ProgressCap | ProgressCapRecipe | 14×14 | 없음 | - | 게이지 캡 (Fixed) |
| SliderTrack | SliderTrackRecipe | 96×12 | 6,6,6,6 | 12×12 | 슬라이더 트랙 |
| SliderFill | SliderFillRecipe | 96×12 | 6,6,6,6 | 12×12 | 슬라이더 채움 |
| SliderThumb | SliderThumbRecipe | 24×24 | 8,8,8,8 | 16×16 | 슬라이더 손잡이 |
| ScrollBarTrack | ScrollBarTrackRecipe | 16×120 | 4,4,4,4 | 8×8 | 스크롤바 트랙 |

- `UIAssetRecipe` (UIPixelBuffer.h:54-61): semantic, width, height, sliceInsets, minimumSize
- `UIBakeOutput` (UIPixelBuffer.h:63-69): buffer, sliceInsets, minimumSize, contentPadding
  - **contentPadding**은 Bake 시 그림자/depth로 인한 머터리얼 내부 여백 (Baker:150에서 계산) → ProgressBar의 `gaugeInset_` 계산에 사용됨

## 3. 픽셀 베이크 (UITextureBaker::Bake) — 핵심

### 3-1. Bake 진입 (UITextureBaker.cpp:85-152)
```cpp
UIBakeOutput UITextureBaker::Bake(recipe, style, options)
{
    int ss = options.supersample;            // = 4 (BakeService.cpp:126)
    workW = recipe.width * ss;  workH = recipe.height * ss;
    UIResolvedStyle scaled = style;          // geometry 값을 ss 배로 스케일
    UIPixelBuffer work(workW, workH);        // 투명으로 클리어
    materialBounds = CalculateMaterialBounds(workW, workH, scaled, /*shadow*/true);
    gaugeBounds   = CalculateMaterialBounds(workW, workH, scaled, /*shadow*/false);

    switch (recipe.semantic)
    {
      Button/CheckBox/ToggleTrack/SliderTrack/ProgressTrack/ScrollBarTrack → RenderMaterial
      Frame                  → RenderFrameOnly
      CheckMark              → RenderCheckMark
      RadioDot               → RenderRadioDot
      ProgressGauge/SliderFill → RenderGaugeFill
      default(기타)           → RenderMaterial
    }
    output = DownsampleLanczos(work, recipe.width, recipe.height);  // 4x → 1x
    ValidateAlphaEdges(output);
    result.contentPadding = materialBounds.x / ss;
}
```

### 3-2. CalculateMaterialBounds (UITextureBaker.cpp:154-163)
```cpp
padding = shadowEnabled ? geometryShadow*2 + geometryDepth + 2.0f : 0.0f;
padding = min(padding, min(workW, workH) * 0.24f);   // 크기 대비 24% 한도
return { padding, padding, workW - padding*2, workH - padding*2 };
```
- **shadowEnabled=true**: 버튼 등 그림자 있는 재질 → 바깥 여백 확보
- **false**: 게이지 등 → 여백 0

### 3-3. RenderMaterial (UITextureBaker.cpp:9-26) — 버튼/트랙/셸 공통
```
1. RenderShadow          (UIGradientRasterizer.cpp:84-105)
2. RasterizeRoundedMask  (UIShapeRasterizer.cpp:23-42)  → shapeMask
3. RenderSurface         (UIGradientRasterizer.cpp:9-42)  top→bottom 세로 그라데이션
4. RenderSoftGloss       (UIGradientRasterizer.cpp:44-82)  shapeMask 마스킹 글로스
5. RenderBorder          (UIGradientRasterizer.cpp:118-148) 메탈 경계선
6. RenderInnerRim        (UIGradientRasterizer.cpp:150-178) 안쪽 림
```

### 3-4. RenderGaugeFill (UITextureBaker.cpp:28-36) — 게이지/슬라이더 채움
```cpp
RenderSurface(work, gaugeBounds, geometryRadius, metalBright, metalHighlight);
```
- **밝은 골드 그라데이션** (metalBright=(240,217,149) → metalHighlight=(216,192,120)).
  - (이전에 surface+metalHighlight 블렌드였으나 2026-07 작업으로 현재 위처럼 변경됨)

### 3-5. RenderCheckMark (UITextureBaker.cpp:38-52) / RenderRadioDot (54-69) / RenderFrameOnly (71-83)
- CheckMark: 3점 곡선을 `RasterizeRoundLine` 2번으로 그림 (semanticText 색)
- RadioDot: 원 마스크에 semanticText 알파 블렌드
- FrameOnly: 그림자 + 보더 + 림 (표면 없음)

## 4. 거리 필드 래스터라이저 (UIShapeRasterizer)

- `RoundedRectDistance(px,py,rect,radius)` (UIShapeRasterizer.cpp:7-21): **SDF 기반** — 사각형에 라운드를 더한 최단거리
- `RasterizeRoundedMask(out, rect, radius, fillAlpha)` (23-42): 각 픽셀 `d = RoundedRectDistance(x+0.5,y+0.5,...)`, `coverage = CoverageFromDistance(d)` = `clamp01(0.5 - d)` (UIColorMath.h:49-52)
  - 즉 d<0.5이면 완전히 덮고, 경계에서 안티앨리어싱
- `RasterizeCircleMask` (44-66): 원 SDF
- `RasterizeRoundLine` (68-112): 선분 + 반지름(halfW) 구간 SDF, 원형 끝단

## 5. 그라데이션/재질 렌더러 (UIGradientRasterizer)

| 함수 | 라인 | 동작 |
|---|---|---|
| RenderSurface | 9-42 | 세로 선형 그라데이션 + SmoothStep01 + rounded coverage. `_reverse` 지원 |
| RenderSoftGloss | 44-82 | 타원형 글로스 마스크 생성 → 가우시안 블러(glossTint.b*10 or 3.0) → shapeMask 곱해서 흰색 덧칠 |
| RenderShadow | 84-105 | shadowRect = rect + (0, depth), mask 0.58 → 블러(geometryShadow*2) → 검정 alpha*0.48 |
| RenderBorder | 118-148 | `q=(x+y)/(w+h)`로 대각선 방향 메탈 그라데이션. `ResolveMetalColor(q)`: q≤0.32이면 highlight→border, 그 뒤 border→shadow (107-116) |
| RenderInnerRim | 150-178 | borderWidth~borderWidth+innerRim 구간에 어두운 림 |

## 6. 버퍼/수학/블러

### 6-1. UIPixelBuffer (UIPixelBuffer.cpp)
- RGBA8888, stride = w*4. `Clear/Store/Load/BlendOver`
- `BlendOver` (59-76): **정규 알파 블렌딩** (src-over, alpha 재정규화)
- `Resize` (9-16): 픽셀 0으로 초기화

### 6-2. UIMaskBuffer (UIMaskBuffer.h:6-30)
- float 알파 그리드. `operator()(x,y)`로 접근. `Mul(mask/scalar)`

### 6-3. UIColorMath (UIColorMath.h)
- `Clamp01`, `SmoothStep01`, `Lerp`, `SRGBToLinear`, `LinearToSRGB`, `CoverageFromDistance(d)=clamp01(0.5-d)`

### 6-4. UIGaussianBlur (UIGaussianBlur.cpp)
- `BuildKernel` (7-27): 반경 r = ceil(radius), sigma=max(radius,0.5) 가우시안 커널 정규화
- `BlurSeparable` (29-65): **세로/가로 분리형** (O(r·w·h))

## 7. 다운샘플 (DownsampleLanczos) — UITextureBaker.cpp:165-227

- 4x supersample 버퍼 → 원본 크기. **Lanczos 계열 가중치** 사용
- 가중치: `w = sinc(π·dx)·sinc(π·dx/sx)` (windowed sinc), `sx = srcW/dstW`
- **선형 색 공간에서 평균** 후 sRGB 복원 (SRGBToLinear → 가중합 → LinearToSRGB, 206-220) — 밝은색 번짐 방지

## 8. 캐시 (UITextureCache)

- `maxMemoryBytes_ = 64MB` (UITextureCache.h:32), `EvictIfNeeded`는 **LRU**(lastAccessFrame) 퇴출 (UITextureCache.cpp:92-117)
- `Insert` (38-53): 메모리 바이트 = w*h*4 누적, 초과 시 퇴출
- `Find` (13-36): UITextureCacheKey 8필드 전부 일치해야 히트
- `UITextureDeferredRelease::ReleaseAfterFrames(obj, 2)` (119-140): 텍스처셋 교체 시 2프레임 지연 릴리스 (SwapTextureSet에서 사용, UIThemeManager.cpp:206)

## 9. TextureSet / Entry (UITextureEntry.h)

```cpp
class UITextureEntry {   // :5-33
    UIAssetKey key;
    cc::Texture2D* texture;      // RGBA8888
    cc::Rect atlasRect;
    UISliceInsets sliceInsets;
    cc::Size minimumSize;
    cc::Rect contentRect;
    float contentPadding;        // ← 게이지 인셋 등에 사용
    bool rotated, premultipliedAlpha;
};
class UITextureSet : public cc::Ref {  // :35-55
    uint64_t generation_, themeHash_;
    jc::HashMap<UIAssetKey, UITextureEntry*> entries_;
    const UITextureEntry* Find(const UIAssetKey&) const;
};
```

## 10. 컨트롤 바인딩 (UIThemeBinding)

`UIThemeTextureBinding::Refresh(set)` (UIThemeBinding.cpp:25-89):
```cpp
for slot in slots_:
    entry = set.Find(slot.key);
    if Scale9:  setSpriteFrame(tex, rect, {sliceInsets...}) + setPreferredSize(contentSize)
    if Fixed:   sprite->initWithTexture(tex, atlasRect)
```
- `BindScale9(target, key, slot)` / `BindFixed(target, key, slot)` (UIThemeBinding.cpp:4-18)
- **주의**: `setSpriteFrame`가 Scale9의 contentSize를 텍스처 크기로 리셋 → 각 컨트롤의 BuildThemeVisuals 끝에서 `setContentSize(uiSize_)` 복구
- 컨트롤별 바인딩 조합:
  | 컨트롤 | Bind 목록 | 파일:라인 |
  |---|---|---|
  | UIButton | Scale9(Button, Background) | UIButton.cpp:323 |
  | UICheckBox | Scale9(CheckBox, Shell) + Fixed(CheckMark, Mark) | UICheckBox.cpp:285,294 |
  | UIToggleButton | Scale9(ToggleTrack, Track) + Fixed(ToggleKnob, Knob) | UIToggleButton.cpp:276,286 |
  | UIProgressBar | Scale9(ProgressTrack,Track) + Scale9(ProgressGauge,Gauge) + Fixed(ProgressCap,Cap) | UIProgressBar.cpp:160,170,180 |

## 11. 게이지 레이아웃 계산 (UIProgressBar)

- `gaugeInset_ = trackEntry->contentPadding + resolved.geometryBorderWidth` (UIProgressBar.cpp:194)
- `SetPercent(p)` (UIProgressBar.cpp:296-360):
  ```
  bodyW = uiSize_.width  - gaugeInset_*2
  bodyH = uiSize_.height - gaugeInset_*2
  visibleWidth = bodyW * p/100
  pGauge setScaleX(visibleWidth / bodyW)  → 게이지 스케일
  pGauge setPosition(gaugeInset_, gaugeInset_)
  pGaugeCap setPosition(visibleWidth/2 + gaugeInset_, uiSize_.height/2)  (cap 있는 경우)
  ```

## 12. 시리얼라이제이션 (UITextureBinaryWriter) — 캐시 디스크 저장용

- `WriteToFile` (UITextureBinaryWriter.cpp:4-33): `UITXHeader`(매직 0x58544955 = "UITX") + 픽셀 원본
- `ReadFromFile` (35-68): 헤더 검증 후 버퍼 복원
- 현재 BakeDefaultTextureSet은 `persistCache=true`지만 디스크 저장 경로는 미사용 (cacheRoot="")
