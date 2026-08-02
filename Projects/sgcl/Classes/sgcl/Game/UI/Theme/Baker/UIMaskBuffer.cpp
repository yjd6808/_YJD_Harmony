#include "GameCoreHeader.h"
#include "sgcl/Game/UI/Theme/Baker/UIMaskBuffer.h"

UIMaskBuffer::UIMaskBuffer(int _w, int _h)
{
    Resize(_w, _h);
}

void UIMaskBuffer::Resize(int _w, int _h)
{
    width = _w;
    height = _h;
    alpha.Resize(_w * _h);
}

void UIMaskBuffer::Clear(float _value)
{
    for (int idx = 0; idx < alpha.Size(); ++idx)
    {
        alpha[idx] = _value;
    }
}

void UIMaskBuffer::Mul(const UIMaskBuffer& _other)
{
    int count = jc::Math::Min(alpha.Size(), _other.alpha.Size());
    for (int idx = 0; idx < count; ++idx)
    {
        alpha[idx] *= _other.alpha[idx];
    }
}

void UIMaskBuffer::Mul(float _scalar)
{
    for (int idx = 0; idx < alpha.Size(); ++idx)
    {
        alpha[idx] *= _scalar;
    }
}
