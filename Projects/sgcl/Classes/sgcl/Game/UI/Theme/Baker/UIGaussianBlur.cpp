#include "GameCoreHeader.h"
#include "sgcl/Game/UI/Theme/Baker/UIGaussianBlur.h"
#include <cmath>

namespace UIGaussianBlur
{
    static void BuildKernel(float _radius, jc::Vector<float>& _kernel, int& _size)
    {
        int r = (int)ceilf(_radius);
        if (r < 1)
        {
            r = 1;
        }
        _size = r * 2 + 1;
        _kernel.Resize(_size);

        float sum = 0.0f;
        float sigma = jc::Math::Max(_radius, 0.5f);
        float sigma2 = sigma * sigma;

        for (int idx = 0; idx < _size; ++idx)
        {
            float x = (float)(idx - r);
            _kernel[idx] = expf(-(x * x) / (2.0f * sigma2));
            sum += _kernel[idx];
        }

        for (int idx = 0; idx < _size; ++idx)
        {
            _kernel[idx] /= sum;
        }
    }

    void BlurSeparable(UIMaskBuffer& _buffer, float _radius)
    {
        jc::Vector<float> kernel;
        int halfSize;
        BuildKernel(_radius, kernel, halfSize);
        int r = halfSize / 2;

        UIMaskBuffer temp(_buffer.width, _buffer.height);

        for (int y = 0; y < _buffer.height; ++y)
        {
            for (int x = 0; x < _buffer.width; ++x)
            {
                float sum = 0.0f;
                for (int k = -r; k <= r; ++k)
                {
                    int sx = jc::Math::Clamp(x + k, 0, _buffer.width - 1);
                    sum += _buffer(sx, y) * kernel[k + r];
                }
                temp(x, y) = sum;
            }
        }

        for (int y = 0; y < _buffer.height; ++y)
        {
            for (int x = 0; x < _buffer.width; ++x)
            {
                float sum = 0.0f;
                for (int k = -r; k <= r; ++k)
                {
                    int sy = jc::Math::Clamp(y + k, 0, _buffer.height - 1);
                    sum += temp(x, sy) * kernel[k + r];
                }
                _buffer(x, y) = sum;
            }
        }
    }

    void Blur(UIMaskBuffer& _buffer, float _radius)
    {
        BlurSeparable(_buffer, _radius);
    }
}
