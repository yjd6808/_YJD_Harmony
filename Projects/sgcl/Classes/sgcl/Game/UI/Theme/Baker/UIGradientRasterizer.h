#pragma once

#include "sgcl/Game/UI/Theme/Baker/UIPixelBuffer.h"
#include "sgcl/Game/UI/Theme/Baker/UIMaskBuffer.h"
#include "sgcl/Game/UI/Theme/UIStyleOverride.h"

namespace UIGradientRasterizer
{
    void RenderSurface(
        UIPixelBuffer& _out,
        const UIRect& _rect,
        float _radius,
        const UIColorF& _top,
        const UIColorF& _bottom,
        bool _reverse = false);

    void RenderSoftGloss(
        UIPixelBuffer& _out,
        const UIRect& _rect,
        const UIResolvedStyle& _style,
        const UIMaskBuffer& _shapeMask);

    void RenderShadow(
        UIPixelBuffer& _out,
        const UIRect& _rect,
        float _radius,
        const UIResolvedStyle& _style);

    void RenderBorder(
        UIPixelBuffer& _out,
        const UIRect& _rect,
        const UIResolvedStyle& _style);

    void RenderInnerRim(
        UIPixelBuffer& _out,
        const UIRect& _rect,
        const UIResolvedStyle& _style);
}
