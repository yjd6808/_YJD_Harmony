#include "GameCoreHeader.h"
#include "sgcl/Game/UI/Theme/Baker/UIShapeRasterizer.h"
#include "sgcl/Game/UI/Theme/Baker/UIColorMath.h"

namespace UIShapeRasterizer
{
    float RoundedRectDistance(float _px, float _py, const UIRect& _rect, float _radius)
    {
        float cx = _rect.x + _rect.w * 0.5f;
        float cy = _rect.y + _rect.h * 0.5f;
        float hx = _rect.w * 0.5f;
        float hy = _rect.h * 0.5f;

        float qx = fabsf(_px - cx) - hx + _radius;
        float qy = fabsf(_py - cy) - hy + _radius;

        float outsideX = jc::Math::Max(qx, 0.0f);
        float outsideY = jc::Math::Max(qy, 0.0f);

        return jc::Math::Min(jc::Math::Max(qx, qy), 0.0f) + sqrtf(outsideX * outsideX + outsideY * outsideY) - _radius;
    }

    void RasterizeRoundedMask(UIMaskBuffer& _out, const UIRect& _rect, float _radius, float _fillAlpha)
    {
        int minX = (int)floorf(_rect.x);
        int maxX = (int)ceilf(_rect.Right());
        int minY = (int)floorf(_rect.y);
        int maxY = (int)ceilf(_rect.Top());

        for (int y = minY; y <= maxY; ++y)
        {
            for (int x = minX; x <= maxX; ++x)
            {
                if (x < 0 || x >= _out.width || y < 0 || y >= _out.height)
                    continue;

                float d = RoundedRectDistance(x + 0.5f, y + 0.5f, _rect, _radius);
                float coverage = UIColorMath::CoverageFromDistance(d);
                _out(x, y) = coverage * _fillAlpha;
            }
        }
    }

    void RasterizeCircleMask(UIMaskBuffer& _out, float _cx, float _cy, float _radius)
    {
        int minX = (int)floorf(_cx - _radius - 1);
        int maxX = (int)ceilf(_cx + _radius + 1);
        int minY = (int)floorf(_cy - _radius - 1);
        int maxY = (int)ceilf(_cy + _radius + 1);

        for (int y = minY; y <= maxY; ++y)
        {
            for (int x = minX; x <= maxX; ++x)
            {
                if (x < 0 || x >= _out.width || y < 0 || y >= _out.height)
                    continue;

                float dx = (x + 0.5f - _cx);
                float dy = (y + 0.5f - _cy);
                float dist = sqrtf(dx * dx + dy * dy);
                float d = dist - _radius;
                float coverage = UIColorMath::CoverageFromDistance(d);
                _out(x, y) = jc::Math::Max(_out(x, y), coverage);
            }
        }
    }

    void RasterizeRoundLine(
        UIPixelBuffer& _out,
        const cc::vec2& _p0, const cc::vec2& _p1,
        float _lineWidth, const UIColorF& _color)
    {
        float halfW = _lineWidth * 0.5f;
        float dx = _p1.x - _p0.x;
        float dy = _p1.y - _p0.y;
        float len = sqrtf(dx * dx + dy * dy);
        if (len < 0.001f) return;

        float nx = -dy / len;
        float ny = dx / len;

        int minX = (int)floorf(jc::Math::Min(_p0.x, _p1.x) - halfW - 1);
        int maxX = (int)ceilf(jc::Math::Max(_p0.x, _p1.x) + halfW + 1);
        int minY = (int)floorf(jc::Math::Min(_p0.y, _p1.y) - halfW - 1);
        int maxY = (int)ceilf(jc::Math::Max(_p0.y, _p1.y) + halfW + 1);

        for (int y = minY; y <= maxY; ++y)
        {
            for (int x = minX; x <= maxX; ++x)
            {
                if (x < 0 || x >= _out.width || y < 0 || y >= _out.height)
                    continue;

                float px = x + 0.5f;
                float py = y + 0.5f;

                float ex = px - _p0.x;
                float ey = py - _p0.y;
                float t = (ex * dx + ey * dy) / (len * len);
                t = UIColorMath::Clamp01(t);

                float cx = _p0.x + t * dx;
                float cy = _p0.y + t * dy;
                float dist = sqrtf((px - cx) * (px - cx) + (py - cy) * (py - cy));
                float d = dist - halfW;
                float coverage = UIColorMath::CoverageFromDistance(d);

                if (coverage > 0.01f)
                    _out.BlendOver(x, y, _color.WithAlpha(_color.a * coverage));
            }
        }
    }
}
