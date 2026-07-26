#pragma once

#include "sgcl/Game/UI/Theme/UIThemeTypes.h"

class UITextureEntry
{
public:
    UIAssetKey key;
    cc::Texture2D* texture = nullptr;
    cc::Rect atlasRect;
    UISliceInsets sliceInsets;
    cc::Size minimumSize;
    cc::Rect contentRect;
    bool rotated = false;
    bool premultipliedAlpha = true;

    UITextureEntry() = default;
    UITextureEntry(const UITextureEntry&) = delete;
    UITextureEntry& operator=(const UITextureEntry&) = delete;

    ~UITextureEntry()
    {
        CC_SAFE_RELEASE(texture);
    }

    void SetTexture(cc::Texture2D* _tex)
    {
        CC_SAFE_RELEASE(texture);
        texture = _tex;
        CC_SAFE_RETAIN(texture);
    }
};

class UITextureSet : public cc::Ref
{
public:
    uint64_t GetGeneration() const { return generation_; }
    uint64_t GetThemeHash() const { return themeHash_; }
    int GetEntryCount() const { return (int)entries_.Size(); }

    const UITextureEntry* Find(const UIAssetKey& _key) const;

    void AddEntry(const UIAssetKey& _key, UITextureEntry* _entry);
    void SetAtlases(const jc::Vector<cc::Texture2D*>& _atlases);

    UITextureSet(uint64_t _generation, uint64_t _themeHash);
    ~UITextureSet() override;

private:
    uint64_t generation_;
    uint64_t themeHash_;
    jc::Vector<cc::Texture2D*> atlases_;
    jc::HashMap<UIAssetKey, UITextureEntry*> entries_;
};
