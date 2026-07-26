#pragma once

#include "sgcl/Game/UI/Theme/Baker/UIMaskBuffer.h"

namespace UIGaussianBlur
{
    void Blur(UIMaskBuffer& _buffer, float _radius);
    void BlurSeparable(UIMaskBuffer& _buffer, float _radius);
}
