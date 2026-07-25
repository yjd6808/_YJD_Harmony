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

void UIThemeManager::RequestBake(UIThemeBakeRequest&& _request)
{
    bakeService_.EnqueueRequest(jc::Move(_request));
}

UIThemeEditSession UIThemeManager::BeginEdit()
{
    UIThemeEditSession session;
    session.active = true;
    session.draft = activeTheme_;
    editSession_ = session;
    return editSession_;
}

void UIThemeManager::CommitDraft(const UIRuntimeTheme& _draft)
{
    activeTheme_ = _draft;
    ++revision_.mappedRevision;
    editSession_ = UIThemeEditSession{};
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
