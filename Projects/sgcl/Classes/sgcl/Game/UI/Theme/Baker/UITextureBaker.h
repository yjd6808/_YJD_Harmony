#pragma once

#include "sgcl/Game/UI/Theme/Baker/UIPixelBuffer.h"
#include "sgcl/Game/UI/Theme/UIStyleOverride.h"

class UITextureBaker
{
public:
    static UIBakeOutput Bake(
        const UIAssetRecipe& _recipe,
        const UIResolvedStyle& _style,
        const UIBakeOptions& _options);

    static UIRect CalculateMaterialBounds(
        int _workW, int _workH,
        const UIResolvedStyle& _style,
        bool _shadowEnabled);

    static UIPixelBuffer DownsampleLanczos(
        const UIPixelBuffer& _src,
        int _dstW, int _dstH);

    static void ValidateAlphaEdges(UIPixelBuffer& _buffer);
};
