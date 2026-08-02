#pragma once

#include "sg/Util/DescBase.h"
#include "sgcl/Game/UI/Theme/UIThemeTypes.h"
#include "jc/Primitives/String.h"

struct UIPixelBuffer
{
    int width = 0;
    int height = 0;
    int stride = 0;
    jc::Vector<uint8_t> pixels;

    UIPixelBuffer() = default;
    UIPixelBuffer(int _w, int _h);

    void Resize(int _w, int _h);
    void Clear(const UIColorF& _color);
    UIColorF Load(int _x, int _y) const;
    void Store(int _x, int _y, const UIColorF& _color);
    void BlendOver(int _x, int _y, const UIColorF& _src);

    uint8_t* RowPtr(int _y) { return &pixels[0] + _y * stride; }
    const uint8_t* RowPtr(int _y) const { return &pixels[0] + _y * stride; }

private:
    static uint8_t FloatToByte(float _v);
    static float ByteToFloat(uint8_t _v);
};

struct UIRect
{
    float x = 0.0f;
    float y = 0.0f;
    float w = 0.0f;
    float h = 0.0f;

    float Left() const { return x; }
    float Right() const { return x + w; }
    float Bottom() const { return y; }
    float Top() const { return y + h; }

    UIRect Offset(float _dx, float _dy) const
    {
        return { x + _dx, y + _dy, w, h };
    }
};

struct UIBakeOptions
{
    int supersample = 4;
    bool premultipliedAlpha = true;
};

struct UIAssetRecipe
{
    UIAssetSemantic semantic;
    int width = 0;
    int height = 0;
    UISliceInsets sliceInsets;
    cc::Size minimumSize;
    jc::String svgPath;	// 아이콘: 베이크할 SVG 파일명 (비어 있으면 도형 레시피)
};

struct UIBakeOutput
{
    UIPixelBuffer buffer;
    UISliceInsets sliceInsets;
    cc::Size minimumSize;
    float contentPadding = 0.0f;
};
