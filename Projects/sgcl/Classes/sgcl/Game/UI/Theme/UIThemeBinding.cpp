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
            _LogWarn_("[UIThemeBinding] Entry not found: semantic=%d styleHash=%llu recipeHash=%llu",
                (int)slot.key.semantic, slot.key.styleHash, slot.key.recipeHash);
            ApplyMissingTexture(slot);
            continue;
        }

        cc::Texture2D* tex = entry->texture;
        if (!tex)
        {
            _LogWarn_("[UIThemeBinding] Entry found but texture is null: semantic=%d", (int)slot.key.semantic);
            continue;
        }

        _LogDebug_("[UIThemeBinding] Applying texture semantic=%d tex=%p rect=(%.0f,%.0f,%.0f,%.0f) slice=(%.1f,%.1f,%.1f,%.1f)",
            (int)slot.key.semantic, tex,
            entry->atlasRect.origin.x, entry->atlasRect.origin.y,
            entry->atlasRect.size.width, entry->atlasRect.size.height,
            entry->sliceInsets.left, entry->sliceInsets.top,
            entry->sliceInsets.right, entry->sliceInsets.bottom);

        if (slot.kind == UIBindingKind::Scale9)
        {
            auto* scale9 = dynamic_cast<cc_ui::Scale9Sprite*>(slot.target);
            if (!scale9) continue;

            auto* frame = cc::SpriteFrame::createWithTexture(
                tex, entry->atlasRect, entry->rotated, {}, entry->atlasRect.size);

            if (frame)
            {
                scale9->setSpriteFrame(frame, {
                    entry->sliceInsets.left,
                    entry->sliceInsets.bottom,
                    entry->atlasRect.size.width - entry->sliceInsets.left - entry->sliceInsets.right,
                    entry->atlasRect.size.height - entry->sliceInsets.top - entry->sliceInsets.bottom
                });
                scale9->setPreferredSize(scale9->getContentSize());

                _LogDebug_("[UIThemeBinding] After setSpriteFrame: contentSize=(%.0f,%.0f) preferredSize=(%.0f,%.0f) isVisible=%d",
                    scale9->getContentSize().width, scale9->getContentSize().height,
                    scale9->getPreferredSize().width, scale9->getPreferredSize().height,
                    scale9->isVisible());
            }
            else
            {
                _LogWarn_("[UIThemeBinding] SpriteFrame::createWithTexture returned null!");
            }
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
