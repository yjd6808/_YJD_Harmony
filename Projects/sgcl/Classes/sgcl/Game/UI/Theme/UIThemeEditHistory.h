#pragma once

#include "sgcl/Game/UI/Theme/UIThemeTypes.h"
#include "sgcl/Game/UI/Theme/UIStyleOverride.h"

struct UIThemeSnapshot
{
    UIResolvedStyle values;
    UIStyleOverrideMask mask;
};

class UIThemeEditHistory
{
public:
    void Clear();
    void Checkpoint(const UIResolvedStyle& _values, const UIStyleOverrideMask& _mask);
    void UpdateCurrent(const UIResolvedStyle& _values, const UIStyleOverrideMask& _mask);
    void SetMaxEntries(int _max) { maxEntries_ = _max; }

    bool CanUndo() const { return undoPos_ > 0; }
    bool CanRedo() const { return redoPos_ > 0; }

    const UIThemeSnapshot& Undo();
    const UIThemeSnapshot& Redo();

    const UIThemeSnapshot& PeekUndo() const;
    const UIThemeSnapshot& PeekRedo() const;

    void TrimToMax();

private:
    jc::Vector<UIThemeSnapshot> undo_;
    jc::Vector<UIThemeSnapshot> redo_;
    int undoPos_ = 0;
    int redoPos_ = 0;
    size_t maxEntries_ = 80;
    bool hasCurrent_ = false;
};
