#include "GameCoreHeader.h"
#include "sgcl/Game/UI/Theme/UIThemeBinding.h"

void UIThemeTextureBinding::BindScale9(cc_ui::Scale9Sprite* _target, const UIAssetKey& _key, UIComponentSlot _slot)
{
    if (!_target) return;

    UIThemeBindingSlot slot(UIBindingKind::Scale9, _target, _key, _slot);
    slots_.PushBack(slot);
}

void UIThemeTextureBinding::BindFixed(cc::Sprite* _target, const UIAssetKey& _key, UIComponentSlot _slot)
{
    if (!_target) return;

    UIThemeBindingSlot slot(UIBindingKind::Fixed, _target, _key, _slot);
    slots_.PushBack(slot);
}

void UIThemeTextureBinding::Clear()
{
    slots_.Clear();
}

void UIThemeTextureBinding::Refresh(const UITextureSet& _set)
{
    for (int i = 0; i < slots_.Size(); ++i)
    {
        auto& slot = slots_[i];
        const UITextureEntry* entry = _set.Find(slot.key);
        if (!entry)
        {
            ApplyMissingTexture(slot);
            continue;
        }

        cc::Texture2D* tex = entry->texture;
        if (!tex) continue;

        if (slot.kind == UIBindingKind::Scale9)
        {
            auto* scale9 = dynamic_cast<cc_ui::Scale9Sprite*>(slot.target);
            if (!scale9) continue;

            auto* frame = cc::SpriteFrame::createWithTexture(
                tex, entry->atlasRect, entry->rotated, {}, entry->atlasRect.size);

            scale9->initWithSpriteFrame(frame);
            scale9->setCapInsets({
                entry->atlasRect.origin.x + entry->sliceInsets.left,
                entry->atlasRect.origin.y + entry->sliceInsets.bottom,
                entry->atlasRect.size.width - entry->sliceInsets.left - entry->sliceInsets.right,
                entry->atlasRect.size.height - entry->sliceInsets.top - entry->sliceInsets.bottom
            });
        }
        else
        {
            auto* sprite = dynamic_cast<cc::Sprite*>(slot.target);
            if (!sprite) continue;

            sprite->initWithTexture(tex, entry->atlasRect);
        }
    }
}

void UIThemeTextureBinding::ApplyMissingTexture(const UIThemeBindingSlot& /*_slot*/)
{
}
