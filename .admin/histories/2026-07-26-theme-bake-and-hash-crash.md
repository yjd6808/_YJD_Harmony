# Theme Bake & Hash Crash Fix

**Date:** 2026-07-26  
**Branch:** `sg-main` (2121d564)  
**Build:** x64 Debug, 0 errors

---

## Problem

1. HashTable `Bucket::Expand()` crashed (`jc_assert(false)`) when 14 UI variant entries exceeded a bucket's capacity and triggered `Expand()`. Root cause: `PlacementNewArray(pNewDynamicArray, _newCapacity)` requires default-constructible `TBucketNode`, but `BucketNode` has no default constructor for non-pointer TValue types.

2. All UI theme semantics (CheckBox, ToggleTrack, etc.) baked at 160×44 (Button size) because:
   - `recipeHash = 0` for all entries → cache collision on same styleHash
   - `SelectRecipe` had no case for Frame, Radio, SliderFill, ScrollBarTrack → fell to ButtonRecipe default

3. Center pixel alpha was 81 instead of 255: `RenderSurface` Y-loop used `_rect.Bottom()` (=y) instead of `_rect.Top()` (=y+h), only rendering 2 rows.

4. Green debug sprite overlay in UIButton.cpp tinted all buttons green.

## Changes

| File | Change |
|------|--------|
| `HashTable_Bucket.h:418-430` | Remove `PlacementNewArray` + `if constexpr` assignment branch; use `PlacementNew` move-construction for all types |
| `UITextureBakeService.cpp:SelectRecipe` | Add cases for Frame(80×80), Radio(32×32), SliderFill(96×12), ScrollBarTrack(16×120) |
| `UIControlBakeRecipes.h/.cpp` | Add `SliderFillRecipe()`, `ScrollBarTrackRecipe()` |
| `UIThemeManager.cpp:BakeDefaultTextureSet` | Add all 14 semantics; set `recipeHash = (uint64_t)semantic` |
| `UIThemeManager.cpp` | Map elemType per semantic (Group, CheckBox, ToggleButton, ProgressBar, ScrollBar) |
| `UI_Generic.cpp` | Add click handlers for 6 teset buttons; add missing `return` after `close` handler |
| `UIButton.cpp` | Remove green debug `setColor(0,255,0)` |
| `UIGradientRasterizer.cpp:18` | `Bottom()` → `Top()` in Y-loop |
| `ui_generic.xml` | Add 6 themed buttons + CheckBox + EditBox |

## Verification

- 14 semantics bake with correct sizes (Button=160×44, CheckBox=32×32, ToggleTrack=54×28, CheckMark=18×18, etc.)
- Center pixels all have α=255
- No `jc_assert` crash on Expand()
- Build: 0 errors, 92 warnings (pre-existing)

## Related Files
- `Projects/jc/Sources/jc/Container/HashTable_Bucket.h`
- `Projects/sgcl/Classes/sgcl/Game/UI/Theme/UITextureBakeService.cpp`
- `Projects/sgcl/Classes/sgcl/Game/UI/Theme/Baker/UIControlBakeRecipes.cpp`
- `Projects/sgcl/Classes/sgcl/Game/UI/Theme/Baker/UIControlBakeRecipes.h`
- `Projects/sgcl/Classes/sgcl/Game/UI/Theme/UIThemeManager.cpp`
- `Projects/sgcl/Classes/sgcl/Game/UI/UIButton.cpp`
- `Projects/sgcl/Classes/sgcl/Game/UI/Theme/Baker/UIGradientRasterizer.cpp`
- `Projects/sgcl/Classes/sgcl/Game/UI_Implementation/UI_Generic.cpp`
- `C:\Users\jdyun\Desktop\UltimateHarmony_Dev\assets\res_data\layout\ui_generic.xml`
