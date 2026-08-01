#include "GameCoreHeader.h"
#include "sgcl/Game/UI/Theme/UITextureEntry.h"

UITextureSet::UITextureSet(uint64_t _generation, uint64_t _themeHash)
    : generation_(_generation)
    , themeHash_(_themeHash)
{
}

UITextureSet::~UITextureSet()
{
    entries_.ForEachValue([](UITextureEntry* _entry) {
        CC_SAFE_RELEASE(_entry);
    });
    entries_.Clear();

    for (int i = 0; i < atlases_.Size(); ++i)
        CC_SAFE_RELEASE(atlases_[i]);
    atlases_.Clear();
}

const UITextureEntry* UITextureSet::Find(const UIAssetKey& _key) const
{
    auto* pEntry = entries_.Find(_key);
    return pEntry ? *pEntry : nullptr;
}

void UITextureSet::AddEntry(const UIAssetKey& _key, UITextureEntry* _entry)
{
    CC_SAFE_RETAIN(_entry);
    entries_.Insert(_key, _entry);
}

void UITextureSet::SetAtlases(const jc::Vector<cc::Texture2D*>& _atlases)
{
    for (int i = 0; i < atlases_.Size(); ++i)
        CC_SAFE_RELEASE(atlases_[i]);
    atlases_.Clear();

    atlases_ = _atlases;
    for (int i = 0; i < atlases_.Size(); ++i)
        CC_SAFE_RETAIN(atlases_[i]);
}
