# sgui에 WPF Brush 모델 접목 가이드

> 작성일: 2026-08-02
> 범위: `Projects/sgcl/Classes/sgcl/Game/UI` — 테마 베이크 + 9-slice 시스템에
> WPF의 Brush(CornerRadius / Gradient / SolidColor / BorderBrush / BorderThickness / Foreground / Background) 개념을 접목하는 설계 문서
> 상태: 설계 제안 (코드 미반영)
> 2026-08-02 갱신: 베이크 유지/탈피 판단은 `sgui-bake-vs-solid-brush-decision.md` 참조
> — 결론: 표면/테두리/모서리는 런타임 브러시(단색+셰이더) 전환 권장

---

## 1. 먼저: WPF는 모서리를 어떻게 처리하나

### 1.1 핵심 전제

WPF는 **"베이크하지 않는다"**. 모든 브러시/모서리는 런타임에 벡터(Geometry)로 해석되고
GPU에서 즉석 래스터화된다. 반면 하모니(sgui)는 **CPU에서 텍스처를 베이크하고
9-slice로 늘리는 방식**이다. 이 둘의 트레이드오프가 아래 모든 설계 결정의 출발점이다.

| | WPF | 하모니(sgui) 현재 |
|---|---|---|
| 모서리 처리 | `Border.CornerRadius` → RoundedRect Geometry | 베이커의 SDF로 둥근 마스크 생성 → 텍스처에 베이크 |
| 그라데이션 | `LinearGradientBrush`(다중 스톱/방향) 실시간 채움 | 표면 2색(top/bottom)만 베이크 |
| 테두리 | `BorderBrush` + `BorderThickness`(Thickness, 변마다 다름) | `geometry.borderWidth` 단일 값 + `metal.border` 색 |
| 전경 | `Foreground` (SolidColorBrush 포함, 자식 상속) | `UIColorF` 상속만 |
| 런타임 비용 | 프레임마다 벡터 래스터화 (GPU) | 베이크 1회 + Scale9 스프라이트 (저비용) |

### 1.2 WPF 코드 예시 1 — 둥근 버튼 (CornerRadius)

**WPF의 `Button` 자체에는 `CornerRadius` 속성이 없다.** 둥근 버튼은
`ControlTemplate` 안의 `Border`가 담당한다. 이것이 "버튼을 둥글게 만드는" 정석 방법이다.

```xml
<Window.Resources>
    <Style x:Key="RoundedButtonStyle" TargetType="Button">
        <Setter Property="Background"     Value="#2D3035"/>
        <Setter Property="Foreground"     Value="#F2F0EA"/>
        <Setter Property="BorderBrush"    Value="#A98A4A"/>
        <Setter Property="BorderThickness" Value="1.5"/>
        <Setter Property="Padding"        Value="14,6"/>
        <Setter Property="Template">
            <Setter.Value>
                <ControlTemplate TargetType="Button">
                    <!-- CornerRadius는 여기 Border가 소유한다 -->
                    <Border x:Name="bd"
                            Background="{TemplateBinding Background}"
                            BorderBrush="{TemplateBinding BorderBrush}"
                            BorderThickness="{TemplateBinding BorderThickness}"
                            CornerRadius="8"
                            SnapsToDevicePixels="True">
                        <ContentPresenter HorizontalAlignment="Center"
                                          VerticalAlignment="Center"/>
                    </Border>
                    <ControlTemplate.Triggers>
                        <Trigger Property="IsMouseOver" Value="True">
                            <Setter TargetName="bd" Property="Background" Value="#4A4E55"/>
                        </Trigger>
                        <Trigger Property="IsPressed" Value="True">
                            <Setter TargetName="bd" Property="Background" Value="#161217"/>
                        </Trigger>
                        <Trigger Property="IsEnabled" Value="False">
                            <Setter TargetName="bd" Property="Background" Value="#3A3C40"/>
                        </Trigger>
                    </ControlTemplate.Triggers>
                </ControlTemplate>
            </Setter.Value>
        </Setter>
    </Style>
</Window.Resources>

<Button Style="{StaticResource RoundedButtonStyle}" Content="로그인"/>
```

코드비하인드(C#)로도 동일하게 만들 수 있다:

```csharp
var bd = new Border
{
    CornerRadius = new CornerRadius(8),
    Background = new SolidColorBrush(Color.FromRgb(0x2D, 0x30, 0x35)),
    BorderBrush = new SolidColorBrush(Color.FromRgb(0xA9, 0x8A, 0x4A)),
    BorderThickness = new Thickness(1.5),
    Child = new ContentPresenter { HorizontalAlignment = HorizontalAlignment.Center }
};

var btn = new Button { Content = "로그인", Template = new ControlTemplate(typeof(Button)) };
btn.Template.VisualTree = bd; // 실제로는 프레임워크가 프레젠터를 주입
```

**WPF 동작 방식 요약**
- `CornerRadius`는 4방향(`TopLeft/TopRight/BottomRight/BottomLeft`)을 각각 지정 가능
- `Border`는 내부적으로 `RectangleGeometry`(라운드 직사각형)를 만들고,
  `Background` 브러시로 채우고, `BorderBrush`로 스트로크한다
- `TemplateBinding` = 템플릿 속성이 버튼 속성(Background 등)을 가리킴 → 상태 트리거만으로
  배경/테두리 교체 가능. 하모니의 `ThemeBrush(controlType + state)`와 역할이 같다

### 1.3 WPF 코드 예시 2 — SolidColorBrush / LinearGradientBrush

```xml
<!-- 다중 스톱 그라데이션 (금속 질감) -->
<LinearGradientBrush x:Key="GoldGradient" StartPoint="0,0" EndPoint="0,1">
    <GradientStop Color="#FFD8C078" Offset="0.00"/>
    <GradientStop Color="#FFA98A4A" Offset="0.45"/>
    <GradientStop Color="#FF675128" Offset="1.00"/>
</LinearGradientBrush>

<!-- 단색 -->
<SolidColorBrush x:Key="PanelBg" Color="#FF1A1F1D"/>
```

```csharp
// 코드에서 만들기
var brush = new LinearGradientBrush
{
    StartPoint = new Point(0, 0), // 위 → 아래 (1,0)이면 좌 → 우
    EndPoint = new Point(0, 1)
};
brush.GradientStops.Add(new GradientStop(Color.FromRgb(0xD8, 0xC0, 0x78), 0.0));
brush.GradientStops.Add(new GradientStop(Color.FromRgb(0x67, 0x51, 0x28), 1.0));
button.Background = brush;
```

- **GradientStop = offset + color**의 리스트. 2개든 5개든 자유
- `StartPoint/EndPoint`로 방향(각도) 지정, 좌표는 0~1 상대값
- WPF는 원형/방사형(`RadialGradientBrush`)도 지원
- 모든 Brush는 `Freezable`이라 여러 요소가 같은 인스턴스를 공유해도 안전

### 1.4 WPF 코드 예시 3 — BorderBrush / BorderThickness / Border 컨트롤

```xml
<Border Background="{StaticResource PanelBg}"
        BorderBrush="#585B63"
        BorderThickness="1,0,1,2"       <!-- left,top,right,bottom — 변마다 다름 -->
        CornerRadius="12,12,0,0"        <!-- 위만 둥글게 (탭 헤더 스타일) -->
        Padding="10">
    <TextBlock Text="패널 내용" Foreground="#F2F0EA"/>
</Border>
```

- `BorderThickness`는 `Thickness` 구조체 → **변마다 두께가 다를 수 있음**
- `BorderBrush`는 Brush — 단색 외에 그라데이션 브러시도 지정 가능
- 이 조합(Background + BorderBrush + BorderThickness + CornerRadius + Padding)이
  WPF의 "테두리 있는 둥근 사각형"의 전부. 하모니의 `Border` 컨트롤이 이걸 목표로 한다

### 1.5 WPF 코드 예시 4 — Foreground 상속

```xml
<StackPanel Foreground="#F2F0EA">   <!-- 자식 전부 상속 -->
    <TextBlock Text="하나"/>
    <Button Content="둘"/>
</StackPanel>
```

- `Control.Foreground`는 **상속 속성(Inherited)** — 자식이 명시하지 않으면 부모 값이 전달됨
- 값 타입이 `Brush`라서 그라데이션 전경 텍스트도 가능
- 하모니의 `UIElement::SetForeground / GetEffectiveForeground`(UIElement.h:116)가
  이미 동일한 상속 구조를 갖고 있다 — 단지 **색상(`UIColorF`)뿐**이라는 차이만 있다

### 1.6 WPF 내부 요약 (하모니와 비교 관점)

| WPF 개념 | WPF 실제 동작 | 하모니 대응 자원 |
|---|---|---|
| `Brush` | 추상 클래스, Freezable, 런타임 채움 | `sgui::Brush` (Brush.h:27) — 3종 |
| `SolidColorBrush` | 단색 채움 | `SolidColorBrush` → `LayerColor` (BrushVisual.cpp:93) |
| `LinearGradientBrush` | 스톱 배열 + 방향 | `LinearGradientBrush` → `LayerGradient` (BrushVisual.cpp:100), 단 2스톱+수직/수평만 |
| `CornerRadius` | Border가 Geometry로 변환 | `CornerRadius` 구조체(존재하지만 미사용, UITypes.h:53) + 베이커 SDF `geometryRadius` 단일값 |
| `BorderBrush/Thickness` | Border 소유 | `geometry.borderWidth` + `metal.border` (베이크 전용, 변별 지원 안 함) / `Border` 컨트롤(4엣지 LayerColor, Border.h:48) |
| `Foreground` | Brush 상속 | `UIColorF` 상속 (UIElement.h:116) |
| `TemplateBinding` | 템플릿→속성 바인딩 | `ThemeBrush(controlType + state)` (Brush.h:63) |

---

## 2. 하모니 현재 시스템 현황 (분석)

### 2.1 파이프라인 전체 흐름

```
테마 JSON ─→ UIThemeMapper ─→ UIRuntimeTheme (토큰 집합)
                                   │
                    UIStyleResolver::Resolve(control, state, theme, override)
                                   ▼
                          UIResolvedStyle (플랫 스타일)
                                   │ styleHash
                    UITextureBakeService (디바운스 + 비동기 베이크)
                                   │
                     UITextureBaker::Bake (4x 수퍼샘플)
                     ├─ UIShapeRasterizer  : SDF 라운드 마스크 / 원 / 라인
                     ├─ UIGradientRasterizer: 표면·글로스·섀도·테두리·림
                     ├─ UIGaussianBlur
                     └─ DownsampleLanczos
                                   │
                          UITextureSet (아틀라스 + 엔트리)
                                   │
          ThemeBrush(semantic, controlType, slot) ── UIAssetKey(semantic, state, styleHash)
                                   │
                 BrushVisual (Scale9Sprite | Sprite | LayerColor-폴백)
                                   │
                 UIThemeTextureBinding (테마 리빌드 시 자동 갱신)
```

### 2.2 관련 파일 지도

| 관심사 | 파일 |
|---|---|
| 브러시 타입 계층 | `UI/Core/Brush.h` — SolidColor / LinearGradient / Theme |
| 브러시 렌더링 래퍼 | `UI/Core/BrushVisual.h/.cpp` — Scale9 / Fixed / FlatFallback |
| 컨트롤 베이스 | `UI/Controls/Control.h` — Background 브러시 + Padding + VisualState |
| Border 컨트롤 | `UI/Controls/Border.h/.cpp` — Background + 4엣지 테두리 |
| 테마 토큰 | `UI/Theme/UIRuntimeTheme.h` — surface/metal/gloss/geometry/... |
| 해석 결과 | `UI/Theme/UIStyleOverride.h` — `UIResolvedStyle` + `UIStyleOverride` |
| 해석기 | `UI/Theme/UIStyleResolver.cpp` |
| 베이크 서비스 | `UI/Theme/UITextureBakeService.h/.cpp` |
| 도형 래스터라이저 | `UI/Theme/Baker/UIShapeRasterizer.cpp` — SDF 라운드 사각형 |
| 그라데이션 래스터라이저 | `UI/Theme/Baker/UIGradientRasterizer.cpp` — 표면/테두리/글로스/섀도 |
| 레시피(크기·인셋) | `UI/Theme/Baker/UIControlBakeRecipes.cpp` |
| 베이커 진입점 | `UI/Theme/Baker/UITextureBaker.cpp` — Bake / 다운샘플 |
| 캐시 키 | `UI/Theme/UIThemeTypes.h` — `UITextureCacheKey` |
| 기본 테마 값 | `UI/Theme/UIRuntimeTheme.cpp` — `EngineDefaults()` |
| WPF 스타일 공용 타입 | `UI/Core/UITypes.h` — `Thickness` / `CornerRadius`(미사용) |

### 2.3 현재 구현의 특징과 한계

**이미 갖고 있는 것 (WPF에 준함)**
- `Background`가 `BrushPtr`라서 SolidColor/LinearGradient/Theme 세 종류를 바꿔 끼울 수 있음 (BrushVisual.cpp:91)
- 테마 = 토큰 집합, 상태별 스타일 해석(`UIStyleResolver`), 스파스 오버라이드(`UIStyleOverride`+mask)
- 스타일 해시(`UIResolvedStyle::ComputeHash`) 기반 캐시 — 수정 시 자동 무효화
- DPI 스케일(`dpiScaleQ8`), 4x 수퍼샘플 + Lanczos 다운샘플 (고품질 엣지)
- 작은 컨트롤 폴백: 9-slice 인셋보다 작으면 평면 색으로 대체 (BrushVisual.cpp:120)

**한계 (이 문서의 개선 대상)**
1. **모서리가 4방향 중 하나뿐** — `geometry.radius` 단일 float (UIRuntimeTheme.h:59).
   `CornerRadius` 구조체는 UITypes.h:53에 있는데 베이크에 안 쓰임
2. **sliceInsets가 하드코딩** — ButtonRecipe 인셋 8, FrameRecipe 10 (UIControlBakeRecipes.cpp:11,22).
   테마에서 radius를 12로 바꿔도 인셋이 안 따라가서 **모서리가 9-slice로 잘려나갈 수 있음**
3. **테두리가 단일 두께/단일 색** — `geometryBorderWidth` + `metal.border`.
   WPF의 `BorderThickness(left,top,right,bottom)` 불가능. `borderGradient`는 대각선 3색 램프뿐
4. **그라데이션이 2스톱** — `UISurfaceTokens.normalTop/normalBottom` 2색 + `RenderSurface`의 세로 선형.
   스톱 배열/방향/라디얼 없음. 런타임 `LinearGradientBrush`도 start/end 2색뿐 (Brush.h:51)
5. **Foreground가 색상 전용** — Brush 상속 불가 → 그라데이션 텍스트 불가
6. **FlatFallback이 무지 둥근 사각형이 아님** — 9-slice보다 작은 컨트롤은
   각진 `LayerColor` 사각형이 됨 (BrushVisual.cpp:125) → WPF라면 CornerRadius가 보장됨

---

## 3. 접목 설계 — WPF 모델을 하모니에 이식하는 방법

### 3.1 [핵심] CornerRadius 4방향 지원

WPF의 `Border.CornerRadius`와 동일하게 **모서리마다 반지름**을 가지게 한다.

**설계**

```cpp
// UITypes.h — 이미 존재하는 구조체를 본격 사용
struct CornerRadius
{
    float topLeft_     = 0.0f;
    float topRight_    = 0.0f;
    float bottomRight_ = 0.0f;
    float bottomLeft_  = 0.0f;
    // 생성자/연산자 유지, 여기에 IsUniform() / Max() 헬퍼 추가
    float MaxRadius() const { return jc::Math::Max(jc::Math::Max(topLeft_, topRight_),
                                                   jc::Math::Max(bottomRight_, bottomLeft_)); }
};
```

```cpp
// UIStyleOverride.h — UIResolvedStyle에 반영 (기존 float geometryRadius 대체)
struct UIResolvedStyle
{
    // ...
    CornerRadius geometryCornerRadius;   // 네 모서리 (WPF Border.CornerRadius)
    float geometryRadius = 8.0f;         // 구버전 호환용 or Max()로 위임
    // ...
};
```

**SDF 확장** (UIShapeRasterizer.cpp) — 현재 `RoundedRectDistance`는 단일 radius.
per-corner 공식은 모서리 사분면에 따라 radius를 선택하는 변형이다:

```cpp
float UIShapeRasterizer::RoundedRectDistance4(
    float _px, float _py, const UIRect& _rect,
    float _rTL, float _rTR, float _rBR, float _rBL)
{
    // _rect 좌표계 규약: UIRect.Bottom() == _rect.y (y는 위로 증가) — 베이커 규약 그대로
    float cx = _rect.x + _rect.w * 0.5f;
    float cy = _rect.y + _rect.h * 0.5f;

    float qx = fabsf(_px - cx) - _rect.w * 0.5f;
    float qy = fabsf(_py - cy) - _rect.h * 0.5f;

    // 픽셀이 어느 코너의 사분면에 있는지에 따라 radius 선택
    float rad = ( _px >= cx )
              ? ( _py >= cy ? _rTR : _rBR )   // y-up이므로 부호 뒤집기 필요하면 규약에 맞춰 매핑
              : ( _py >= cy ? _rTL : _rBL );

    float outsideX = jc::Math::Max(qx + rad, 0.0f);
    float outsideY = jc::Math::Max(qy + rad, 0.0f);
    return jc::Math::Min(jc::Math::Max(qx + rad, qy + rad), 0.0f)
         + sqrtf(outsideX * outsideX + outsideY * outsideY) - rad;
}
```

> 실제 모서리↔사분면 매핑은 `UIRect`의 y-up 규약(UIPixelBuffer.h:38 `Top()=y+h`)을
> 확인하고 TL/TR/BL/BR 순서를 확정해야 한다. 텍스처 좌표가 위아래로 뒤집혀 보일 수 있으므로
> **변환 테스트**(정사각형 radius 하나만 0 → 한쪽 코너만 각짐)를 반드시 포함할 것.

이 함수 하나를 `RasterizeRoundedMask` / `RenderSurface` / `RenderBorder` / `RenderShadow` /
`RenderInnerRim`의 공통 거리 계산으로 교체하면 **모든 베이크 패스가 자동으로 4방향 모서리를 따른다**.

**slice 인셋 자동화** — WPF는 인셋 개념이 없지만 하모니는 9-slice이므로 모서리가 인셋 안에
들어가야 한다. 레시피의 하드코딩 인셋을 radius에서 계산으로 바꾼다:

```cpp
// UITextureBaker.cpp 또는 UIControlBakeRecipes.cpp 헬퍼
static UISliceInsets SliceInsetsFromCornerRadius(const CornerRadius& _r)
{
    return
    {
        jc::Math::Max(_r.topLeft_, _r.bottomLeft_),      // left  = 좌측 두 코너 중 큰 값
        jc::Math::Max(_r.topLeft_, _r.topRight_),        // top
        jc::Math::Max(_r.topRight_, _r.bottomRight_),    // right
        jc::Math::Max(_r.bottomLeft_, _r.bottomRight_)   // bottom
    };
}
```

- `minimumSize` = `{ left+right, top+bottom }`로 파생 (현재 ButtonRecipe의 16px과 동일 규칙)
- 수퍼샘플 배율(`ss`) 곱셈: `UITextureBaker::Bake`의 `scaled.geometryRadius *= ss` 지점에
  `scaled.geometryCornerRadius` 각 항목 `*= ss` 추가 (UITextureBaker.cpp:187)

**해시/캐시 영향** — `UIResolvedStyle::ComputeHash`에 코너 배열을 포함하면
`styleHash → UITextureCacheKey`가 자동으로 달라져 캐시가 정확히 무효화된다. 별도 조치 불필요.
단, 포맷 파괴이므로 `UITextureCacheKey.bakerVersion`을 1 올려 이전 캐시와 혼재를 방지한다.

**테마 JSON/스튜디오** — `UIStyleToken`에 `GeometryRadiusTL/TR/BR/BL` 4토큰 추가하고
`UIThemePropertyDescriptor` 테이블에 등록 → Theme Studio 인스펙터에서 각 모서리 편집 가능.
호환성: 기존 `radius` 필드는 4개에 일괄 적용되게 파싱한다.

### 3.2 BorderBrush / BorderThickness — "컨트롤 소유" 테두리

WPF는 Border가 소유하지만 하모니는 **Control 단위로** `BorderBrush/BorderThickness`를
제공하는 것이 접목 방향 (WPF의 `Control`은 둘이 없고, 하모니 `Control`은 템플릿이
별도로 없어서 컨트롤이 직접 가진다).

**A. 베이크 경로 (테마 테두리 — 지금 그대로 발전)**

```cpp
// UIRuntimeTheme.h — geometry 토큰 확장
struct UIGeometryTokens
{
    CornerRadius cornerRadius;    // 4방향 (기존 radius 대체/병행)
    float borderWidth = 1.0f;     // uniform용으로 유지 (인셋 = slice이므로 스트로크는 uniform OK)
    bool  borderGradient = false; // 대각선 금속 램프 (기존)
    // ...
};
```

- **비대칭(BorderThickness)은 9-slice 베이크로 해결**: 왼쪽 띠(left 두께)만 채우는
  스트로크 패스를 그리면 인셋 9분할 스트레칭이 자동으로 변별 두께를 보존한다.
  구현은 `RenderBorder`에 `UISliceInsets borderThickness`를 받는 변형 패스 추가 —
  각 픽셀에서 (x < left || x >= w-right || y < top || y >= h-bottom)이면 테두리 색.
  코너는 사각형 영역이라 라운드 코너와 자연스럽게 합성된다 (권장: Phase 2)
- **간단 버전 (Phase 1에서 가능)**: uniform만 베이크, 비대칭은 3.2-B의 동적 엣지로

**B. 런타임 경로 (로컬 오버라이드 — WPF `Border` 스타일)**

```cpp
// Control.h — WPF Border가 가진 속성과 동일 시그니처
class Control : public UIElement
{
    void SetBorderBrush(const UIColorF& _color);       // 색 or BrushPtr로 확장 가능
    void SetBorderThickness(const Thickness& _t);      // 변마다 다름 — Thickness 그대로
    void SetCornerRadius(const CornerRadius& _r);      // 런타임 라운드 (아래 3.6 참조)
    // ...
};
```

구현은 이미 존재하는 `Border` 컨트롤의 패턴(Border.h:48 `cc::LayerColor* pEdges_[4]`)을
`Control` 안으로 가져와 `BrushVisual` 옆에 놓는다. 4변은 각각 `LayerColor`로 그려
`Thickness(left,top,right,bottom)`을 그대로 표현한다.
(기존 `Border.h`의 `SyncBorderEdges()`가 이 구현의 원형 — 그대로 이식/정리하면 됨)

**C. 테마↔로컬 우선순위 (WPF TemplateBinding 순서와 일치)**

```
UIElement의 로컬 속성 (BorderBrush/Thickness/CornerRadius) 설정됨?
  ├─ 아니오 → ThemeBrush (테마의 metal.border + geometry.borderWidth + radius)  [베이크]
  └─ 예     → 동적 Border 엣지 + 동적 라운드 (3.6)                            [런타임]
```

### 3.3 Foreground → Brush (상속 유지)

```cpp
// UIElement.h — 기존 UIColorF 상속 구조에 Brush 상속 추가
class UIElement : public cc::Node
{
    void SetForeground(const UIColorF& _color);      // 기존 유지 (SolidColorBrush로 래핑)
    void SetForegroundBrush(const BrushPtr& _brush); // 신규 — 그라데이션 전경
    BrushPtr GetEffectiveForegroundBrush() const;    // 부모 체인 따라 상속
    // hasForeground_ 대신 foregroundBrush_ != null 판정, 색상은 특수 케이스로 최적화
};
```

- `TextBlock::RefreshThemeVisuals`가 전경 브러시를 확인:
  - SolidColor → 기존 `Label::setColor` (변화 없음)
  - LinearGradient → **텍스트 그라데이션 두 가지 방법**:
    1. **셰이더**: Label 전용 커스텀 GL 셰이더에 그라데이션 uniform(스톱 배열)을 넘겨
       픽셀 단위로 색 보간 (권장 — 런타임 비용 0, 스톱 개수 무제한)
    2. **텍스트 베이크**: 글리프를 픽셀 버퍼로 래스터화 후 그라데이션으로 채워 텍스처화
       (이미 `UITextureBaker` 인프라가 있으므로 2차 단계로 가능)
- 상속 체인은 기존 `GetEffectiveForeground()` 로직(부모 방문)을 Brush 타입으로 일반화
- 참고: WPF의 `TextElement.Foreground`도 Brush이며 같은 상속 구조 — 하모니는 색상만
  브러시로 올리는 것

### 3.4 그라데이션 일반화 — GradientStop 리스트 + 방향 + 라디얼

WPF는 스톱 리스트 + StartPoint/EndPoint(0~1 상대좌표)다. 하모니는 지금 표면이
2스톱 고정 세로 방향이다. 접목:

```cpp
// UIStyleOverride.h — UIResolvedStyle에 그라데이션 설명 추가
constexpr int kMaxGradientStops = 8;

struct UIGradientStop { float offset = 0.0f; UIColorF color; };

struct UIGradientDesc
{
    UIGradientStop stops[kMaxGradientStops];
    int            stopCount = 0;
    float          angleDeg = 90.0f;   // 90=위→아래 (WPF StartPoint(0,0)→EndPoint(0,1))
    bool           radial = false;
    // ComputeHash에 포함 → 캐시 자동 무효화
};
```

```cpp
// UIGradientRasterizer.cpp — RenderSurface의 2색 보간을 스톱 룩업으로 교체
static UIColorF SampleGradient(const UIGradientDesc& _g, float _t)
{
    for (int i = 1; i < _g.stopCount; ++i)
        if (_t <= _g.stops[i].offset)
        {
            float local = (_t - _g.stops[i-1].offset)
                        / jc::Math::Max(1e-6f, _g.stops[i].offset - _g.stops[i-1].offset);
            return UIColorMath::Lerp(_g.stops[i-1].color, _g.stops[i].color,
                                     UIColorMath::SmoothStep01(local));
        }
    return _g.stops[_g.stopCount - 1].color;
}
```

- **방향**: 픽셀의 t 계산을 `angleDeg`로 회전된 축으로 치환
  (cos/sin 벡터 내적 — WPF의 StartPoint/EndPoint와 동일 효과)
- **라디얼**: `dist / maxDist`를 t로 → `RadialGradientBrush` 대응
- 런타임 `LinearGradientBrush`(Brush.h:51)는 `UIGradientDesc`를 내부에 갖도록
  확장하고, `BrushVisual`의 `LayerGradient` 생성부는 방향만 유지 (코코스 한계),
  멀티스톱은 런타임 브러시 베이크(3.6) 또는 셰이더로
- 표면 토큰(`normalTop/normalBottom`)은 `stops[2]`로 자동 변환 — **기존 테마 JSON이
  그대로 동작**하는 하위호환 규칙을 정할 것

### 3.5 ThemeBrush ↔ WPF TemplateBinding 정리

지금 `ThemeBrush(controlType, state)`가 사실상 `TemplateBinding + Trigger`의 역할을
하고 있다. 접목 후 매핑을 문서화:

| WPF | 하모니 접목 후 |
|---|---|
| `Button.Template` (ControlTemplate) | `UIAssetSemantic::Button` 레시피 (UIControlBakeRecipes) |
| `TemplateBinding Background` | `ThemeBrush(Button, state→surfaceTop/Bottom)` |
| `Trigger IsMouseOver → Background` | `UIStyleResolver::ApplySurfaceToken(state)` |
| `Trigger IsPressed → Background` | 동일 (Pressed 분기) |
| `Border.CornerRadius` | `geometry.cornerRadius` 4방향 |
| `Border.BorderBrush/Thickness` | `metal.border` + `geometry.borderWidth` (베이크) / Control 오버라이드 (런타임) |

즉, **"모서리는 템플릿(Border)의 속성"**이라는 WPF 사상과
**"모서리는 테마 토큰"**이라는 하모니 사상이 충돌하지 않도록,
`UIResolvedStyle.geometryCornerRadius`가 두 진입점(테마 토큰 + 로컬 오버라이드)을
모두 받는 단일 창구가 되게 한다.

### 3.6 DynamicBrush — "베이크 없는 런타임 채움" 경로 (FlatFallback 승격)

WPF는 모든 것을 런타임에 그리므로 이 개념이 없지만, 하모니는 베이크 지연(디바운스
0.15s)과 작은 컨트롤 폴백 문제 때문에 런타임 채움이 필요하다. **FlatFallback을
"런타임 둥근 사각형 + 테두리"로 승격**하자:

- 현재: 9-slice보다 작으면 `LayerColor` 각진 사각형 (BrushVisual.cpp:125)
- 목표: 크기가 작아도 `CornerRadius`가 유지된 둥근 사각형

구현 옵션 (복잡도순):
1. **cc::DrawNode 라운드 사각형**: `drawSolidRect`/둥근 패스 구현 — 충분한가:
   DrawNode는 컬러 채움만 가능, 그라데이션/글로스 불가. **Phase 1**: 베이크 대기 중
   폴백으로 충분
2. **런타임 미니 베이크 (권장)**: `UITextureBaker::Bake`를 재사용해 해당 컨트롤
   크기로 즉석 베이크하고 크기+스타일해시 키로 캐시. 9-slice 없이 원본 스프라이트로
   깔기. "베이크 서비스 결과가 오기 전의 임시 렌더"와 "인셋보다 작은 크기" 모두 해결
3. (중장기) SDF 셰이더: 쿼드 + 둥근거리 uniform으로 GPU에서 커버리지 — WPF와
   사실상 동일 아키텍처. `BrushVisual`이 Texture2D 대신 커스텀 셰이더를 쓰는 분기

**권장 로드맵은 1 → 2 → 3** 순서. 3번까지 가면 "베이크"가 필요 없는 요소는
WPF와 동일하게 실시간 처리되므로 스튜디오 미리보기 성능도 좋아진다.

---

## 4. 단계별 적용 로드맵

### Phase 1 — 모서리 + 인셋 정합 (코어, 변경 범위 작음)

- [ ] `UITypes.h::CornerRadius`에 `MaxRadius()/IsUniform()` 헬퍼 추가
- [ ] `UIStyleOverride.h::UIResolvedStyle`에 `geometryCornerRadius` 추가,
      `UIStyleToken` 4토큰(GeometryRadiusTL/TR/BR/BL) 추가
- [ ] `UIShapeRasterizer::RoundedRectDistance4` 구현 → `RasterizeRoundedMask`/
      `RenderSurface`/`RenderBorder`/`RenderShadow`/`RenderInnerRim` 교체
- [ ] `UITextureBaker::Bake` 수퍼샘플 스케일 보정 (UITextureBaker.cpp:187)
- [ ] `SliceInsetsFromCornerRadius()`로 레시피 인셋 자동화 (UIControlBakeRecipes.cpp)
- [ ] `UIStyleResolver::Resolve`에서 radius 파싱 (기존 `radius` → 4방향 일괄)
- [ ] `bakerVersion` 1 증가, **정사각형 코너 변환 테스트**(한 코너만 0인 경우 검증)
- [ ] 브러시 스튜디오(UIThemePropertyDescriptor)에 4토큰 등록

검증: `sgcl` 빌드 후 테마 스튜디오에서 radius 4개를 다르게 바꿔
한쪽만 각진 버튼/프레임이 9-slice로 깨지지 않는지 확인

### Phase 2 — BorderBrush / BorderThickness

- [ ] `Control`에 `SetBorderBrush/SetBorderThickness/SetCornerRadius` 추가
      (Border.h의 4엣지 패턴 이식)
- [ ] `RenderBorder`에 비대칭 스트로크 패스(`UISliceInsets borderThickness`) 추가
- [ ] `BrushVisual`의 테마 경로가 로컬 오버라이드 존재 시 3.2-C 우선순위 적용
- [ ] 작은 컨트롤 FlatFallback → 런타임 둥근 사각형(DrawNode)으로 대체 (3.6-1)

### Phase 3 — 그라데이션 일반화

- [ ] `UIGradientDesc` + `SampleGradient` (3.4)
- [ ] `RenderSurface`/`RenderBorder(borderGradient)`를 스톱 기반으로 교체
- [ ] 표면 토큰 2색 → 2스톱 자동 변환 (기존 테마 JSON 하위호환)
- [ ] 런타임 `LinearGradientBrush` 확장 (스톱 + 방향) → 런타임 미니 베이크와 연결
- [ ] 라디얼(선택) — `RadialGradientBrush` 대응
- [ ] `UIGlossTokens`와 분리 정리 (글로스는 여전히 별도 오버레이)

### Phase 4 — Foreground Brush + 텍스트 그라데이션

- [ ] `UIElement::SetForegroundBrush/GetEffectiveForegroundBrush` (상속 유지)
- [ ] `TextBlock::RefreshThemeVisuals`에서 전경 브러시 분기
- [ ] 그라데이션 텍스트: Label 셰이더(권장) 또는 텍스트 베이크
- [ ] (선택) `BrushVisual`에 SDF 셰이더 경로 — 베이크 없는 실시간 채움

---

## 5. WPF ↔ 하모니 최종 대응표

| WPF 속성/개념 | 하모니 현재 | 접목 후 |
|---|---|---|
| `Border.CornerRadius` | `geometry.radius` 단일 | `geometryCornerRadius` 4방향 (SDF per-corner) |
| 9-slice 인셋 | 레시피 하드코딩 (8/10px) | radius에서 파생 (모서리 보존 보장) |
| `Background` (Brush) | `Control::SetBackground(BrushPtr)` ✅ | 유지 |
| `LinearGradientBrush` | 2스톱 고정 세로 (표면/런타임) | 스톱 리스트 + 방향 + 라디얼 |
| `SolidColorBrush` | `SolidColorBrush` → LayerColor ✅ | 유지 |
| `BorderBrush` | `metal.border` (베이크 전용) | Control 오버라이드 + 비대칭 스트로크 |
| `BorderThickness` | `geometry.borderWidth` 단일 | `Thickness` (변별) — 9-slice로 보존 |
| `Foreground` (Brush) | `UIColorF` 상속 | BrushPtr 상속 + 텍스트 그라데이션 |
| `ControlTemplate` + Trigger | `ThemeBrush(controlType, state)` ✅ | 유지 (3.5 매핑 문서화) |
| 런타임 벡터 채움 | 베이크 + Scale9 | FlatFallback → 런타임 라운드/미니 베이크 → SDF 셰이더 |

---

## 6. 결론 — 권장 방향

1. **가장 큰 효율**: `CornerRadius` 4방향 + slice 인셋 자동화 (Phase 1).
   베이커 SDF 하나만 확장하면 표면/테두리/섀도/림이 전부 따라온다.
   인셋 자동화는 기존 "radius와 9-slice 인셋 불일치로 모서리 잘림" 버그까지 해결한다.
2. **WPF의 사상 중 하모니에 맞는 것은 "속성 계층(Background/Foreground/Border가
   브러시 타입)"** — 이미 절반은 되어 있다. 남은 것은 Foreground의 브러시화와
   Control 소유 테두리다.
3. **베이크 모델은 유지하되, 단순 채움(단색/라운드/그라데이션)은 런타임 경로로
   승격**한다. WPF가 "안 베이크하는" 이유는 유연성이고, 하모니의 베이크가 강한 이유는
   글로스/섀도/금속 램프 같은 고비용 효과다. 둘을 나누는 경계가 `DynamicBrush`다.
4. 캐시/해시는 전부 `ComputeHash` 기반이라 새 필드만 추가하면 무효화가 자동으로
   따라오므로 **형식 안전**하다. 캐시 포맷이 바뀌는 시점만 `bakerVersion`을 올리자.
