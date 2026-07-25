#pragma once

#include <cmath>
#include "sgcl/Game/UI/Theme/UIThemeTypes.h"

namespace UIColorMath
{
    inline float Clamp01(float _v)
    {
        if (_v < 0.0f) return 0.0f;
        if (_v > 1.0f) return 1.0f;
        return _v;
    }

    inline float SmoothStep01(float _t)
    {
        _t = Clamp01(_t);
        return _t * _t * (3.0f - 2.0f * _t);
    }

    inline UIColorF Lerp(const UIColorF& _a, const UIColorF& _b, float _t)
    {
        return {
            _a.r + (_b.r - _a.r) * _t,
            _a.g + (_b.g - _a.g) * _t,
            _a.b + (_b.b - _a.b) * _t,
            _a.a + (_b.a - _a.a) * _t
        };
    }

    inline UIColorF SRGBToLinear(const UIColorF& _srgb)
    {
        auto toLinear = [](float _c) -> float {
            if (_c <= 0.04045f) return _c / 12.92f;
            return powf((_c + 0.055f) / 1.055f, 2.4f);
        };
        return { toLinear(_srgb.r), toLinear(_srgb.g), toLinear(_srgb.b), _srgb.a };
    }

    inline UIColorF LinearToSRGB(const UIColorF& _linear)
    {
        auto toSRGB = [](float _c) -> float {
            if (_c <= 0.0031308f) return _c * 12.92f;
            return 1.055f * powf(_c, 1.0f / 2.4f) - 0.055f;
        };
        return { toSRGB(_linear.r), toSRGB(_linear.g), toSRGB(_linear.b), _linear.a };
    }

    inline float CoverageFromDistance(float _distance)
    {
        return Clamp01(0.5f - _distance);
    }
}
