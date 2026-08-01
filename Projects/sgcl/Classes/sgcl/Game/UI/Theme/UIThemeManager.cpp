#include "GameCoreHeader.h"
#include "sgcl/Game/UI/Theme/UIThemeManager.h"

static UIThemeManager* g_pThemeManager = nullptr;

UIThemeManager* UIThemeManager::Get()
{
    if (!g_pThemeManager)
    {
        g_pThemeManager = dbg_new UIThemeManager();
    }
    return g_pThemeManager;
}

bool UIThemeManager::Initialize(const UIThemeInitParams& _params)
{
    params_ = _params;
    activeTheme_ = UIRuntimeTheme::EngineDefaults();
    revision_.sourceRevision = 1;
    revision_.mappedRevision = 1;
    revision_.textureRevision = 1;

    bakeService_.SetOnBakeCompleted([this](UITextureSet* _set) {
        OnBakeCompleted(_set);
    });

    _LogDebug_("[UIThemeManager] Initialize complete");

    return true;
}

void UIThemeManager::Shutdown()
{
    editSession_ = UIThemeEditSession{};
    CC_SAFE_RELEASE(activeTextureSet_);
    CC_SAFE_RELEASE(pendingTextureSet_);
    bakeService_.CancelPending(0);
    deferredRelease_.Update();
}

void UIThemeManager::Update(float _dt)
{
    ++frameCount_;

    bakeService_.Update(_dt);
    bakeService_.ProcessCompleted();
    deferredRelease_.Update();
    SwapTextureSet();
}

UIThemeRevision UIThemeManager::IncrementSourceRevision()
{
    ++revision_.sourceRevision;
    return revision_;
}

UIResolvedStyle UIThemeManager::Resolve(
    UIElementType_t _control,
    UIVisualState _state,
    const UIStyleOverride& _local) const
{
    return resolver_.Resolve(_control, _state, activeTheme_, _local);
}

UIResolvedStyle UIThemeManager::ResolveWithEditOverrides(
    UIElementType_t _control,
    UIVisualState _state) const
{
    if (editSession_.active)
        return resolver_.Resolve(_control, _state, editSession_.draft, editSession_.overrides);
    return resolver_.Resolve(_control, _state, activeTheme_, {});
}

void UIThemeManager::RequestBake(UIThemeBakeRequest&& _request)
{
    bakeService_.EnqueueRequest(jc::Move(_request));
}

void UIThemeManager::BakeDefaultTextureSet()
{
    if (activeTextureSet_)
        return;

    UIThemeBakeRequest request;
    request.generation = 1;
    request.theme = activeTheme_;
    request.dpiScale = 1.0f;
    request.resolvedScheme = UIColorScheme::Dark;
    request.preview = false;
    request.persistCache = true;

    UIAssetSemantic semantics[] = {
        UIAssetSemantic::Button,
        UIAssetSemantic::Frame,
        UIAssetSemantic::CheckBox,
        UIAssetSemantic::CheckMark,
        UIAssetSemantic::Radio,
        UIAssetSemantic::RadioDot,
        UIAssetSemantic::ToggleTrack,
        UIAssetSemantic::ToggleKnob,
        UIAssetSemantic::ProgressTrack,
        UIAssetSemantic::ProgressGauge,
        UIAssetSemantic::ProgressCap,
        UIAssetSemantic::SliderTrack,
        UIAssetSemantic::SliderFill,
        UIAssetSemantic::SliderThumb,
        UIAssetSemantic::ScrollBarTrack
    };

    for (auto semantic : semantics)
    {
        UIElementType_t elemType = UIElementType::Button;
        switch (semantic)
        {
        case UIAssetSemantic::Button:        elemType = UIElementType::Button; break;
        case UIAssetSemantic::Frame:         elemType = UIElementType::Group; break;
        case UIAssetSemantic::CheckBox:
        case UIAssetSemantic::CheckMark:     elemType = UIElementType::CheckBox; break;
        case UIAssetSemantic::Radio:
        case UIAssetSemantic::RadioDot:      elemType = UIElementType::CheckBox; break;
        case UIAssetSemantic::ToggleTrack:
        case UIAssetSemantic::ToggleKnob:    elemType = UIElementType::ToggleButton; break;
        case UIAssetSemantic::ProgressTrack:
        case UIAssetSemantic::ProgressGauge:
        case UIAssetSemantic::ProgressCap:  elemType = UIElementType::ProgressBar; break;
        case UIAssetSemantic::SliderTrack:
        case UIAssetSemantic::SliderFill:
        case UIAssetSemantic::SliderThumb:   elemType = UIElementType::Button; break;
        case UIAssetSemantic::ScrollBarTrack: elemType = UIElementType::ScrollBar; break;
        default: break;
        }

        UIResolvedStyle resolvedStyle = resolver_.Resolve(
            elemType, UIVisualState::Normal, activeTheme_, {});

        UIResolvedVariantRequest variant;
        variant.asset.semantic = semantic;
        variant.asset.styleHash = resolvedStyle.ComputeHash();
        variant.asset.recipeHash = (uint64_t)semantic;
        variant.state = UIVisualState::Normal;
        variant.style = resolvedStyle;
        variant.styleHash = resolvedStyle.ComputeHash();
        request.variants.PushBack(variant);
    }

    _LogDebug_("[BakeDefaultTextureSet] variants=%d, firstStyleHash=%llu", request.variants.Size(), request.variants.Size() > 0 ? request.variants[0].styleHash : 0);

    UITextureSet* set = bakeService_.BuildTextureSet(request);
    if (set)
    {
        CC_SAFE_RELEASE(activeTextureSet_);
        activeTextureSet_ = set;
        ++revision_.textureRevision;
        _LogDebug_("[BakeDefaultTextureSet] textureSet=%p entries=%d generation=%llu", set, set->GetEntryCount(), set->GetGeneration());
    }
    else
    {
        _LogWarn_("[BakeDefaultTextureSet] BuildTextureSet returned null!");
    }
}

UIThemeEditSession UIThemeManager::BeginEdit()
{
    UIThemeEditSession session;
    session.active = true;
    session.draft = activeTheme_;
    session.overrides = UIStyleOverride{};
    editSession_ = session;
    return editSession_;
}

void UIThemeManager::CommitDraft(const UIRuntimeTheme& _draft)
{
    activeTheme_ = _draft;
    ++revision_.mappedRevision;
    editSession_ = UIThemeEditSession{};
}

void UIThemeManager::CommitDraft(const UIRuntimeTheme& _draft, const UIStyleOverride& _overrides)
{
    activeTheme_ = _draft;
    editSession_.active = true;
    editSession_.draft = _draft;
    editSession_.overrides = _overrides;
    ++revision_.mappedRevision;
}

void UIThemeManager::CancelPreview()
{
    editSession_ = UIThemeEditSession{};
}

void UIThemeManager::OnBakeCompleted(UITextureSet* _set)
{
    CC_SAFE_RELEASE(pendingTextureSet_);
    pendingTextureSet_ = _set;
    CC_SAFE_RETAIN(pendingTextureSet_);
}

void UIThemeManager::SwapTextureSet()
{
    if (!pendingTextureSet_)
        return;

    if (activeTextureSet_)
        deferredRelease_.ReleaseAfterFrames(activeTextureSet_, 2);

    activeTextureSet_ = pendingTextureSet_;
    pendingTextureSet_ = nullptr;

    ++revision_.textureRevision;
}
