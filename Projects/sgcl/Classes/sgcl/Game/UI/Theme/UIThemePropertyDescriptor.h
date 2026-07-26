#pragma once

#include "sgcl/Game/UI/Theme/UIThemeTypes.h"
#include "sgcl/Game/UI/Theme/UIRuntimeTheme.h"
#include "sgcl/Game/UI/Theme/UIStyleOverride.h"

enum class UIPropertyEditorType : uint8_t
{
    Color,
    Slider,
    Dropdown,
    Toggle
};

struct UIThemePropertyDescriptor
{
    UIStyleToken token;
    const char* label;
    UIPropertyEditorType editor;
    float minValue = 0.0f;
    float maxValue = 1.0f;
    float step = 0.01f;
    const char* tooltip = nullptr;
};

class UIThemePropertyTable
{
public:
    static const UIThemePropertyDescriptor* GetDescriptor(UIStyleToken _token);
    static int GetDescriptorCount();
    static const UIThemePropertyDescriptor* GetDescriptorByIndex(int _index);

    struct Section
    {
        const char* name;
        int startIndex;
        int count;
    };

    static int GetSectionCount();
    static const Section* GetSectionByIndex(int _index);

private:
    static UIThemePropertyDescriptor descriptors_[];
    static Section sections_[];
    static int descriptorCount_;
    static int sectionCount_;
};
