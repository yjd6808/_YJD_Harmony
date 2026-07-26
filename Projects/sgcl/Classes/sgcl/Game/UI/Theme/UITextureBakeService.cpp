#include "GameCoreHeader.h"
#include "sgcl/Game/UI/Theme/UITextureBakeService.h"
#include "sgcl/Game/UI/Theme/Baker/UIControlBakeRecipes.h"
#include "sgcl/Game/UI/Theme/Serialization/UITextureBinaryWriter.h"

UITextureBakeService::UITextureBakeService()
{
}

UITextureBakeService::~UITextureBakeService()
{
    for (int i = 0; i < completedResults_.Size(); ++i)
        CC_SAFE_RELEASE(completedResults_[i].textureSet);
    completedResults_.Clear();
    pendingRequests_.Clear();
}

void UITextureBakeService::EnqueueRequest(UIThemeBakeRequest&& _request)
{
    if (_request.generation > latestGeneration_)
        latestGeneration_ = _request.generation;

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
    for (int i = pendingRequests_.Size() - 1; i >= 0; --i)
    {
        if (pendingRequests_[i].generation < _generation)
            pendingRequests_.RemoveAt(i);
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

    for (int i = 0; i < completedResults_.Size(); ++i)
    {
        PendingResult& result = completedResults_[i];
        if (onBakeCompleted_)
            onBakeCompleted_(result.textureSet);

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
    case UIAssetSemantic::Button:        return UIControlBakeRecipes::ButtonRecipe(64, 32);
    case UIAssetSemantic::CheckMark:     return UIControlBakeRecipes::CheckMarkRecipe(18);
    case UIAssetSemantic::RadioDot:      return UIControlBakeRecipes::RadioDotRecipe(14);
    case UIAssetSemantic::CheckBox:      return UIControlBakeRecipes::CheckBoxShellRecipe(32);
    case UIAssetSemantic::ToggleTrack:   return UIControlBakeRecipes::ToggleTrackRecipe(54, 28);
    case UIAssetSemantic::ToggleKnob:    return UIControlBakeRecipes::ToggleKnobRecipe(24);
    case UIAssetSemantic::ProgressTrack: return UIControlBakeRecipes::ProgressTrackRecipe(96, 20);
    case UIAssetSemantic::ProgressGauge: return UIControlBakeRecipes::ProgressGaugeRecipe(96, 12);
    case UIAssetSemantic::SliderTrack:   return UIControlBakeRecipes::SliderTrackRecipe(96, 12);
    case UIAssetSemantic::SliderThumb:   return UIControlBakeRecipes::SliderThumbRecipe(24);
    default: return UIControlBakeRecipes::ButtonRecipe(64, 32);
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
            tex->retain();

            auto* entry = dbg_new UITextureEntry();
            entry->key = variant.asset;
            entry->SetTexture(tex);
            entry->atlasRect = { 0, 0, (float)output.buffer.width, (float)output.buffer.height };
            entry->sliceInsets = output.sliceInsets;
            entry->minimumSize = output.minimumSize;
            entry->contentRect = { 0, 0, (float)output.buffer.width, (float)output.buffer.height };
            entry->premultipliedAlpha = true;

            set->AddEntry(variant.asset, entry);
            cache_.Insert(cacheKey, entry);
        }
        else
        {
            JC_DELETE_SAFE(tex);
        }
    }

    return set;
}
