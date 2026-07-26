#include "GameCoreHeader.h"
#include "sgcl/Game/UI/Theme/Baker/UIControlBakeRecipes.h"

UIAssetRecipe UIControlBakeRecipes::ButtonRecipe(int _width, int _height)
{
    UIAssetRecipe r;
    r.semantic = UIAssetSemantic::Button;
    r.width = _width;
    r.height = _height;
    r.sliceInsets = { 14, 14, 14, 14 };
    r.minimumSize = cc::Size(28, 28);
    return r;
}

UIAssetRecipe UIControlBakeRecipes::FrameRecipe(int _width, int _height)
{
    UIAssetRecipe r;
    r.semantic = UIAssetSemantic::Frame;
    r.width = _width;
    r.height = _height;
    r.sliceInsets = { 10, 10, 10, 10 };
    r.minimumSize = cc::Size(20, 20);
    return r;
}

UIAssetRecipe UIControlBakeRecipes::CheckBoxShellRecipe(int _size)
{
    UIAssetRecipe r;
    r.semantic = UIAssetSemantic::CheckBox;
    r.width = _size;
    r.height = _size;
    r.sliceInsets = { 10, 10, 10, 10 };
    r.minimumSize = cc::Size(20, 20);
    return r;
}

UIAssetRecipe UIControlBakeRecipes::CheckMarkRecipe(int _size)
{
    UIAssetRecipe r;
    r.semantic = UIAssetSemantic::CheckMark;
    r.width = _size;
    r.height = _size;
    return r;
}

UIAssetRecipe UIControlBakeRecipes::RadioShellRecipe(int _size)
{
    UIAssetRecipe r;
    r.semantic = UIAssetSemantic::Radio;
    r.width = _size;
    r.height = _size;
    r.sliceInsets = { 10, 10, 10, 10 };
    r.minimumSize = cc::Size(20, 20);
    return r;
}

UIAssetRecipe UIControlBakeRecipes::RadioDotRecipe(int _size)
{
    UIAssetRecipe r;
    r.semantic = UIAssetSemantic::RadioDot;
    r.width = _size;
    r.height = _size;
    return r;
}

UIAssetRecipe UIControlBakeRecipes::ToggleTrackRecipe(int _width, int _height)
{
    UIAssetRecipe r;
    r.semantic = UIAssetSemantic::ToggleTrack;
    r.width = _width;
    r.height = _height;
    r.sliceInsets = { 6, 6, 6, 6 };
    r.minimumSize = cc::Size(12, 12);
    return r;
}

UIAssetRecipe UIControlBakeRecipes::ToggleKnobRecipe(int _size)
{
    UIAssetRecipe r;
    r.semantic = UIAssetSemantic::ToggleKnob;
    r.width = _size;
    r.height = _size;
    return r;
}

UIAssetRecipe UIControlBakeRecipes::ProgressTrackRecipe(int _width, int _height)
{
    UIAssetRecipe r;
    r.semantic = UIAssetSemantic::ProgressTrack;
    r.width = _width;
    r.height = _height;
    r.sliceInsets = { 8, 8, 8, 8 };
    r.minimumSize = cc::Size(16, 16);
    return r;
}

UIAssetRecipe UIControlBakeRecipes::ProgressGaugeRecipe(int _width, int _height)
{
    UIAssetRecipe r;
    r.semantic = UIAssetSemantic::ProgressGauge;
    r.width = _width;
    r.height = _height;
    r.sliceInsets = { 6, 6, 6, 6 };
    r.minimumSize = cc::Size(12, 12);
    return r;
}

UIAssetRecipe UIControlBakeRecipes::SliderTrackRecipe(int _width, int _height)
{
    UIAssetRecipe r;
    r.semantic = UIAssetSemantic::SliderTrack;
    r.width = _width;
    r.height = _height;
    r.sliceInsets = { 6, 6, 6, 6 };
    r.minimumSize = cc::Size(12, 12);
    return r;
}

UIAssetRecipe UIControlBakeRecipes::SliderThumbRecipe(int _size)
{
    UIAssetRecipe r;
    r.semantic = UIAssetSemantic::SliderThumb;
    r.width = _size;
    r.height = _size;
    r.sliceInsets = { 8, 8, 8, 8 };
    r.minimumSize = cc::Size(16, 16);
    return r;
}
