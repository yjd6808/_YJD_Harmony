#pragma once

#include "sgcl/Game/UI/Theme/UIThemeTypes.h"
#include "sgcl/Game/UI/Theme/UITextureEntry.h"

enum class UIBindingKind : uint8_t
{
    Scale9,
    Fixed
};

struct UIThemeBindingSlot
{
    UIBindingKind kind = UIBindingKind::Fixed;
    cc::Node* target = nullptr;
    UIAssetKey key;
    UIComponentSlot slot = UIComponentSlot::Background;

    UIThemeBindingSlot() = default;
    UIThemeBindingSlot(UIBindingKind _kind, cc::Node* _target, const UIAssetKey& _key, UIComponentSlot _slot)
        : kind(_kind), target(_target), key(_key), slot(_slot) {}
};

class UIThemeTextureBinding
{
public:
    void BindScale9(cc_ui::Scale9Sprite* _target, const UIAssetKey& _key, UIComponentSlot _slot);
    void BindFixed(cc::Sprite* _target, const UIAssetKey& _key, UIComponentSlot _slot);
    void Refresh(const UITextureSet& _set);
    void Clear();

    bool HasBinding() const { return !slots_.IsEmpty(); }

private:
    void ApplyMissingTexture(const UIThemeBindingSlot& _slot);

    jc::Vector<UIThemeBindingSlot> slots_;
};
