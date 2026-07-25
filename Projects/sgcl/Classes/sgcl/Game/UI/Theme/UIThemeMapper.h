#pragma once

#include <json/value.h>
#include "sgcl/Game/UI/Theme/UIThemeTypes.h"
#include "sgcl/Game/UI/Theme/UIRuntimeTheme.h"

class UIThemeMapper
{
public:
    static UIColorF Mix(const UIColorF& _a, const UIColorF& _b, int _percent);
    static UIColorF Mix(const UIColorF& _a, const UIColorF& _b, float _t);

    static UIColorF ReadColor(const Json::Value& _root, const char* _path, const UIColorF& _fallback);

    static UIRuntimeTheme Map(const Json::Value& _source, UIColorScheme _scheme);

    static UIColorScheme ResolveScheme(UIColorScheme _requested);

    uint32_t GetMapperVersion() const { return mapperVersion_; }

private:
    uint32_t mapperVersion_ = 1;
};
