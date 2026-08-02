/*
 * 작성자: 윤정도
 * =====================
 * sgui - 절대좌표 배치 패널 (WPF Canvas)
 * 자식은 SetCanvasLeft/Top/Right/Bottom 부착 속성으로 배치된다.
 */

#pragma once

#include "sgcl/Game/UI/Panels/Panel.h"

namespace sgui
{

class Canvas : public Panel
{
public:
	static Canvas* Create()
	{
		Canvas* pCanvas = dbg_new Canvas;
		pCanvas->autorelease();
		return pCanvas;
	}

protected:
	cc::size MeasureOverride(const cc::size& _constraint) override
	{
		UNUSED(_constraint);

		const cc::size infinite(Infinity_v, Infinity_v);

		for (size_t i = 0; i < children_.size(); ++i)
		{
			children_[i]->Measure(infinite);
		}

		// Canvas는 자식 크기를 보고하지 않는다. (WPF와 동일)
		return cc::size(0.0f, 0.0f);
	}

	void ArrangeOverride(const cc::size& _finalSize) override
	{
		for (size_t i = 0; i < children_.size(); ++i)
		{
			UIElement* pChild = children_[i];
			const cc::size& desired = pChild->GetDesiredSize();

			float width = desired.width;
			float height = desired.height;

			// 명시 크기가 있으면 그것이 desired에 반영되어 있다.
			float x = 0.0f;
			float y = 0.0f;

			if (!IsAutoSize(pChild->GetCanvasLeft()))
			{
				x = pChild->GetCanvasLeft();
			}
			else if (!IsAutoSize(pChild->GetCanvasRight()))
			{
				x = _finalSize.width - pChild->GetCanvasRight() - width;
			}

			if (!IsAutoSize(pChild->GetCanvasTop()))
			{
				y = pChild->GetCanvasTop();
			}
			else if (!IsAutoSize(pChild->GetCanvasBottom()))
			{
				y = _finalSize.height - pChild->GetCanvasBottom() - height;
			}

			pChild->Arrange(Rect(x, y, width, height));
		}
	}
};

} // namespace sgui
