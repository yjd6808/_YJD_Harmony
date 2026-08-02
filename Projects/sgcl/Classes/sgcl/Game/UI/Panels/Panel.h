/*
 * 작성자: 윤정도
 * =====================
 * sgui - 모든 레이아웃 컨테이너의 베이스 (WPF Panel)
 * 자식 목록과 Background 브러시를 소유한다.
 */

#pragma once

#include "sgcl/Game/UI/Core/UIElement.h"
#include "sgcl/Game/UI/Core/BrushVisual.h"

namespace sgui
{

class Panel : public UIElement
{
public:
	Panel();
	virtual ~Panel() override;

	void AddChild(UIElement* _pChild);
	void RemoveChild(UIElement* _pChild);
	void ClearChildren();

	virtual int GetChildElementCount() const override { return (int)children_.size(); }
	virtual UIElement* GetChildElementAt(int _index) const override;

	void SetBackground(const BrushPtr& _brush);
	const BrushPtr& GetBackground() const { return backgroundVisual_.GetBrush(); }

	// 배경이 있을 때만 자기 자신이 히트테스트 대상이 된다. (WPF와 동일)
	virtual bool IsSelfHitTestVisible() const override { return isHitTestVisible_ && backgroundVisual_.GetBrush() != nullptr; }

	virtual void RefreshThemeVisuals() override;

protected:
	virtual void OnRenderSizeChanged(const cc::size& _size) override;

	std::vector<UIElement*> children_;
	BrushVisual backgroundVisual_;
};

} // namespace sgui
