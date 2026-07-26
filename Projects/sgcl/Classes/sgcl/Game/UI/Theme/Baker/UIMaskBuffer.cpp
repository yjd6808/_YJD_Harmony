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
    for (int i = 0; i < alpha.Size(); ++i)
        alpha[i] = _value;
}

void UIMaskBuffer::Mul(const UIMaskBuffer& _other)
{
    int count = jc::Math::Min(alpha.Size(), _other.alpha.Size());
    for (int i = 0; i < count; ++i)
        alpha[i] *= _other.alpha[i];
}

void UIMaskBuffer::Mul(float _scalar)
{
    for (int i = 0; i < alpha.Size(); ++i)
        alpha[i] *= _scalar;
}
