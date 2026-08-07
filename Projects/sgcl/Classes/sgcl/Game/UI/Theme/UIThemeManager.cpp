#include "GameCoreHeader.h"
#include "sgcl/Game/UI/Theme/UIThemeManager.h"
#include "sg/Core/AppConfig.h"
#include "jc/FileSystem/Path.h"

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
    colorTable_.BuildDefaults(activeTheme_);
    revision_.sourceRevision = 1;
    revision_.mappedRevision = 1;
    revision_.textureRevision = 1;

    // SVG 아이콘 소스 디렉토리 (resDataPath/gfx/mdi)
    bakeService_.SetIconDirectory(jc::Path::Combine(g_cAppConfig.resDataPath_, "gfx/mdi").Source());

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
    deferredRelease_.Flush();
}

void UIThemeManager::Free()
{
    if (g_pThemeManager)
    {
        g_pThemeManager->Shutdown();
        JC_DELETE_SAFE(g_pThemeManager);
        g_pThemeManager = nullptr;
    }
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

bool UIThemeManager::ApplyTheme(const char* _jsonPath, UIColorScheme _scheme)
{
    if (!_jsonPath || !_jsonPath[0])
        return false;

    std::string jsonContent = cc::FileUtils::getInstance()->getStringFromFile(_jsonPath);
    if (jsonContent.empty())
    {
        _LogError_("[UIThemeManager] ApplyTheme 파일을 읽지 못했습니다: %s", _jsonPath);
        return false;
    }

    Json::Value root;
    Json::Reader reader;
    if (!reader.parse(jsonContent, root))
    {
        _LogError_("[UIThemeManager] ApplyTheme JSON 파싱 실패: %s (%s)", _jsonPath, reader.getFormattedErrorMessages().c_str());
        return false;
    }

    UIRuntimeTheme theme = UIThemeMapper::Map(root, _scheme);

    activeTheme_ = theme;

    // 컨트롤/상태별 단색 테이블 재구축: 토큰 기반 기본값 → JSON "controls" 섹션 오버라이드
    colorTable_.BuildDefaults(activeTheme_);
    colorTable_.LoadJson(root);

    activeScheme_ = _scheme;
    activeThemeName_ = theme.meta.displayName.IsEmpty() ? jc::String("unnamed") : theme.meta.displayName;
    activeThemeJsonPath_ = jc::String(_jsonPath);
    ++revision_.mappedRevision;

    UIThemeBakeRequest request;
    request.generation = revision_.sourceRevision + 1;
    request.theme = activeTheme_;
    request.dpiScale = 1.0f;
    request.resolvedScheme = _scheme;
    request.preview = false;
    request.persistCache = true;
    BuildThemeVariants(request);

    bakeService_.EnqueueRequest(jc::Move(request));

    _LogDebug_("[UIThemeManager] ApplyTheme 전환 요청: theme=%s scheme=%d json=%s revision=(%llu,%llu,%llu)",
        activeThemeName_.Source(), (int)_scheme, _jsonPath,
        revision_.sourceRevision, revision_.mappedRevision, revision_.textureRevision);
    return true;
}

void UIThemeManager::BuildThemeVariants(UIThemeBakeRequest& _request) const
{
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
        UIAssetSemantic::ScrollBarTrack,
        UIAssetSemantic::ScrollBarThumb,
        UIAssetSemantic::EditBox,
        UIAssetSemantic::WindowIconMinimize,
        UIAssetSemantic::WindowIconMaximize,
        UIAssetSemantic::WindowIconClose
    };

    // 상태별 variant 베이크 (스타일 해시가 상태별로 달라 텍스처 키가 자동 분리된다)
    static const UIVisualState kStates[] = {
        UIVisualState::Normal,
        UIVisualState::Hover,
        UIVisualState::Pressed,
        UIVisualState::Disabled
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
        case UIAssetSemantic::ScrollBarThumb: elemType = UIElementType::ScrollBar; break;
        case UIAssetSemantic::EditBox:        elemType = UIElementType::EditBox; break;
        case UIAssetSemantic::WindowIconMinimize:
        case UIAssetSemantic::WindowIconMaximize:
        case UIAssetSemantic::WindowIconClose: elemType = UIElementType::Button; break;
        default: break;
        }

        for (UIVisualState state : kStates)
        {
            UIResolvedStyle resolvedStyle = resolver_.Resolve(elemType, state, _request.theme, {});

            UIResolvedVariantRequest variant;
            variant.asset.semantic = semantic;
            variant.asset.state = state;
            variant.asset.styleHash = resolvedStyle.ComputeHash();
            variant.asset.recipeHash = (uint64_t)semantic;
            variant.state = state;
            variant.style = resolvedStyle;
            variant.styleHash = resolvedStyle.ComputeHash();
            _request.variants.PushBack(variant);
        }
    }
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
    BuildThemeVariants(request);

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
    colorTable_.BuildDefaults(activeTheme_);
    ++revision_.mappedRevision;
    editSession_ = UIThemeEditSession{};
}

void UIThemeManager::CommitDraft(const UIRuntimeTheme& _draft, const UIStyleOverride& _overrides)
{
    activeTheme_ = _draft;
    colorTable_.BuildDefaults(activeTheme_);
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
    {
        deferredRelease_.ReleaseAfterFrames(activeTextureSet_, 2);
    }

    activeTextureSet_ = pendingTextureSet_;
    pendingTextureSet_ = nullptr;

    ++revision_.textureRevision;
    _LogDebug_("[UIThemeManager] SwapTextureSet textureRevision=%llu (theme=%s scheme=%d)",
        revision_.textureRevision, activeThemeName_.Source(), (int)activeScheme_);

    if (onThemeRefreshed_)
    {
        onThemeRefreshed_();
    }
}
