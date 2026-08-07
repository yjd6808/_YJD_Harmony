#pragma once

#include "sgcl/Game/UI/Theme/UIThemeTypes.h"
#include "sgcl/Game/UI/Theme/UIRuntimeTheme.h"
#include "sgcl/Game/UI/Theme/UIStyleOverride.h"
#include "sgcl/Game/UI/Theme/UIStyleResolver.h"
#include "sgcl/Game/UI/Theme/UIThemeMapper.h"
#include "sgcl/Game/UI/Theme/UIThemeColorTable.h"
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
    static void Free();

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

    // 컨트롤/상태별 테마 색상 조회 (WPF 스타일 단색 시스템)
    // 예) UIThemeManager::Get()->GetColor(UIThemeColor::ButtonHoverBackground)
    const UIThemeColorTable& GetColors() const { return colorTable_; }
    UIColorF GetColor(UIThemeColor _color) const { return colorTable_.Get(_color); }
    UIColorF GetColor(UIThemeControl _control, UIVisualState _state, UIThemeColorRole _role) const { return colorTable_.Get(_control, _state, _role); }

    void RequestBake(UIThemeBakeRequest&& _request);
    void BakeDefaultTextureSet();

    bool ApplyTheme(const char* _jsonPath, UIColorScheme _scheme);
    UIColorScheme GetActiveScheme() const { return activeScheme_; }
    const char* GetActiveThemeName() const { return activeThemeName_.Source(); }
    const char* GetActiveThemeJsonPath() const { return activeThemeJsonPath_.Source(); }

    void SetOnThemeRefreshed(jc::Action<> _callback) { onThemeRefreshed_ = _callback; }

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
    void BuildThemeVariants(UIThemeBakeRequest& _request) const;

    UIThemeRevision revision_;
    UIRuntimeTheme activeTheme_;
    UIThemeColorTable colorTable_;
    UIStyleResolver resolver_;
    UIThemeMapper mapper_;
    UIThemeEditSession editSession_;
    UIThemeInitParams params_;

    UIColorScheme activeScheme_ = UIColorScheme::Dark;
    jc::String activeThemeName_;
    jc::String activeThemeJsonPath_;
    jc::Action<> onThemeRefreshed_;

    UITextureBakeService bakeService_;
    UITextureSet* activeTextureSet_ = nullptr;
    UITextureSet* pendingTextureSet_ = nullptr;
    UITextureDeferredRelease deferredRelease_;
    int frameCount_ = 0;
};
