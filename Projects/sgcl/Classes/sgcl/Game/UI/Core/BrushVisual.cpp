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

	if (pNode_ == nullptr || brush_ == nullptr)
	{
		return;
	}

	if (brush_->GetType() == BrushType::Theme)
	{
		ApplyThemeBinding();
	}
	else if (brush_->GetType() == BrushType::ThemeColor)
	{
		ApplyThemeColorTint();
	}
}

void BrushVisual::Resize(const cc::size& _size)
{
	size_ = _size;
	ApplySize();
}

void BrushVisual::RefreshTheme()
{
	if (pNode_ == nullptr || brush_ == nullptr)
	{
		return;
	}

	if (brush_->GetType() == BrushType::Theme)
	{
		ApplyThemeBinding();
	}
	else if (brush_->GetType() == BrushType::ThemeColor)
	{
		ApplyThemeColorTint();
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
	case BrushType::ThemeColor:
	{
		// 현재 테마의 컨트롤/상태별 단색 플레이트
		const UIColorF color = ResolveBrushColor(brush_.get(), state_);
		auto* pLayer = LayerColor::create(ToColor4B(color), size_.width, size_.height);
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

void BrushVisual::ApplyThemeColorTint()
{
	auto* pLayer = dynamic_cast<LayerColor*>(pNode_);

	if (pLayer == nullptr)
	{
		return;
	}

	const UIColorF color = ResolveBrushColor(brush_.get(), state_);
	const Color4B color4b = ToColor4B(color);
	pLayer->setColor(Color3B(color4b.r, color4b.g, color4b.b));
	pLayer->setOpacity(color4b.a);
}

//////////////////////////////////////////////////////////////////////////////////////////
// 브러시 → 단색 해석
//////////////////////////////////////////////////////////////////////////////////////////
UIColorF ResolveBrushColor(const Brush* _pBrush, UIVisualState _state)
{
	if (_pBrush == nullptr)
	{
		return UIColorF{ 1.0f, 1.0f, 1.0f, 1.0f };
	}

	switch (_pBrush->GetType())
	{
	case BrushType::SolidColor:
		return static_cast<const SolidColorBrush*>(_pBrush)->color_;
	case BrushType::LinearGradient:
		return static_cast<const LinearGradientBrush*>(_pBrush)->start_;
	case BrushType::ThemeColor:
	{
		const auto* pBrush = static_cast<const ThemeColorBrush*>(_pBrush);
		const UIThemeColorState state = pBrush->followsState_ ? ToThemeColorState(_state) : pBrush->fixedState_;
		return UIThemeManager::Get()->GetColors().Get(pBrush->control_, state, pBrush->role_);
	}
	case BrushType::Theme:
	{
		// 베이크 텍스처 브러시는 대표색으로 Resolve된 표면 상단색을 사용한다.
		const auto* pBrush = static_cast<const ThemeBrush*>(_pBrush);
		UIResolvedStyle resolved = UIThemeManager::Get()->Resolve(pBrush->controlType_, _state, {});
		return resolved.surfaceTop;
	}
	default:
		return UIColorF{ 1.0f, 1.0f, 1.0f, 1.0f };
	}
}

//////////////////////////////////////////////////////////////////////////////////////////
// BorderEdgeVisual
//////////////////////////////////////////////////////////////////////////////////////////
BorderEdgeVisual::~BorderEdgeVisual()
{
	Detach();
}

void BorderEdgeVisual::Attach(cc::Node* _pHost, int _zOrder /* = -50 */)
{
	pHost_ = _pHost;
	zOrder_ = _zOrder;
	Sync();
}

void BorderEdgeVisual::Detach()
{
	for (int idx = 0; idx < 4; ++idx)
	{
		if (pEdges_[idx])
		{
			pEdges_[idx]->removeFromParent();
			pEdges_[idx] = nullptr;
		}
	}

	pHost_ = nullptr;
}

void BorderEdgeVisual::SetBrush(const BrushPtr& _brush)
{
	brush_ = _brush;
	Sync();
}

void BorderEdgeVisual::SetThickness(const Thickness& _thickness)
{
	thickness_ = _thickness;
	Sync();
}

void BorderEdgeVisual::SetVisualState(UIVisualState _state)
{
	if (state_ == _state)
	{
		return;
	}

	state_ = _state;
	Sync();
}

void BorderEdgeVisual::Resize(const cc::size& _size)
{
	size_ = _size;
	Sync();
}

void BorderEdgeVisual::RefreshTheme()
{
	Sync();
}

void BorderEdgeVisual::Sync()
{
	if (pHost_ == nullptr)
	{
		return;
	}

	const bool hasBorder = brush_ != nullptr
		&& (thickness_.left_ > 0.0f || thickness_.top_ > 0.0f || thickness_.right_ > 0.0f || thickness_.bottom_ > 0.0f)
		&& size_.width > 0.0f && size_.height > 0.0f;

	if (!hasBorder)
	{
		for (int idx = 0; idx < 4; ++idx)
		{
			if (pEdges_[idx])
			{
				pEdges_[idx]->setVisible(false);
			}
		}
		return;
	}

	const UIColorF color = ResolveBrushColor(brush_.get(), state_);
	const Color4B color4b = ToColor4B(color);

	const float width = size_.width;
	const float height = size_.height;
	const float thickness[4] = { thickness_.left_, thickness_.top_, thickness_.right_, thickness_.bottom_ };

	// 상/하단 변은 좌/우 변과 겹치지 않도록 내부 폭만 차지한다.
	const float innerWidth = std::max(0.0f, width - thickness_.left_ - thickness_.right_);

	for (int idx = 0; idx < 4; ++idx)
	{
		if (thickness[idx] <= 0.0f)
		{
			if (pEdges_[idx])
			{
				pEdges_[idx]->setVisible(false);
			}
			continue;
		}

		if (pEdges_[idx] == nullptr)
		{
			pEdges_[idx] = LayerColor::create(Color4B::WHITE, 1.0f, 1.0f);
			pHost_->addChild(pEdges_[idx], zOrder_);
		}

		pEdges_[idx]->setColor(Color3B(color4b.r, color4b.g, color4b.b));
		pEdges_[idx]->setOpacity(color4b.a);
		pEdges_[idx]->setVisible(true);
	}

	// left, top, right, bottom (코코스 y-up 좌표)
	if (pEdges_[0] && thickness[0] > 0.0f) { pEdges_[0]->setContentSize(cc::size(thickness[0], height)); pEdges_[0]->setPosition(0.0f, 0.0f); }
	if (pEdges_[1] && thickness[1] > 0.0f) { pEdges_[1]->setContentSize(cc::size(innerWidth, thickness[1])); pEdges_[1]->setPosition(thickness_.left_, height - thickness[1]); }
	if (pEdges_[2] && thickness[2] > 0.0f) { pEdges_[2]->setContentSize(cc::size(thickness[2], height)); pEdges_[2]->setPosition(width - thickness[2], 0.0f); }
	if (pEdges_[3] && thickness[3] > 0.0f) { pEdges_[3]->setContentSize(cc::size(innerWidth, thickness[3])); pEdges_[3]->setPosition(thickness_.left_, 0.0f); }
}

} // namespace sgui
