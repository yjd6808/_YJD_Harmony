/*
 * 작성자: 윤정도
 * =====================
 * sgui - 드래그 요소 (WPF Thumb)
 * 드래그 시 ueDragStarted / ueDragDelta / ueDragCompleted(UIEventDrag)를 발생시킨다.
 */

#pragma once

#include "sgcl/Game/UI/Controls/Control.h"

namespace sgui
{

class Thumb : public Control
{
public:
	static Thumb* Create();

	bool IsDragging() const { return isDragging_; }

	virtual void OnEventRaised(int _id, UIEvent* _pEvent) override;

protected:
	void RaiseDragEvent(UIEventType _type, const cc::vec2& _worldPos);

	bool isDragging_ = false;
	cc::vec2 dragStartWorld_;
	cc::vec2 lastWorld_;
};

} // namespace sgui
