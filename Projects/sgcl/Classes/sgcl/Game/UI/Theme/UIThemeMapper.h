#pragma once

#include <json.h>
#include "sgcl/Game/UI/Theme/UIThemeTypes.h"
#include "sgcl/Game/UI/Theme/UIRuntimeTheme.h"

class UIThemeMapper
{
public:
    static UIColorF Mix(const UIColorF& _a, const UIColorF& _b, int _percent);
    static UIColorF Mix(const UIColorF& _a, const UIColorF& _b, float _t);

    static UIColorF ReadColor(const Json::Value& _root, const char* _path, const UIColorF& _fallback);
    static float ReadFloat(const Json::Value& _root, const char* _path, float _fallback);
    static jc::String ReadString(const Json::Value& _root, const char* _path, const char* _fallback);

    static UIRuntimeTheme Map(const Json::Value& _source, UIColorScheme _scheme);

    static UIColorScheme ResolveScheme(UIColorScheme _requested);

    static void ApplyLightScheme(UIRuntimeTheme& _theme);

private:
    static const Json::Value* FindNode(const Json::Value& _root, const char* _path);

    uint32_t GetMapperVersion() const { return mapperVersion_; }

private:
    uint32_t mapperVersion_ = 1;
};
