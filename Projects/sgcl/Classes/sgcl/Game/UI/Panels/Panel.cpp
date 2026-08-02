/*
 * 작성자: 윤정도
 * =====================
 */

#include "GameCoreHeader.h"
#include "sgcl/Game/UI/Panels/Panel.h"

#include <algorithm>

namespace sgui
{

Panel::Panel()
{
	backgroundVisual_.Attach(this, -100);
}

Panel::~Panel()
{
	children_.clear();
}

void Panel::AddChild(UIElement* _pChild)
{
	if (_pChild == nullptr)
	{
		return;
	}

	children_.push_back(_pChild);
	_pChild->SetParentElement(this);
	addChild(_pChild);
	InvalidateLayout();
}

void Panel::RemoveChild(UIElement* _pChild)
{
	if (_pChild == nullptr)
	{
		return;
	}

	const auto it = std::find(children_.begin(), children_.end(), _pChild);

	if (it == children_.end())
	{
		return;
	}

	children_.erase(it);
	_pChild->SetParentElement(nullptr);
	_pChild->removeFromParent();
	InvalidateLayout();
}

void Panel::ClearChildren()
{
	for (size_t i = 0; i < children_.size(); ++i)
	{
		children_[i]->SetParentElement(nullptr);
		children_[i]->removeFromParent();
	}

	children_.clear();
	InvalidateLayout();
}

UIElement* Panel::GetChildElementAt(int _index) const
{
	if (_index < 0 || _index >= (int)children_.size())
	{
		return nullptr;
	}

	return children_[_index];
}

void Panel::SetBackground(const BrushPtr& _brush)
{
	backgroundVisual_.SetBrush(_brush);
	backgroundVisual_.Resize(renderSize_);
}

void Panel::RefreshThemeVisuals()
{
	backgroundVisual_.RefreshTheme();
	UIElement::RefreshThemeVisuals();
}

void Panel::OnRenderSizeChanged(const cc::size& _size)
{
	backgroundVisual_.Resize(_size);
}

} // namespace sgui
