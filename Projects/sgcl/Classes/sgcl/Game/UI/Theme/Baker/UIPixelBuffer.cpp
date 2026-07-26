#include "GameCoreHeader.h"
#include "sgcl/Game/UI/Theme/Baker/UIPixelBuffer.h"

UIPixelBuffer::UIPixelBuffer(int _w, int _h)
{
    Resize(_w, _h);
}

void UIPixelBuffer::Resize(int _w, int _h)
{
    width = _w;
    height = _h;
    stride = _w * 4;
    pixels.Resize(stride * _h);
	jc::Memory::Set(&pixels[0], pixels.Size(), 0);
}

void UIPixelBuffer::Clear(const UIColorF& _color)
{
    for (int y = 0; y < height; ++y)
        for (int x = 0; x < width; ++x)
            Store(x, y, _color);
}

uint8_t UIPixelBuffer::FloatToByte(float _v)
{
    int i = (int)(_v * 255.0f + 0.5f);
    if (i < 0) i = 0;
    if (i > 255) i = 255;
    return (uint8_t)i;
}

float UIPixelBuffer::ByteToFloat(uint8_t _v)
{
    return _v / 255.0f;
}

UIColorF UIPixelBuffer::Load(int _x, int _y) const
{
    if (_x < 0 || _x >= width || _y < 0 || _y >= height)
        return { 0, 0, 0, 0 };

    const uint8_t* p = &pixels[0] + _y * stride + _x * 4;
    return { ByteToFloat(p[0]), ByteToFloat(p[1]), ByteToFloat(p[2]), ByteToFloat(p[3]) };
}

void UIPixelBuffer::Store(int _x, int _y, const UIColorF& _color)
{
    if (_x < 0 || _x >= width || _y < 0 || _y >= height)
        return;

    uint8_t* p = &pixels[0] + _y * stride + _x * 4;
    p[0] = FloatToByte(_color.r);
    p[1] = FloatToByte(_color.g);
    p[2] = FloatToByte(_color.b);
    p[3] = FloatToByte(_color.a);
}

void UIPixelBuffer::BlendOver(int _x, int _y, const UIColorF& _src)
{
    UIColorF dst = Load(_x, _y);
    float outA = _src.a + dst.a * (1.0f - _src.a);

    if (outA <= 0.0f)
    {
        Store(_x, _y, { 0, 0, 0, 0 });
        return;
    }

    UIColorF result;
    result.r = (_src.r * _src.a + dst.r * dst.a * (1.0f - _src.a)) / outA;
    result.g = (_src.g * _src.a + dst.g * dst.a * (1.0f - _src.a)) / outA;
    result.b = (_src.b * _src.a + dst.b * dst.a * (1.0f - _src.a)) / outA;
    result.a = outA;
    Store(_x, _y, result);
}
