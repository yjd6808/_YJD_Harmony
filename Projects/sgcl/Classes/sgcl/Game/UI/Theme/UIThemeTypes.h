#pragma once

#include "jc/container/HashMap.h"

#include "sg/Util/DescBase.h"

enum class UIColorScheme : uint8_t
{
    Light,
    Dark,
    System
};

enum class UIVisualState : uint8_t
{
    Normal,
    Hover,
    Pressed,
    Disabled,
    Focused,
    Selected,
    Checked,
    Indeterminate,
    Success,
    Warning,
    Error
};

enum class UIAssetSemantic : uint8_t
{
    Button,
    Frame,
    CheckBox,
    Radio,
    ToggleTrack,
    ToggleKnob,
    CheckMark,
    RadioDot,
    ProgressTrack,
    ProgressGauge,
    SliderTrack,
    SliderFill,
    SliderThumb,
    ScrollBarTrack,
    ScrollBarThumb,
    EditBox,
    FocusRing,
    ProgressCap
};

enum class UIScalingMode : uint8_t
{
    Scale9,
    Fixed,
    Stretch,
    Tile,
    ClipFill
};

enum class UIComponentSlot : uint8_t
{
    Background,
    Shell,
    Track,
    Gauge,
    Knob,
    Mark,
    Dot,
    Thumb,
    FocusRing,
    Icon,
    Label,
    Cap
};

enum class UITextureMode : uint8_t
{
    NONE,
    SGA,
    THEME
};

struct UIColorF
{
    float r = 0.0f;
    float g = 0.0f;
    float b = 0.0f;
    float a = 1.0f;

    UIColorF() = default;
    UIColorF(float _r, float _g, float _b, float _a = 1.0f)
        : r(_r), g(_g), b(_b), a(_a) {}

    static UIColorF FromRGBA(uint8_t _r, uint8_t _g, uint8_t _b, uint8_t _a = 255)
    {
        return { _r / 255.0f, _g / 255.0f, _b / 255.0f, _a / 255.0f };
    }

    static UIColorF FromHex(uint32_t _hex)
    {
        return FromRGBA(
            (_hex >> 16) & 0xFF,
            (_hex >> 8) & 0xFF,
            _hex & 0xFF,
            (_hex >> 24) & 0xFF
        );
    }

    UIColorF WithAlpha(float _a) const { return { r, g, b, _a }; }

    bool operator==(const UIColorF& _other) const
    {
        return r == _other.r && g == _other.g && b == _other.b && a == _other.a;
    }
};

struct UISliceInsets
{
    float left = 0.0f;
    float top = 0.0f;
    float right = 0.0f;
    float bottom = 0.0f;

    float Width() const { return left + right; }
    float Height() const { return top + bottom; }
};

struct UIAssetKey
{
    UIAssetSemantic semantic;
    uint64_t styleHash;
    uint64_t recipeHash;

    bool operator==(const UIAssetKey& _other) const
    {
        return semantic == _other.semantic
            && styleHash == _other.styleHash
            && recipeHash == _other.recipeHash;
    }

    bool operator<(const UIAssetKey& _other) const
    {
        if (semantic != _other.semantic) return semantic < _other.semantic;
        if (styleHash != _other.styleHash) return styleHash < _other.styleHash;
        return recipeHash < _other.recipeHash;
    }

    bool operator>(const UIAssetKey& _other) const
    {
        return _other < *this;
    }

    uint64_t Hash() const
    {
        return (uint64_t)semantic ^ (styleHash * 0x9E3779B9) ^ (recipeHash * 0xC6A4A793);
    }
};

struct UIThemeRevision
{
    uint64_t sourceRevision = 0;
    uint64_t mappedRevision = 0;
    uint64_t textureRevision = 0;

    bool IsValid() const { return sourceRevision > 0; }
};

struct UITextureCacheKey
{
    uint64_t bakerVersion = 0;
    uint64_t mapperVersion = 0;
    uint64_t recipeVersion = 0;
    uint32_t dpiScaleQ8 = 0;
    uint32_t pixelFormat = 0;
    uint64_t resolvedStyleHash = 0;
    uint64_t recipeHash = 0;
    uint64_t stateHash = 0;

    bool operator==(const UITextureCacheKey& _other) const
    {
        return bakerVersion == _other.bakerVersion
            && mapperVersion == _other.mapperVersion
            && recipeVersion == _other.recipeVersion
            && dpiScaleQ8 == _other.dpiScaleQ8
            && pixelFormat == _other.pixelFormat
            && resolvedStyleHash == _other.resolvedStyleHash
            && recipeHash == _other.recipeHash
            && stateHash == _other.stateHash;
    }

    bool operator<(const UITextureCacheKey& _other) const
    {
        if (bakerVersion != _other.bakerVersion) return bakerVersion < _other.bakerVersion;
        if (mapperVersion != _other.mapperVersion) return mapperVersion < _other.mapperVersion;
        if (recipeVersion != _other.recipeVersion) return recipeVersion < _other.recipeVersion;
        if (dpiScaleQ8 != _other.dpiScaleQ8) return dpiScaleQ8 < _other.dpiScaleQ8;
        if (pixelFormat != _other.pixelFormat) return pixelFormat < _other.pixelFormat;
        if (resolvedStyleHash != _other.resolvedStyleHash) return resolvedStyleHash < _other.resolvedStyleHash;
        if (recipeHash != _other.recipeHash) return recipeHash < _other.recipeHash;
        return stateHash < _other.stateHash;
    }

    bool operator>(const UITextureCacheKey& _other) const
    {
        return _other < *this;
    }
};

template <>
struct jc::Hasher<UIAssetKey>
{
    _u32 operator()(const UIAssetKey& _val) const
    {
        return (_u32)(_val.Hash() & 0xFFFFFFFF);
    }
};

template <>
struct jc::Hasher<UITextureCacheKey>
{
    _u32 operator()(const UITextureCacheKey& _val) const
    {
        _u32 h = (_u32)(_val.bakerVersion & 0xFFFFFFFF);
        h ^= (_u32)(_val.mapperVersion & 0xFFFFFFFF) * 0x9E3779B9;
        h ^= (_u32)(_val.recipeVersion & 0xFFFFFFFF) * 0xC6A4A793;
        h ^= _val.dpiScaleQ8 * 0xABCDEF01;
        h ^= _val.pixelFormat * 0x12345678;
        h ^= (_u32)(_val.resolvedStyleHash & 0xFFFFFFFF);
        h ^= (_u32)(_val.recipeHash & 0xFFFFFFFF) * 0x9E3779B9;
        h ^= (_u32)(_val.stateHash & 0xFFFFFFFF) * 0xC6A4A793;
        return h;
    }
};
