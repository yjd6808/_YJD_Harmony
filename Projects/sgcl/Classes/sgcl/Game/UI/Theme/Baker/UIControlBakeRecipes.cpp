#include "GameCoreHeader.h"
#include "sgcl/Game/UI/Theme/Baker/UIControlBakeRecipes.h"

UIAssetRecipe UIControlBakeRecipes::ButtonRecipe(int _width, int _height)
{
    UIAssetRecipe r;
    r.semantic = UIAssetSemantic::Button;
    r.width = _width;
    r.height = _height;
    // 인셋 8px(min 16px): 20x20 소형 버튼도 Scale9 + 테두리가 적용되도록 최소 크기를 낮춘다.
    r.sliceInsets = { 8, 8, 8, 8 };
    r.minimumSize = cc::Size(16, 16);
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
    r.sliceInsets = { 6, 0, 6, 0 };
    r.minimumSize = cc::Size(12, 12);
    return r;
}

UIAssetRecipe UIControlBakeRecipes::ProgressCapRecipe(int _size)
{
    UIAssetRecipe r;
    r.semantic = UIAssetSemantic::ProgressCap;
    r.width = _size;
    r.height = _size;
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

UIAssetRecipe UIControlBakeRecipes::SliderFillRecipe(int _width, int _height)
{
    UIAssetRecipe r;
    r.semantic = UIAssetSemantic::SliderFill;
    r.width = _width;
    r.height = _height;
    r.sliceInsets = { 6, 6, 6, 6 };
    r.minimumSize = cc::Size(12, 12);
    return r;
}

UIAssetRecipe UIControlBakeRecipes::ScrollBarTrackRecipe(int _width, int _height)
{
    UIAssetRecipe r;
    r.semantic = UIAssetSemantic::ScrollBarTrack;
    r.width = _width;
    r.height = _height;
    r.sliceInsets = { 4, 4, 4, 4 };
    r.minimumSize = cc::Size(8, 8);
    return r;
}

UIAssetRecipe UIControlBakeRecipes::ScrollBarThumbRecipe(int _size)
{
    UIAssetRecipe r;
    r.semantic = UIAssetSemantic::ScrollBarThumb;
    r.width = _size;
    r.height = _size;
    r.sliceInsets = { 8, 8, 8, 8 };
    r.minimumSize = cc::Size(16, 16);
    return r;
}

UIAssetRecipe UIControlBakeRecipes::EditBoxRecipe(int _width, int _height)
{
    UIAssetRecipe r;
    r.semantic = UIAssetSemantic::EditBox;
    r.width = _width;
    r.height = _height;
    r.sliceInsets = { 10, 10, 10, 10 };
    r.minimumSize = cc::Size(20, 20);
    return r;
}

UIAssetRecipe UIControlBakeRecipes::WindowIconRecipe(UIAssetSemantic _semantic, int _size)
{
    UIAssetRecipe r;
    r.semantic = _semantic;
    r.width = _size;
    r.height = _size;

    switch (_semantic)
    {
    case UIAssetSemantic::WindowIconMinimize: r.svgPath = "window-minimize.svg"; break;
    case UIAssetSemantic::WindowIconMaximize: r.svgPath = "window-maximize.svg"; break;
    case UIAssetSemantic::WindowIconClose:    r.svgPath = "window-close.svg"; break;
    default: break;
    }

    return r;
}
