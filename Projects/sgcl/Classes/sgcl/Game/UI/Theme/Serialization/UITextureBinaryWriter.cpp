#include "GameCoreHeader.h"
#include "sgcl/Game/UI/Theme/Serialization/UITextureBinaryWriter.h"

bool UITextureBinaryWriter::WriteToFile(const jc::String& _path, const UIPixelBuffer& _buffer,
    const UISliceInsets& _insets, const UITextureCacheKey& _cacheKey)
{
    UITXHeader header;
    header.width = _buffer.width;
    header.height = _buffer.height;
    header.pixelFormat = 0;
    header.dataSize = _buffer.pixels.Size();
    header.flags = UITX_PremultipliedAlpha;

    header.sliceLeft = (uint16_t)_insets.left;
    header.sliceTop = (uint16_t)_insets.top;
    header.sliceRight = (uint16_t)_insets.right;
    header.sliceBottom = (uint16_t)_insets.bottom;

    header.contentWidth = _buffer.width;
    header.contentHeight = _buffer.height;

    header.cacheKeyHigh = _cacheKey.resolvedStyleHash ^ _cacheKey.recipeHash;
    header.cacheKeyLow = _cacheKey.stateHash;

    FILE* fp = nullptr;
    if (fopen_s(&fp, _path.SafeSource(), "wb") != 0 || !fp)
        return false;

    fwrite(&header, sizeof(header), 1, fp);
    fwrite(&_buffer.pixels[0], 1, _buffer.pixels.Size(), fp);
    fclose(fp);
    return true;
}

bool UITextureBinaryWriter::ReadFromFile(const jc::String& _path, UIPixelBuffer& _outBuffer,
    UISliceInsets& _outInsets)
{
    FILE* fp = nullptr;
    if (fopen_s(&fp, _path.SafeSource(), "rb") != 0 || !fp)
        return false;

    UITXHeader header;
    if (fread(&header, sizeof(header), 1, fp) != 1)
    {
        fclose(fp);
        return false;
    }

    if (header.magic != 0x58544955)
    {
        fclose(fp);
        return false;
    }

    _outBuffer.Resize(header.width, header.height);
    if (fread(&_outBuffer.pixels[0], 1, header.dataSize, fp) != header.dataSize)
    {
        fclose(fp);
        return false;
    }

    _outInsets.left = (float)header.sliceLeft;
    _outInsets.top = (float)header.sliceTop;
    _outInsets.right = (float)header.sliceRight;
    _outInsets.bottom = (float)header.sliceBottom;

    fclose(fp);
    return true;
}
