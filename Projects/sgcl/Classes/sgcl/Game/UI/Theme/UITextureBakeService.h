#pragma once

#include "sgcl/Game/UI/Theme/UIThemeTypes.h"
#include "sgcl/Game/UI/Theme/UITextureEntry.h"
#include "sgcl/Game/UI/Theme/UIStyleOverride.h"
#include "sgcl/Game/UI/Theme/Baker/UITextureBaker.h"
#include "sgcl/Game/UI/Theme/UITextureCache.h"

struct UIResolvedVariantRequest
{
    UIAssetKey asset;
    UIVisualState state;
    UIResolvedStyle style;
    uint64_t styleHash = 0;
};

struct UIThemeBakeRequest
{
    uint64_t generation = 0;
    UIRuntimeTheme theme;
    UIColorScheme resolvedScheme = UIColorScheme::Dark;
    float dpiScale = 1.0f;
    bool preview = true;
    bool persistCache = false;

    jc::Vector<UIResolvedVariantRequest> variants;
};

class UITextureBakeService
{
public:
    using BakeCompletedCallback = jc::Action<UITextureSet*>;

    UITextureBakeService();
    ~UITextureBakeService();

    void EnqueueRequest(UIThemeBakeRequest&& _request);
    void CancelPending(uint64_t _generation);
    void Update(float _dt);
    void ProcessCompleted();

    void SetDebounceTime(float _seconds) { debounceTime_ = _seconds; }
    void SetOnBakeCompleted(BakeCompletedCallback _callback)
    {
        onBakeCompleted_ = _callback;
    }

    // SVG 아이콘 소스 디렉토리 (예: resDataPath/gfx/mdi)
    void SetIconDirectory(const char* _dir) { iconDirectory_ = _dir; }
    const jc::String& GetIconDirectory() const { return iconDirectory_; }

    UITextureCache& GetCache() { return cache_; }

private:
    void ExecuteBake(const UIThemeBakeRequest& _request);

public:
    UITextureSet* BuildTextureSet(const UIThemeBakeRequest& _request);

    struct PendingResult
    {
        uint64_t generation;
        UITextureSet* textureSet;
    };

    UITextureCache cache_;
    jc::String iconDirectory_;
    jc::Vector<UIThemeBakeRequest> pendingRequests_;
    jc::Vector<PendingResult> completedResults_;
    uint64_t latestGeneration_ = 0;
    bool isBaking_ = false;

    float debounceTime_ = 0.15f;
    float debounceTimer_ = 0.0f;
    bool hasPendingDebounce_ = false;

    BakeCompletedCallback onBakeCompleted_;
};
