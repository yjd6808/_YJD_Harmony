#pragma once

#include "sg/Util/DescBase.h"
#include "sgcl/Game/UI/Theme/Baker/UIPixelBuffer.h"

struct UIMaskBuffer
{
    int width = 0;
    int height = 0;
    jc::Vector<float> alpha;

    UIMaskBuffer() = default;
    UIMaskBuffer(int _w, int _h);

    void Resize(int _w, int _h);
    void Clear(float _value = 0.0f);

    float& operator()(int _x, int _y)
    {
        return alpha[_y * width + _x];
    }

    const float& operator()(int _x, int _y) const
    {
        return alpha[_y * width + _x];
    }

    void Mul(const UIMaskBuffer& _other);
    void Mul(float _scalar);
};
