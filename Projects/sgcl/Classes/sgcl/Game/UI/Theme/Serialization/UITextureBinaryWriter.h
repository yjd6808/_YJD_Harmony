#pragma once

#include "sgcl/Game/UI/Theme/UIThemeTypes.h"
#include "sgcl/Game/UI/Theme/Baker/UIPixelBuffer.h"

#pragma pack(push, 1)
struct UITXHeader
{
    uint32_t magic = 0x58544955;
    uint16_t formatVersion = 1;
    uint16_t headerSize = sizeof(UITXHeader);
    uint32_t flags = 0;

    uint32_t width = 0;
    uint32_t height = 0;
    uint32_t pixelFormat = 0;
    uint32_t dataSize = 0;

    uint16_t sliceLeft = 0;
    uint16_t sliceTop = 0;
    uint16_t sliceRight = 0;
    uint16_t sliceBottom = 0;

    uint16_t contentX = 0;
    uint16_t contentY = 0;
    uint16_t contentWidth = 0;
    uint16_t contentHeight = 0;

    uint64_t cacheKeyHigh = 0;
    uint64_t cacheKeyLow = 0;
    uint32_t crc32 = 0;
};
#pragma pack(pop)

enum UITXFlags : uint32_t
{
    UITX_PremultipliedAlpha = 1 << 0,
    UITX_SRGB = 1 << 1,
    UITX_Compressed = 1 << 2,
    UITX_HasMipMap = 1 << 3,
    UITX_AtlasPage = 1 << 4
};

class UITextureBinaryWriter
{
public:
    static bool WriteToFile(const jc::String& _path, const UIPixelBuffer& _buffer,
        const UISliceInsets& _insets, const UITextureCacheKey& _cacheKey);

    static bool ReadFromFile(const jc::String& _path, UIPixelBuffer& _outBuffer,
        UISliceInsets& _outInsets);
};
