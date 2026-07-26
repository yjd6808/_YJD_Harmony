#include "GameCoreHeader.h"
#include "sgcl/Game/UI/Theme/UIThemePropertyDescriptor.h"

UIThemePropertyDescriptor UIThemePropertyTable::descriptors_[] = {
    { UIStyleToken::SurfaceNormalTop,        "Surface Normal Top",      UIPropertyEditorType::Color,   0, 1, 0.01f, "Top color of normal surface gradient" },
    { UIStyleToken::SurfaceNormalBottom,     "Surface Normal Bottom",   UIPropertyEditorType::Color,   0, 1, 0.01f, "Bottom color of normal surface gradient" },
    { UIStyleToken::SurfaceHoverTop,         "Surface Hover Top",       UIPropertyEditorType::Color,   0, 1, 0.01f, "Top color of hover surface gradient" },
    { UIStyleToken::SurfaceHoverBottom,      "Surface Hover Bottom",    UIPropertyEditorType::Color,   0, 1, 0.01f, "Bottom color of hover surface gradient" },
    { UIStyleToken::SurfacePressedTop,       "Surface Pressed Top",     UIPropertyEditorType::Color,   0, 1, 0.01f, "Top color of pressed surface gradient" },
    { UIStyleToken::SurfacePressedBottom,    "Surface Pressed Bottom",  UIPropertyEditorType::Color,   0, 1, 0.01f, "Bottom color of pressed surface gradient" },
    { UIStyleToken::SurfaceDisabledTop,      "Surface Disabled Top",    UIPropertyEditorType::Color,   0, 1, 0.01f, "Top color of disabled surface gradient" },
    { UIStyleToken::SurfaceDisabledBottom,   "Surface Disabled Bottom", UIPropertyEditorType::Color,   0, 1, 0.01f, "Bottom color of disabled surface gradient" },
    { UIStyleToken::SurfaceInsetTop,         "Surface Inset Top",       UIPropertyEditorType::Color,   0, 1, 0.01f, "Top color of surface inset" },
    { UIStyleToken::SurfaceInsetBottom,      "Surface Inset Bottom",    UIPropertyEditorType::Color,   0, 1, 0.01f, "Bottom color of surface inset" },

    { UIStyleToken::MetalBorder,             "Metal Border",            UIPropertyEditorType::Color,   0, 1, 0.01f, "Metal border color" },
    { UIStyleToken::MetalHover,              "Metal Hover",             UIPropertyEditorType::Color,   0, 1, 0.01f, "Metal hover highlight color" },
    { UIStyleToken::MetalHighlight,          "Metal Highlight",         UIPropertyEditorType::Color,   0, 1, 0.01f, "Metal bright highlight color" },
    { UIStyleToken::MetalBright,             "Metal Bright",            UIPropertyEditorType::Color,   0, 1, 0.01f, "Metal bright tone" },
    { UIStyleToken::MetalShadow,             "Metal Shadow",            UIPropertyEditorType::Color,   0, 1, 0.01f, "Metal dark shadow color" },
    { UIStyleToken::MetalMuted,              "Metal Muted",             UIPropertyEditorType::Color,   0, 1, 0.01f, "Metal muted/ subdued color" },

    { UIStyleToken::GlossTint,               "Gloss Tint",              UIPropertyEditorType::Color,   0, 1, 0.01f, "Gloss highlight tint color" },
    { UIStyleToken::GlossNormalAlpha,        "Gloss Alpha",             UIPropertyEditorType::Slider,  0, 1, 0.01f, "Gloss opacity for normal state" },
    { UIStyleToken::GlossHoverMultiplier,    "Gloss Hover Mult",        UIPropertyEditorType::Slider,  0, 3, 0.05f, "Gloss alpha multiplier on hover" },
    { UIStyleToken::GlossDisabledMultiplier, "Gloss Disabled Mult",     UIPropertyEditorType::Slider,  0, 1, 0.05f, "Gloss alpha multiplier when disabled" },
    { UIStyleToken::GlossHeightRatio,        "Gloss Height",            UIPropertyEditorType::Slider,  0, 1, 0.01f, "Height ratio of the gloss highlight" },
    { UIStyleToken::GlossBlur,               "Gloss Blur",              UIPropertyEditorType::Slider,  0, 20, 0.5f, "Blur radius of gloss highlight" },
    { UIStyleToken::GlossCenterX,            "Gloss Center X",          UIPropertyEditorType::Slider,  0, 1, 0.01f, "Horizontal center position of gloss" },

    { UIStyleToken::GeometryRadius,          "Corner Radius",           UIPropertyEditorType::Slider,  0, 40, 1.0f, "Corner radius of all controls" },
    { UIStyleToken::GeometryBorderWidth,     "Border Width",            UIPropertyEditorType::Slider,  0, 10, 0.5f, "Width of the metal border" },
    { UIStyleToken::GeometryInnerRim,        "Inner Rim",               UIPropertyEditorType::Slider,  0, 10, 0.5f, "Inner rim highlight width" },
    { UIStyleToken::GeometryDepth,           "Depth Offset",            UIPropertyEditorType::Slider,  0, 20, 0.5f, "Shadow depth offset in pixels" },
    { UIStyleToken::GeometryShadow,          "Shadow Radius",           UIPropertyEditorType::Slider,  0, 20, 0.5f, "Shadow blur radius" },

    { UIStyleToken::SemanticText,            "Text Color",              UIPropertyEditorType::Color,   0, 1, 0.01f, "Default text color" },
    { UIStyleToken::SemanticDanger,          "Danger Color",            UIPropertyEditorType::Color,   0, 1, 0.01f, "Color for danger/error states" },
    { UIStyleToken::SemanticSuccess,         "Success Color",           UIPropertyEditorType::Color,   0, 1, 0.01f, "Color for success states" },
    { UIStyleToken::SemanticWarning,         "Warning Color",           UIPropertyEditorType::Color,   0, 1, 0.01f, "Color for warning states" },
};

UIThemePropertyTable::Section UIThemePropertyTable::sections_[] = {
    { "Surface",     0,  10 },
    { "Metal",       10, 6 },
    { "Gloss",       16, 7 },
    { "Geometry",    23, 5 },
    { "Semantic",    28, 4 },
};

int UIThemePropertyTable::descriptorCount_ = sizeof(descriptors_) / sizeof(descriptors_[0]);
int UIThemePropertyTable::sectionCount_ = sizeof(sections_) / sizeof(sections_[0]);

const UIThemePropertyDescriptor* UIThemePropertyTable::GetDescriptor(UIStyleToken _token)
{
    for (int i = 0; i < descriptorCount_; ++i)
    {
        if (descriptors_[i].token == _token)
            return &descriptors_[i];
    }
    return nullptr;
}

int UIThemePropertyTable::GetDescriptorCount()
{
    return descriptorCount_;
}

const UIThemePropertyDescriptor* UIThemePropertyTable::GetDescriptorByIndex(int _index)
{
    if (_index < 0 || _index >= descriptorCount_)
        return nullptr;
    return &descriptors_[_index];
}

int UIThemePropertyTable::GetSectionCount()
{
    return sectionCount_;
}

const UIThemePropertyTable::Section* UIThemePropertyTable::GetSectionByIndex(int _index)
{
    if (_index < 0 || _index >= sectionCount_)
        return nullptr;
    return &sections_[_index];
}
