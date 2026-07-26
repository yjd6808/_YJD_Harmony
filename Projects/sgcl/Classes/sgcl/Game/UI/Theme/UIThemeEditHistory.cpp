#include "GameCoreHeader.h"
#include "sgcl/Game/UI/Theme/UIThemeEditHistory.h"

void UIThemeEditHistory::Clear()
{
    undo_.Clear();
    redo_.Clear();
    undoPos_ = 0;
    redoPos_ = 0;
    hasCurrent_ = false;
}

void UIThemeEditHistory::Checkpoint(const UIResolvedStyle& _values, const UIStyleOverrideMask& _mask)
{
    if (hasCurrent_)
    {
        undo_[undoPos_].values = _values;
        undo_[undoPos_].mask = _mask;
        undoPos_ = undo_.Size() - 1;
    }
    else
    {
        UIThemeSnapshot snap;
        snap.values = _values;
        snap.mask = _mask;
        undo_.PushBack(snap);
        undoPos_ = undo_.Size() - 1;
        hasCurrent_ = true;
    }

    redo_.Clear();
    redoPos_ = 0;
    TrimToMax();
}

void UIThemeEditHistory::UpdateCurrent(const UIResolvedStyle& _values, const UIStyleOverrideMask& _mask)
{
    if (!hasCurrent_ || undo_.IsEmpty())
    {
        Checkpoint(_values, _mask);
        return;
    }

    undo_[undoPos_].values = _values;
    undo_[undoPos_].mask = _mask;
}

const UIThemeSnapshot& UIThemeEditHistory::Undo()
{
    if (!CanUndo())
    {
        static UIThemeSnapshot empty;
        return empty;
    }

    const UIThemeSnapshot& current = undo_[undoPos_];
    redo_.PushBack(current);
    ++redoPos_;

    --undoPos_;
    if (undoPos_ < 0)
    {
        undoPos_ = 0;
        static UIThemeSnapshot first;
        first = undo_[0];
        return first;
    }

    return undo_[undoPos_];
}

const UIThemeSnapshot& UIThemeEditHistory::Redo()
{
    if (!CanRedo())
    {
        static UIThemeSnapshot empty;
        return empty;
    }

    --redoPos_;
    const UIThemeSnapshot& snap = redo_[redoPos_];
    undo_.PushBack(snap);
    ++undoPos_;

    return snap;
}

const UIThemeSnapshot& UIThemeEditHistory::PeekUndo() const
{
    if (!CanUndo())
    {
        static UIThemeSnapshot empty;
        return empty;
    }
    return undo_[undoPos_];
}

const UIThemeSnapshot& UIThemeEditHistory::PeekRedo() const
{
    if (!CanRedo())
    {
        static UIThemeSnapshot empty;
        return empty;
    }
    return redo_[redoPos_ - 1];
}

void UIThemeEditHistory::TrimToMax()
{
    while (undo_.Size() > maxEntries_)
    {
        undo_.RemoveAt(0);
        --undoPos_;
    }
    while (redo_.Size() > maxEntries_)
    {
        redo_.RemoveAt(0);
        --redoPos_;
    }
}
