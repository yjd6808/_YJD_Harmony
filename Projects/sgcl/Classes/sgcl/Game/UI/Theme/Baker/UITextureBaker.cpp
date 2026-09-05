#define _USE_MATH_DEFINES
#include <cmath>
#include "GameCoreHeader.h"
#include "sgcl/Game/UI/Theme/Baker/UITextureBaker.h"
#include "sgcl/Game/UI/Theme/Baker/UIShapeRasterizer.h"
#include "sgcl/Game/UI/Theme/Baker/UIGradientRasterizer.h"
#include "sgcl/Game/UI/Theme/Baker/UIColorMath.h"
#include "jc/IO/File.h"

#define NANOSVG_IMPLEMENTATION
#define NANOSVGRAST_IMPLEMENTATION
#include "nanosvg.h"
#include "nanosvgrast.h"

static const char* SemanticName(UIAssetSemantic _semantic)
{
    switch (_semantic)
    {
    case UIAssetSemantic::Button: return "Button";
    case UIAssetSemantic::Frame: return "Frame";
    case UIAssetSemantic::CheckBox: return "CheckBox";
    case UIAssetSemantic::Radio: return "Radio";
    case UIAssetSemantic::ToggleTrack: return "ToggleTrack";
    case UIAssetSemantic::ToggleKnob: return "ToggleKnob";
    case UIAssetSemantic::CheckMark: return "CheckMark";
    case UIAssetSemantic::RadioDot: return "RadioDot";
    case UIAssetSemantic::ProgressTrack: return "ProgressTrack";
    case UIAssetSemantic::ProgressGauge: return "ProgressGauge";
    case UIAssetSemantic::SliderTrack: return "SliderTrack";
    case UIAssetSemantic::SliderFill: return "SliderFill";
    case UIAssetSemantic::SliderThumb: return "SliderThumb";
    case UIAssetSemantic::ScrollBarTrack: return "ScrollBarTrack";
    case UIAssetSemantic::ScrollBarThumb: return "ScrollBarThumb";
    case UIAssetSemantic::EditBox: return "EditBox";
    case UIAssetSemantic::FocusRing: return "FocusRing";
    case UIAssetSemantic::ProgressCap: return "ProgressCap";
    case UIAssetSemantic::WindowIconMinimize: return "WindowIconMinimize";
    case UIAssetSemantic::WindowIconMaximize: return "WindowIconMaximize";
    case UIAssetSemantic::WindowIconClose: return "WindowIconClose";
    default: return "Unknown";
    }
}

// 아이콘 SVG가 없을 때 동적으로 생성되는 느낌표 폴백 (현재 테마색으로 틴트됨)
static const char* kFallbackExclamationSvg =
    "<svg xmlns=\"http://www.w3.org/2000/svg\" viewBox=\"0 0 22 22\">"
    "<path d=\"M11 2.2c-.95 0-1.7.75-1.7 1.7v9.8c0 .95.75 1.7 1.7 1.7s1.7-.75 1.7-1.7V3.9c0-.95-.75-1.7-1.7-1.7z\" fill=\"currentColor\"/>"
    "<circle cx=\"11\" cy=\"18.6\" r=\"1.9\" fill=\"currentColor\"/>"
    "</svg>";

static void RenderMaterial(
    UIPixelBuffer& _work,
    const UIRect& _bounds,
    const UIResolvedStyle& _style)
{
    UIGradientRasterizer::RenderShadow(_work, _bounds, _style.geometryRadius, _style);

    UIMaskBuffer shapeMask(_work.width, _work.height);
    UIShapeRasterizer::RasterizeRoundedMask(shapeMask, _bounds, _style.geometryRadius);

    UIGradientRasterizer::RenderSurface(
        _work, _bounds, _style.geometryRadius,
        _style.surfaceTop, _style.surfaceBottom);

    UIGradientRasterizer::RenderSoftGloss(_work, _bounds, _style, shapeMask);
    UIGradientRasterizer::RenderBorder(_work, _bounds, _style);
    UIGradientRasterizer::RenderInnerRim(_work, _bounds, _style);
}

static void RenderGaugeFill(
    UIPixelBuffer& _work,
    const UIRect& _bounds,
    const UIResolvedStyle& _style)
{
    UIGradientRasterizer::RenderSurface(
        _work, _bounds, _style.geometryRadius,
        _style.metalBright, _style.metalHighlight);
}

static void RenderCheckMark(
    UIPixelBuffer& _work,
    const UIResolvedStyle& _style)
{
    float w = (float)_work.width;
    float h = (float)_work.height;

    cc::vec2 p0 = { w * 0.14f, h * 0.52f };
    cc::vec2 p1 = { w * 0.41f, h * 0.80f };
    cc::vec2 p2 = { w * 0.88f, h * 0.18f };
    float lineWidth = jc::Math::Max(2.0f, jc::Math::Min(w, h) * 0.14f);

    UIShapeRasterizer::RasterizeRoundLine(_work, p0, p1, lineWidth, _style.semanticText);
    UIShapeRasterizer::RasterizeRoundLine(_work, p1, p2, lineWidth, _style.semanticText);
}

static void RenderRadioDot(
    UIPixelBuffer& _work,
    const UIResolvedStyle& _style)
{
    float cx = (float)_work.width * 0.5f;
    float cy = (float)_work.height * 0.5f;
    float r = jc::Math::Min((float)_work.width, (float)_work.height) * 0.35f;

    UIMaskBuffer dot(_work.width, _work.height);
    UIShapeRasterizer::RasterizeCircleMask(dot, cx, cy, r);

    for (int y = 0; y < _work.height; ++y)
    {
        for (int x = 0; x < _work.width; ++x)
        {
            if (dot(x, y) > 0.01f)
            {
                _work.BlendOver(x, y, _style.semanticText.WithAlpha(dot(x, y)));
            }
        }
    }
}

static void RenderFrameOnly(
    UIPixelBuffer& _work,
    const UIRect& _bounds,
    const UIResolvedStyle& _style)
{
    UIGradientRasterizer::RenderShadow(_work, _bounds, _style.geometryRadius, _style);

    UIMaskBuffer shapeMask(_work.width, _work.height);
    UIShapeRasterizer::RasterizeRoundedMask(shapeMask, _bounds, _style.geometryRadius);

    UIGradientRasterizer::RenderBorder(_work, _bounds, _style);
    UIGradientRasterizer::RenderInnerRim(_work, _bounds, _style);
}

// SVG 아이콘 렌더링: 파일 로드 -> 파싱 -> 고해상도 래스터라이즈 -> 틴트(테마 semanticText) -> 다운샘플
static bool RenderSvgIcon(
    UIPixelBuffer& _work,
    const UIAssetRecipe& _recipe,
    const UIResolvedStyle& _style,
    const UIBakeOptions& _options)
{
    if (_recipe.svgPath.IsEmpty())
    {
        _LogError_("[UITextureBaker] SVG 아이콘 경로가 비어 있습니다.");
        return false;
    }

    _LogDebug_("[UITextureBaker] SVG icon render start: %s (%dx%d)", _recipe.svgPath.Source(), _recipe.width, _recipe.height);

    jc::String content;
    if (!jc::File::Exist(_recipe.svgPath.Source()))
    {
        _LogWarn_("[UITextureBaker] SVG 아이콘 파일 없음(semantic=%s) - 느낌표 폴백 사용: %s",
            SemanticName(_recipe.semantic), _recipe.svgPath.Source());
        content = kFallbackExclamationSvg;
    }
    else
    {
        content = jc::File::ReadAllText(_recipe.svgPath.Source());
        _LogDebug_("[UITextureBaker] SVG read: %s (%d bytes)", _recipe.svgPath.Source(), content.Length());
        if (content.IsEmpty())
        {
            _LogWarn_("[UITextureBaker] SVG 파일을 읽지 못했습니다(semantic=%s) - 느낌표 폴백 사용: %s",
                SemanticName(_recipe.semantic), _recipe.svgPath.Source());
            content = kFallbackExclamationSvg;
        }
    }

    NSVGimage* pImage = nsvgParse(content.Source(), "px", 96.0f);
    _LogDebug_("[UITextureBaker] SVG parsed: %p size=%.1fx%.1f", (void*)pImage, pImage ? pImage->width : 0.0f, pImage ? pImage->height : 0.0f);
    if (pImage == nullptr)
    {
        _LogError_("[UITextureBaker] SVG 파싱 실패: %s", _recipe.svgPath.Source());
        return false;
    }

    const int ss = _options.supersample;
    const int workW = _work.width;
    const int workH = _work.height;

    // SVG 원본 크기에 맞춰 작업 버퍼로 확대 래스터라이즈 (다운샘플 전 고해상도)
    const float scale = (pImage->width > 0.0f && pImage->height > 0.0f)
        ? jc::Math::Max((float)workW / pImage->width, (float)workH / pImage->height)
        : 1.0f;

    jc::Vector<unsigned char> rgba;
    rgba.Resize(workW * workH * 4, 0);

    NSVGrasterizer* pRast = nsvgCreateRasterizer();
    if (pRast == nullptr)
    {
        nsvgDelete(pImage);
        return false;
    }

    nsvgRasterize(pRast, pImage, 0.0f, 0.0f, scale, rgba.Source(), workW, workH, workW * 4);
    _LogDebug_("[UITextureBaker] SVG rasterized: %dx%d scale=%.2f", workW, workH, scale);

    const UIColorF tint = _style.semanticText;

    for (int y = 0; y < workH; ++y)
    {
        for (int x = 0; x < workW; ++x)
        {
            const int idx = (y * workW + x) * 4;
            const float alpha = rgba[idx + 3] / 255.0f;
            if (alpha <= 0.0f)
            {
                continue;
            }

            _work.Store(x, y, { tint.r, tint.g, tint.b, alpha });
        }
    }

    nsvgDeleteRasterizer(pRast);
    nsvgDelete(pImage);
    return true;
}

UIBakeOutput UITextureBaker::Bake(
    const UIAssetRecipe& _recipe,
    const UIResolvedStyle& _style,
    const UIBakeOptions& _options)
{
    int ss = _options.supersample;
    int workW = _recipe.width * ss;
    int workH = _recipe.height * ss;

    UIResolvedStyle scaled = _style;
    scaled.geometryRadius *= ss;
    scaled.geometryBorderWidth *= ss;
    scaled.geometryInnerRim *= ss;
    scaled.geometryDepth *= ss;
    scaled.geometryShadow *= ss;

    UIPixelBuffer work(workW, workH);
    work.Clear({ 0, 0, 0, 0 });

    // SVG 아이콘 레시피: 도형 레시피 대신 SVG 파싱/래스터라이즈로 렌더링한다.
    if (!_recipe.svgPath.IsEmpty())
    {
        if (!RenderSvgIcon(work, _recipe, scaled, _options))
        {
            _LogWarn_("[UITextureBaker] SVG 아이콘 렌더링 실패(semantic=%s), 도형 폴백: %s",
                SemanticName(_recipe.semantic), _recipe.svgPath.Source());
            RenderMaterial(work, CalculateMaterialBounds(workW, workH, scaled, true), scaled);
        }

        UIPixelBuffer output = DownsampleLanczos(work, _recipe.width, _recipe.height);
        ValidateAlphaEdges(output);

        UIBakeOutput result;
        result.buffer = jc::Move(output);
        result.sliceInsets = _recipe.sliceInsets;
        result.minimumSize = _recipe.minimumSize;
        return result;
    }

    UIRect materialBounds = CalculateMaterialBounds(
        workW, workH, scaled, true);

    UIRect gaugeBounds = CalculateMaterialBounds(
        workW, workH, scaled, false);

    switch (_recipe.semantic)
    {
    case UIAssetSemantic::Button:
    case UIAssetSemantic::CheckBox:
    case UIAssetSemantic::ToggleTrack:
    case UIAssetSemantic::SliderTrack:
    case UIAssetSemantic::ProgressTrack:
    case UIAssetSemantic::ScrollBarTrack:
        RenderMaterial(work, materialBounds, scaled);
        break;

    case UIAssetSemantic::Frame:
        RenderFrameOnly(work, materialBounds, scaled);
        break;

    case UIAssetSemantic::CheckMark:
        RenderCheckMark(work, scaled);
        break;

    case UIAssetSemantic::RadioDot:
        RenderRadioDot(work, scaled);
        break;

    case UIAssetSemantic::ProgressGauge:
    case UIAssetSemantic::SliderFill:
        RenderGaugeFill(work, gaugeBounds, scaled);
        break;

    default:
        RenderMaterial(work, materialBounds, scaled);
        break;
    }

    UIPixelBuffer output = DownsampleLanczos(work, _recipe.width, _recipe.height);
    ValidateAlphaEdges(output);

    UIBakeOutput result;
    result.buffer = jc::Move(output);
    result.sliceInsets = _recipe.sliceInsets;
    result.minimumSize = _recipe.minimumSize;
    result.contentPadding = materialBounds.x / (float)ss;
    return result;
}

UIRect UITextureBaker::CalculateMaterialBounds(
    int _workW, int _workH,
    const UIResolvedStyle& _style,
    bool _shadowEnabled)
{
    float padding = _shadowEnabled ? _style.geometryShadow * 2.0f + _style.geometryDepth + 2.0f : 0.0f;
    float maxPadding = std::min((float)_workW, (float)_workH) * 0.24f;
    padding = std::min(padding, maxPadding);
    return { padding, padding, (float)_workW - padding * 2, (float)_workH - padding * 2 };
}

UIPixelBuffer UITextureBaker::DownsampleLanczos(
    const UIPixelBuffer& _src,
    int _dstW, int _dstH)
{
    UIPixelBuffer dst(_dstW, _dstH);

    float sx = (float)_src.width / (float)_dstW;
    float sy = (float)_src.height / (float)_dstH;

    for (int dy = 0; dy < _dstH; ++dy)
    {
        for (int dx = 0; dx < _dstW; ++dx)
        {
            float sumR = 0, sumG = 0, sumB = 0, sumA = 0, totalW = 0;

            float cx = (dx + 0.5f) * sx;
            float cy = (dy + 0.5f) * sy;

            int rx = (int)ceilf(sx);
            int ry = (int)ceilf(sy);

            for (int sy2 = -ry; sy2 <= ry; ++sy2)
            {
                for (int sx2 = -rx; sx2 <= rx; ++sx2)
                {
                    int ix = (int)(cx + sx2);
                    int iy = (int)(cy + sy2);

                    if (ix < 0 || ix >= _src.width || iy < 0 || iy >= _src.height)
                        continue;

                    float dx = (ix + 0.5f) - cx;
                    float dy = (iy + 0.5f) - cy;
                    float adx = fabsf(dx), ady = fabsf(dy);
                    float wx = (adx < 0.0001f) ? 1.0f : (sinf((float)M_PI * dx) / ((float)M_PI * dx))
                        * (sinf((float)M_PI * dx / sx) / ((float)M_PI * dx / sx));
                    float wy = (ady < 0.0001f) ? 1.0f : (sinf((float)M_PI * dy) / ((float)M_PI * dy))
                        * (sinf((float)M_PI * dy / sy) / ((float)M_PI * dy / sy));
                    float w = wx * wy;

                    UIColorF c = _src.Load(ix, iy);
                    UIColorF linear = UIColorMath::SRGBToLinear(c);

                    sumR += linear.r * w;
                    sumG += linear.g * w;
                    sumB += linear.b * w;
                    sumA += linear.a * w;
                    totalW += w;
                }
            }

            if (totalW > 0)
            {
                UIColorF result = UIColorMath::LinearToSRGB({
                    sumR / totalW, sumG / totalW, sumB / totalW, sumA / totalW
                });
                dst.Store(dx, dy, result);
            }
        }
    }

    return dst;
}

void UITextureBaker::ValidateAlphaEdges(UIPixelBuffer& _buffer)
{
    for (int y = 0; y < _buffer.height; ++y)
    {
        for (int x = 0; x < _buffer.width; ++x)
        {
            UIColorF c = _buffer.Load(x, y);
            if (c.a > 0.01f && c.r == 0 && c.g == 0 && c.b == 0)
            {
                _buffer.Store(x, y, { 0, 0, 0, c.a });
            }
        }
    }
}
