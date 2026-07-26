#define _USE_MATH_DEFINES
#include <cmath>
#include "GameCoreHeader.h"
#include "sgcl/Game/UI/Theme/Baker/UITextureBaker.h"
#include "sgcl/Game/UI/Theme/Baker/UIShapeRasterizer.h"
#include "sgcl/Game/UI/Theme/Baker/UIGradientRasterizer.h"
#include "sgcl/Game/UI/Theme/Baker/UIColorMath.h"

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
        for (int x = 0; x < _work.width; ++x)
            if (dot(x, y) > 0.01f)
                _work.BlendOver(x, y, _style.semanticText.WithAlpha(dot(x, y)));
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

    UIRect materialBounds = CalculateMaterialBounds(
        workW, workH, scaled, true);

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
        RenderMaterial(work, materialBounds, scaled);
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
    return result;
}

UIRect UITextureBaker::CalculateMaterialBounds(
    int _workW, int _workH,
    const UIResolvedStyle& _style,
    bool _shadowEnabled)
{
    float padding = _shadowEnabled ? _style.geometryShadow * 2.0f + _style.geometryDepth + 2.0f : 0.0f;
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

                    float wx = (sx2 == 0) ? 1.0f : (sinf((float)M_PI * sx2) / ((float)M_PI * sx2))
                        * (sinf((float)M_PI * sx2 / sx) / ((float)M_PI * sx2 / sx));
                    float wy = (sy2 == 0) ? 1.0f : (sinf((float)M_PI * sy2) / ((float)M_PI * sy2))
                        * (sinf((float)M_PI * sy2 / sy) / ((float)M_PI * sy2 / sy));
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
