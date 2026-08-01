#pragma once

#include "sgcl/Game/UI/Theme/Baker/UIPixelBuffer.h"
#include "sgcl/Game/UI/Theme/UIThemeTypes.h"

class UIControlBakeRecipes
{
public:
    static UIAssetRecipe ButtonRecipe(int _width, int _height);
    static UIAssetRecipe FrameRecipe(int _width, int _height);
    static UIAssetRecipe CheckBoxShellRecipe(int _size);
    static UIAssetRecipe CheckMarkRecipe(int _size);
    static UIAssetRecipe RadioShellRecipe(int _size);
    static UIAssetRecipe RadioDotRecipe(int _size);
    static UIAssetRecipe ToggleTrackRecipe(int _width, int _height);
    static UIAssetRecipe ToggleKnobRecipe(int _size);
    static UIAssetRecipe ProgressTrackRecipe(int _width, int _height);
    static UIAssetRecipe ProgressGaugeRecipe(int _width, int _height);
    static UIAssetRecipe ProgressCapRecipe(int _size);
    static UIAssetRecipe SliderTrackRecipe(int _width, int _height);
    static UIAssetRecipe SliderFillRecipe(int _width, int _height);
    static UIAssetRecipe SliderThumbRecipe(int _size);
    static UIAssetRecipe ScrollBarTrackRecipe(int _width, int _height);
};
