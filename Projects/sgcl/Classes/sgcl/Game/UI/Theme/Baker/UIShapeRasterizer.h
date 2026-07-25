#pragma once

#include "sgcl/Game/UI/Theme/Baker/UIPixelBuffer.h"
#include "sgcl/Game/UI/Theme/Baker/UIMaskBuffer.h"

namespace UIShapeRasterizer
{
    float RoundedRectDistance(float _px, float _py, const UIRect& _rect, float _radius);

    void RasterizeRoundedMask(
        UIMaskBuffer& _out,
        const UIRect& _rect,
        float _radius,
        float _fillAlpha = 1.0f);

    void RasterizeCircleMask(
        UIMaskBuffer& _out,
        float _cx, float _cy, float _radius);

    void RasterizeRoundLine(
        UIPixelBuffer& _out,
        const cc::vec2& _p0, const cc::vec2& _p1,
        float _lineWidth, const UIColorF& _color);
}
