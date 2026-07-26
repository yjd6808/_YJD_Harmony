#pragma once

#include "sgcl/Game/UI/Theme/UIThemeTypes.h"
#include "sgcl/Game/UI/Theme/UIRuntimeTheme.h"
#include "sgcl/Game/UI/Theme/UIStyleOverride.h"
#include "sgcl/Game/UI/Theme/UIStyleResolver.h"
#include "sgcl/Game/UI/Theme/UIThemeMapper.h"
#include "sgcl/Game/UI/Theme/UITextureEntry.h"
#include "sgcl/Game/UI/Theme/UITextureBakeService.h"
#include "sgcl/Game/UI/Theme/UITextureCache.h"

struct UIThemeInitParams
{
    jc::String cacheRoot;
};

struct UIThemeEditSession
{
    bool active = false;
    UIRuntimeTheme draft;
    UIStyleOverride overrides;
};

class UIThemeManager final
{
public:
    static UIThemeManager* Get();

    bool Initialize(const UIThemeInitParams& _params);
    void Shutdown();
    void Update(float _dt);

    UIThemeRevision GetRevision() const { return revision_; }
    const UIRuntimeTheme& GetActiveTheme() const { return activeTheme_; }
    const UITextureSet* GetActiveTextureSet() const { return activeTextureSet_; }

    UIThemeRevision IncrementSourceRevision();

    UIResolvedStyle Resolve(
        UIElementType_t _control,
        UIVisualState _state,
        const UIStyleOverride& _local) const;

    UIResolvedStyle ResolveWithEditOverrides(
        UIElementType_t _control,
        UIVisualState _state) const;

    void RequestBake(UIThemeBakeRequest&& _request);

    UIThemeEditSession BeginEdit();
    void CommitDraft(const UIRuntimeTheme& _draft);
    void CommitDraft(const UIRuntimeTheme& _draft, const UIStyleOverride& _overrides);
    void CancelPreview();

    UITextureCache& GetCache() { return bakeService_.GetCache(); }
    UITextureBakeService& GetBakeService() { return bakeService_; }

private:
    UIThemeManager() = default;
    ~UIThemeManager() = default;
    UIThemeManager(const UIThemeManager&) = delete;
    UIThemeManager& operator=(const UIThemeManager&) = delete;

    void OnBakeCompleted(UITextureSet* _set);
    void SwapTextureSet();

    UIThemeRevision revision_;
    UIRuntimeTheme activeTheme_;
    UIStyleResolver resolver_;
    UIThemeMapper mapper_;
    UIThemeEditSession editSession_;
    UIThemeInitParams params_;

    UITextureBakeService bakeService_;
    UITextureSet* activeTextureSet_ = nullptr;
    UITextureSet* pendingTextureSet_ = nullptr;
    UITextureDeferredRelease deferredRelease_;
    int frameCount_ = 0;
};
