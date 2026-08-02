/*
 * 작성자: 윤정도
 * =====================
 */

#include "GameCoreHeader.h"
#include "sgcl/Game/UI/Core/BrushVisual.h"
#include "sgcl/Game/UI/Theme/UIThemeManager.h"

USING_NS_CC;
USING_NS_CCUI;

namespace sgui
{

BrushVisual::~BrushVisual()
{
	Detach();
}

void BrushVisual::Attach(cc::Node* _pHost, int _zOrder /* = -1 */)
{
	pHost_ = _pHost;
	zOrder_ = _zOrder;
	Rebuild();
}

void BrushVisual::Detach()
{
	binding_.Clear();

	if (pNode_)
	{
		pNode_->removeFromParent();
		pNode_ = nullptr;
	}

	pHost_ = nullptr;
}

void BrushVisual::SetBrush(const BrushPtr& _brush)
{
	brush_ = _brush;
	Rebuild();
}

void BrushVisual::SetVisualState(UIVisualState _state)
{
	if (state_ == _state)
	{
		return;
	}

	state_ = _state;

	if (pNode_ && brush_ && brush_->GetType() == BrushType::Theme)
	{
		ApplyThemeBinding();
	}
}

void BrushVisual::Resize(const cc::size& _size)
{
	size_ = _size;
	ApplySize();
}

void BrushVisual::RefreshTheme()
{
	if (pNode_ && brush_ && brush_->GetType() == BrushType::Theme)
	{
		ApplyThemeBinding();
	}
}

void BrushVisual::Rebuild()
{
	binding_.Clear();

	if (pNode_)
	{
		pNode_->removeFromParent();
		pNode_ = nullptr;
	}

	if (pHost_ == nullptr || brush_ == nullptr)
	{
		return;
	}

	switch (brush_->GetType())
	{
	case BrushType::SolidColor:
	{
		auto* pBrush = static_cast<SolidColorBrush*>(brush_.get());
		auto* pLayer = LayerColor::create(ToColor4B(pBrush->color_), size_.width, size_.height);
		pNode_ = pLayer;
		break;
	}
	case BrushType::LinearGradient:
	{
		auto* pBrush = static_cast<LinearGradientBrush*>(brush_.get());
		auto* pLayer = LayerGradient::create(
			ToColor4B(pBrush->start_), ToColor4B(pBrush->end_),
			pBrush->vertical_ ? cc::vec2(0.0f, -1.0f) : cc::vec2(1.0f, 0.0f));
		pLayer->setContentSize(size_);
		pNode_ = pLayer;
		break;
	}
	case BrushType::Theme:
	{
		auto* pBrush = static_cast<ThemeBrush*>(brush_.get());

		if (pBrush->fixedSize_)
		{
			auto* pSprite = Sprite::create();
			pSprite->setAnchorPoint(cc::vec2(0.5f, 0.5f));
			pNode_ = pSprite;
		}
		else if (flatFallback_)
		{
			// 9-slice 인셋(모서리) 크기보다 작은 컨트롤은 Scale9 모서리들이
			// 면적 대부분을 차지해 테마 쉐이딩이 4분할로 보이는 문제가 있다.
			// 이런 경우 테마 표면 색의 평면 필러로 대체한다.
			auto* pLayer = LayerColor::create(ToColor4B(flatColor_), size_.width, size_.height);
			pNode_ = pLayer;
		}
		else
		{
			auto* pSprite = Scale9Sprite::create();
			pSprite->setAnchorPoint(cc::vec2::ZERO);
			pNode_ = pSprite;
		}
		break;
	}
	}

	if (pNode_ == nullptr)
	{
		return;
	}

	pHost_->addChild(pNode_, zOrder_);

	if (brush_->GetType() == BrushType::Theme)
	{
		ApplyThemeBinding();
	}

	ApplySize();
}

void BrushVisual::ApplyThemeBinding()
{
	auto* pBrush = static_cast<ThemeBrush*>(brush_.get());
	UIThemeManager* pThemeMgr = UIThemeManager::Get();

	UIResolvedStyle resolved = pThemeMgr->Resolve(pBrush->controlType_, state_, {});
	const uint64_t styleHash = resolved.ComputeHash();
	const UIAssetKey key = UIAssetKey::For(pBrush->semantic_, styleHash, state_);

	flatColor_ = resolved.surfaceTop;

	binding_.Clear();

	if (pBrush->fixedSize_)
	{
		if (auto* pSprite = dynamic_cast<Sprite*>(pNode_))
		{
			binding_.BindFixed(pSprite, key, pBrush->slot_);
		}
	}
	else
	{
		if (auto* pScale9 = dynamic_cast<cc_ui::Scale9Sprite*>(pNode_))
		{
			binding_.BindScale9(pScale9, key, pBrush->slot_);
		}
	}

	if (const UITextureSet* pSet = pThemeMgr->GetActiveTextureSet())
	{
		binding_.Refresh(*pSet);
	}
	else
	{
		_LogWarn_("[BrushVisual] GetActiveTextureSet()가 null입니다.");
	}

	if (auto* pScale9 = dynamic_cast<cc_ui::Scale9Sprite*>(pNode_))
	{
		// 실제 9-slice 최소 크기를 기억한다. (현재 노드가 LayerColor여도
		// 이전 Scale9 인셋 정보를 유지해야 크기 확대 시 Scale9로 복귀할 수 있다)
		sliceMinWidth_ = pScale9->getInsetLeft() + pScale9->getInsetRight();
		sliceMinHeight_ = pScale9->getInsetTop() + pScale9->getInsetBottom();
	}

	if (auto* pLayer = dynamic_cast<LayerColor*>(pNode_))
	{
		const cc::Color4B color = ToColor4B(flatColor_);
		pLayer->setColor(cc::Color3B(color));
		pLayer->setOpacity(color.a);
	}

	ApplySize();
	SyncNodeKind();
}

void BrushVisual::ApplySize()
{
	if (pNode_ == nullptr)
	{
		return;
	}

	const bool fixedSize = brush_ && brush_->GetType() == BrushType::Theme && static_cast<ThemeBrush*>(brush_.get())->fixedSize_;

	if (fixedSize)
	{
		// 고정 크기 스프라이트는 호스트 중앙에 배치한다. (호스트 크기를 아직 모르면 자기 크기 기준)
		const float hostW = pHost_ ? pHost_->getContentSize().width : 0.0f;
		const float hostH = pHost_ ? pHost_->getContentSize().height : 0.0f;
		pNode_->setPosition(
			(hostW > 0.0f ? hostW : size_.width) * 0.5f,
			(hostH > 0.0f ? hostH : size_.height) * 0.5f);
	}
	else
	{
		pNode_->setContentSize(size_);
	}

	SyncNodeKind();
}

bool BrushVisual::ComputeFlatFallback() const
{
	if (!brush_ || brush_->GetType() != BrushType::Theme)
	{
		return false;
	}

	auto* pBrush = static_cast<ThemeBrush*>(brush_.get());

	if (pBrush->fixedSize_)
	{
		return false;
	}

	// Scale9 최소 인셋 크기가 아직 확보되지 않았으면(텍스처 미바인딩 등) Scale9 유지
	if (sliceMinWidth_ <= 0.0f && sliceMinHeight_ <= 0.0f)
	{
		return false;
	}

	return size_.width < sliceMinWidth_ || size_.height < sliceMinHeight_;
}

void BrushVisual::SyncNodeKind()
{
	if (!brush_ || brush_->GetType() != BrushType::Theme)
	{
		return;
	}

	const bool wantsFlat = ComputeFlatFallback();

	if (wantsFlat != flatFallback_)
	{
		flatFallback_ = wantsFlat;
		Rebuild();
	}
}

} // namespace sgui
