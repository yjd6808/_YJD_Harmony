#include "GameCoreHeader.h"
#include "sgcl/Game/UI/Theme/UITextureBakeService.h"
#include "sgcl/Game/UI/Theme/Baker/UIControlBakeRecipes.h"
#include "sgcl/Game/UI/Theme/Serialization/UITextureBinaryWriter.h"
#include "jc/IO/Path.h"

UITextureBakeService::UITextureBakeService()
{
}

UITextureBakeService::~UITextureBakeService()
{
    for (int idx = 0; idx < completedResults_.Size(); ++idx)
    {
        CC_SAFE_RELEASE(completedResults_[idx].textureSet);
    }
    completedResults_.Clear();
    pendingRequests_.Clear();
}

void UITextureBakeService::EnqueueRequest(UIThemeBakeRequest&& _request)
{
    if (_request.generation > latestGeneration_)
    {
        latestGeneration_ = _request.generation;
    }

    if (_request.generation > 0)
    {
        pendingRequests_.Clear();
    }

    pendingRequests_.PushBack(jc::Move(_request));
    hasPendingDebounce_ = true;
    debounceTimer_ = 0.0f;
}

void UITextureBakeService::CancelPending(uint64_t _generation)
{
    for (int idx = pendingRequests_.Size() - 1; idx >= 0; --idx)
    {
        if (pendingRequests_[idx].generation < _generation)
        {
            pendingRequests_.RemoveAt(idx);
        }
    }

    if (pendingRequests_.IsEmpty())
    {
        hasPendingDebounce_ = false;
        debounceTimer_ = 0.0f;
    }
}

void UITextureBakeService::Update(float _dt)
{
    if (!hasPendingDebounce_ || pendingRequests_.IsEmpty())
        return;

    debounceTimer_ += _dt;
    if (debounceTimer_ >= debounceTime_)
    {
        hasPendingDebounce_ = false;
        debounceTimer_ = 0.0f;

        UIThemeBakeRequest request = pendingRequests_.Back();
        pendingRequests_.Clear();
        ExecuteBake(request);
    }
}

void UITextureBakeService::ProcessCompleted()
{
    if (completedResults_.IsEmpty())
        return;

    for (int idx = 0; idx < completedResults_.Size(); ++idx)
    {
        PendingResult& result = completedResults_[idx];
        if (onBakeCompleted_)
        {
            onBakeCompleted_(result.textureSet);
        }

        CC_SAFE_RELEASE(result.textureSet);
    }

    completedResults_.Clear();
    isBaking_ = false;
}

void UITextureBakeService::ExecuteBake(const UIThemeBakeRequest& _request)
{
    isBaking_ = true;

    UITextureSet* set = BuildTextureSet(_request);

    PendingResult result;
    result.generation = _request.generation;
    result.textureSet = set;

    completedResults_.PushBack(result);
}

static UIAssetRecipe SelectRecipe(UIAssetSemantic _semantic)
{
    switch (_semantic)
    {
    case UIAssetSemantic::Button:        return UIControlBakeRecipes::ButtonRecipe(160, 44);
    case UIAssetSemantic::CheckMark:     return UIControlBakeRecipes::CheckMarkRecipe(12);
    case UIAssetSemantic::RadioDot:      return UIControlBakeRecipes::RadioDotRecipe(14);
    case UIAssetSemantic::CheckBox:      return UIControlBakeRecipes::CheckBoxShellRecipe(32);
    case UIAssetSemantic::ToggleTrack:   return UIControlBakeRecipes::ToggleTrackRecipe(54, 28);
    case UIAssetSemantic::ToggleKnob:    return UIControlBakeRecipes::ToggleKnobRecipe(24);
    case UIAssetSemantic::ProgressTrack: return UIControlBakeRecipes::ProgressTrackRecipe(96, 20);
    case UIAssetSemantic::ProgressGauge: return UIControlBakeRecipes::ProgressGaugeRecipe(96, 12);
    case UIAssetSemantic::ProgressCap:   return UIControlBakeRecipes::ProgressCapRecipe(14);
    case UIAssetSemantic::Frame:         return UIControlBakeRecipes::FrameRecipe(80, 80);
    case UIAssetSemantic::Radio:         return UIControlBakeRecipes::RadioShellRecipe(32);
    case UIAssetSemantic::SliderTrack:   return UIControlBakeRecipes::SliderTrackRecipe(96, 12);
    case UIAssetSemantic::SliderFill:    return UIControlBakeRecipes::SliderFillRecipe(96, 12);
    case UIAssetSemantic::SliderThumb:   return UIControlBakeRecipes::SliderThumbRecipe(24);
    case UIAssetSemantic::ScrollBarTrack: return UIControlBakeRecipes::ScrollBarTrackRecipe(16, 120);
    case UIAssetSemantic::ScrollBarThumb: return UIControlBakeRecipes::ScrollBarThumbRecipe(24);
    case UIAssetSemantic::EditBox:        return UIControlBakeRecipes::EditBoxRecipe(160, 44);
    case UIAssetSemantic::WindowIconMinimize:
    case UIAssetSemantic::WindowIconMaximize:
    case UIAssetSemantic::WindowIconClose:    return UIControlBakeRecipes::WindowIconRecipe(_semantic, 24);
    default: return UIControlBakeRecipes::ButtonRecipe(160, 44);
    }
}

UITextureSet* UITextureBakeService::BuildTextureSet(const UIThemeBakeRequest& _request)
{
    auto* set = dbg_new UITextureSet(_request.generation, 0);
    set->autorelease();
    set->retain();

    UIBakeOptions options;
    options.supersample = 4;
    options.premultipliedAlpha = true;

    for (int vi = 0; vi < _request.variants.Size(); ++vi)
    {
        const UIResolvedVariantRequest& variant = _request.variants[vi];
        UIAssetRecipe recipe = SelectRecipe(variant.asset.semantic);

        // 아이콘 레시피는 디렉토리와 조합해 절대 경로를 만든다.
        if (!recipe.svgPath.IsEmpty() && !iconDirectory_.IsEmpty())
        {
            recipe.svgPath = jc::Path::Combine(iconDirectory_.Source(), recipe.svgPath.Source());
        }

        UITextureCacheKey cacheKey;
        cacheKey.resolvedStyleHash = variant.styleHash;
        cacheKey.recipeHash = variant.asset.recipeHash;
        cacheKey.stateHash = (uint64_t)variant.state;

        UITextureEntry* cached = cache_.Find(cacheKey);
        if (cached)
        {
            set->AddEntry(variant.asset, cached);
            continue;
        }

        UIBakeOutput output = UITextureBaker::Bake(recipe, variant.style, options);

        auto* tex = dbg_new cc::Texture2D();
        if (tex->initWithData(output.buffer.pixels.Source(),
            output.buffer.pixels.Size(),
            cc::Texture2D::PixelFormat::RGBA8888,
            output.buffer.width, output.buffer.height,
            cc::Size(output.buffer.width, output.buffer.height)))
        {
            tex->autorelease();

            auto* entry = dbg_new UITextureEntry();
            entry->key = variant.asset;
            entry->SetTexture(tex);
            entry->atlasRect = { 0, 0, (float)output.buffer.width, (float)output.buffer.height };
            entry->sliceInsets = output.sliceInsets;
            entry->minimumSize = output.minimumSize;
            entry->contentPadding = output.contentPadding;
            entry->contentRect = { 0, 0, (float)output.buffer.width, (float)output.buffer.height };
            entry->premultipliedAlpha = true;

            set->AddEntry(variant.asset, entry);
            cache_.Insert(cacheKey, entry);
            CC_SAFE_RELEASE(entry);

            if (output.buffer.pixels.Size() >= 4)
            {
                const uint8_t* p = (const uint8_t*)output.buffer.pixels.Source();
                _LogDebug_("[Bake] semantic=%d w=%d h=%d pixel0=(%d,%d,%d,%d)",
                    (int)variant.asset.semantic,
                    output.buffer.width, output.buffer.height,
                    p[0], p[1], p[2], p[3]);

                int stride = output.buffer.width * 4;
                int nonZero = 0;
                for (int y = 0; y < output.buffer.height; ++y)
                {
                    for (int x = 0; x < output.buffer.width; ++x)
                    {
                        if (p[y * stride + x * 4 + 3] > 0)
                        {
                            ++nonZero;
                        }
                    }
                }
                _LogDebug_("[Bake] semantic=%d nonZeroAlpha=%d / %d",
                    (int)variant.asset.semantic, nonZero, output.buffer.width * output.buffer.height);

                int cx = output.buffer.width / 2;
                int cy = output.buffer.height / 2;
                int ci = cy * stride + cx * 4;
                _LogDebug_("[Bake] semantic=%d center(%d,%d)=(%d,%d,%d,%d)",
                    (int)variant.asset.semantic, cx, cy,
                    p[ci], p[ci+1], p[ci+2], p[ci+3]);
            }
        }
        else
        {
            JC_DELETE_SAFE(tex);
        }
    }

    return set;
}
