#include "GameCoreHeader.h"
#include "sgcl/Game/UI/Theme/Baker/UIGradientRasterizer.h"
#include "sgcl/Game/UI/Theme/Baker/UIShapeRasterizer.h"
#include "sgcl/Game/UI/Theme/Baker/UIGaussianBlur.h"
#include "sgcl/Game/UI/Theme/Baker/UIColorMath.h"

namespace UIGradientRasterizer
{
    void RenderSurface(
        UIPixelBuffer& _out,
        const UIRect& _rect,
        float _radius,
        const UIColorF& _top,
        const UIColorF& _bottom,
        bool _reverse)
    {
        int minY = (int)floorf(_rect.y);
        int maxY = (int)ceilf(_rect.Top());

        for (int y = minY; y <= maxY; ++y)
        {
            float t = (y + 0.5f - _rect.y) / jc::Math::Max(1.0f, _rect.h);
            t = UIColorMath::SmoothStep01(t);
            if (_reverse) t = 1.0f - t;

            UIColorF rowColor = UIColorMath::Lerp(_top, _bottom, t);

            int minX = (int)floorf(_rect.x);
            int maxX = (int)ceilf(_rect.Right());

            for (int x = minX; x <= maxX; ++x)
            {
                if (x < 0 || x >= _out.width || y < 0 || y >= _out.height)
                    continue;

                float d = UIShapeRasterizer::RoundedRectDistance(
                    x + 0.5f, y + 0.5f, _rect, _radius);
                float coverage = UIColorMath::CoverageFromDistance(d);
                _out.BlendOver(x, y, rowColor.WithAlpha(rowColor.a * coverage));
            }
        }
    }

    void RenderSoftGloss(
        UIPixelBuffer& _out,
        const UIRect& _rect,
        const UIResolvedStyle& _style,
        const UIMaskBuffer& _shapeMask)
    {
        float centerX = _rect.x + _rect.w * _style.glossTint.r;
        if (centerX < _rect.x) centerX = _rect.x + _rect.w * 0.5f;

        float centerY = _rect.y + _rect.h * 0.02f;
        float radiusX = jc::Math::Max(1.0f, _rect.w * 0.64f);
        float radiusY = jc::Math::Max(1.0f, _rect.h * 1.50f);
        float maxY = _rect.y + _rect.h * UIColorMath::Clamp01(_style.glossTint.g > 0 ? _style.glossTint.g : 0.55f);

        UIMaskBuffer glossMask(_out.width, _out.height);

        for (int y = (int)floorf(_rect.y); y < (int)ceilf(maxY); ++y)
        {
            for (int x = (int)floorf(_rect.x); x < (int)ceilf(_rect.Right()); ++x)
            {
                if (x < 0 || x >= _out.width || y < 0 || y >= _out.height)
                    continue;

                float dx = (x + 0.5f - centerX) / radiusX;
                float dy = (y + 0.5f - centerY) / radiusY;
                float dist2 = dx * dx + dy * dy;
                float intensity = jc::Math::Max(0.0f, 1.0f - dist2);

                glossMask(x, y) = intensity * _style.glossAlpha * _shapeMask(x, y);
            }
        }

        UIGaussianBlur::Blur(glossMask, _style.glossTint.b > 0 ? _style.glossTint.b * 10.0f : 3.0f);

        for (int y = 0; y < _out.height; ++y)
            for (int x = 0; x < _out.width; ++x)
                if (glossMask(x, y) > 0.01f)
                    _out.BlendOver(x, y, UIColorF(1, 1, 1, glossMask(x, y) * _shapeMask(x, y)));
    }

    void RenderShadow(
        UIPixelBuffer& _out,
        const UIRect& _rect,
        float _radius,
        const UIResolvedStyle& _style)
    {
        UIMaskBuffer shadow(_out.width, _out.height);
        UIRect shadowRect = _rect.Offset(0, _style.geometryDepth);
        UIShapeRasterizer::RasterizeRoundedMask(shadow, shadowRect, _radius, 0.58f);

        UIGaussianBlur::Blur(shadow, _style.geometryShadow * 2.0f);

        for (int y = 0; y < _out.height; ++y)
        {
            for (int x = 0; x < _out.width; ++x)
            {
                float alpha = shadow(x, y) * 0.48f;
                if (alpha > 0.01f)
                    _out.BlendOver(x, y, UIColorF(0, 0, 0, alpha));
            }
        }
    }

    static UIColorF ResolveMetalColor(float _q, const UIResolvedStyle& _style)
    {
        if (_q <= 0.32f)
            return UIColorMath::Lerp(_style.metalHighlight, _style.metalBorder, _q / 0.32f);

        return UIColorMath::Lerp(
            _style.metalBorder,
            _style.metalShadow,
            (_q - 0.32f) / 0.68f);
    }

    void RenderBorder(
        UIPixelBuffer& _out,
        const UIRect& _rect,
        const UIResolvedStyle& _style)
    {
        float bw = _style.geometryBorderWidth;
        if (bw <= 0.0f) return;

        for (int y = (int)floorf(_rect.y); y <= (int)ceilf(_rect.Top()); ++y)
        {
            for (int x = (int)floorf(_rect.x); x <= (int)ceilf(_rect.Right()); ++x)
            {
                if (x < 0 || x >= _out.width || y < 0 || y >= _out.height)
                    continue;

                float d = UIShapeRasterizer::RoundedRectDistance(
                    x + 0.5f, y + 0.5f, _rect, _style.geometryRadius);
                float outer = UIColorMath::CoverageFromDistance(d);
                float inner = UIColorMath::CoverageFromDistance(
                    d + _style.geometryBorderWidth);
                float borderMask = UIColorMath::Clamp01(outer - inner);

                if (borderMask <= 0.01f) continue;

                float q = ((x - _rect.x) + (y - _rect.y))
                    / jc::Math::Max(1.0f, _rect.w + _rect.h);
                UIColorF color = ResolveMetalColor(q, _style);
                _out.BlendOver(x, y, color.WithAlpha(borderMask * 0.92f));
            }
        }
    }

    void RenderInnerRim(
        UIPixelBuffer& _out,
        const UIRect& _rect,
        const UIResolvedStyle& _style)
    {
        if (_style.geometryInnerRim <= 0.0f) return;

        for (int y = (int)floorf(_rect.y); y <= (int)ceilf(_rect.Top()); ++y)
        {
            for (int x = (int)floorf(_rect.x); x <= (int)ceilf(_rect.Right()); ++x)
            {
                if (x < 0 || x >= _out.width || y < 0 || y >= _out.height)
                    continue;

                float d = UIShapeRasterizer::RoundedRectDistance(
                    x + 0.5f, y + 0.5f, _rect, _style.geometryRadius);
                float outer = UIColorMath::CoverageFromDistance(d + _style.geometryBorderWidth);
                float inner = UIColorMath::CoverageFromDistance(
                    d + _style.geometryBorderWidth + _style.geometryInnerRim);
                float rimMask = UIColorMath::Clamp01(outer - inner);

                if (rimMask <= 0.01f) continue;

                UIColorF rimColor = UIColorMath::Lerp(
                    _style.glossTint, UIColorF(0, 0, 0), 0.82f);
                _out.BlendOver(x, y, rimColor.WithAlpha(rimMask * 0.18f));
            }
        }
    }
}
