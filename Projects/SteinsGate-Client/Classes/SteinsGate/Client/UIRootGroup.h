/*
 * 작성자: 윤정도
 * 생성일: 3/22/2023 12:50:32 AM
 * =====================
 *
 */


#pragma once

#include <SteinsGate/Client/UIGroup.h>
#include <SteinsGate/Client/UIButton.h>
#include <SteinsGate/Client/UISprite.h>
#include <SteinsGate/Client/UILabel.h>
#include <SteinsGate/Client/UICheckBox.h>
#include <SteinsGate/Client/UIEditBox.h>
#include <SteinsGate/Client/UIToggleButton.h>
#include <SteinsGate/Client/UIProgressBar.h>
#include <SteinsGate/Client/UIScrollBar.h>
#include <SteinsGate/Client/UIStatic.h>

struct DragState;

class UIRootGroup : public UIGroup
{
public:
	UIRootGroup(UIGroupInfo* _pGroupInfo);

	bool IsRootGroup() override
	{
		return true;
	}

	bool init() final;

	// 로딩전, 자식 컨트롤들 init만 전부 이뤄진상태
	// 처음 한번만 호출 하도록 함
	virtual void OnInit() = 0;

	// 로딩후, 자식 컨트롤들 텍스쳐, 스프라이트 모두 로딩되었을 때
	// 텍스쳐 재로딩시 다시 호출 됨
	virtual void OnLoaded();
	virtual void OnUnloaded();

	// UILayer에 부착/해제시
	virtual void OnAdded();
	virtual void OnRemoved();

	// 마스터 그룹 자식들중에서 발생하는 이벤트들
	virtual void OnMouseEnterTarget(UIElement* _pElement, SGEventMouse* _pMouseEvent);
	virtual void OnMouseMoveTarget(UIElement* _pElement, SGEventMouse* _pMouseEvent);
	virtual void OnMouseDownTarget(UIElement* _pElement, SGEventMouse* _pMouseEvent);
	virtual void OnMouseUpTarget(UIElement* _pElement, SGEventMouse* _pMouseEvent);
	virtual void OnMouseLeaveTarget(UIElement* _pElement, SGEventMouse* _pMouseEvent);
	virtual void OnMouseScrollTarget(UIElement* _pElement, SGEventMouse* _pMouseEvent);

	virtual void OnDragEnter(UIElement* _pElement, const DragState& _dragState);
	virtual void OnDragMove(UIElement* _pElement, const DragState& _dragState);
	virtual void OnDragEnd(UIElement* _pElement);

	virtual void OnCheckedStateChanged(UICheckBox* _pCheckBox, bool _checked);
	virtual void OnToggleStateChanged(UIToggleButton* _pToggleBtn, ToggleState _state);

	virtual void OnScrollBarUpButtonPressed(UIScrollBar* _pScrollBar, int _rowPos);
	virtual void OnScrollBarDownButtonPressed(UIScrollBar* _pScrollBar, int _rowPos);
	virtual void OnScrollBarThumbButtonPressed(UIScrollBar* _pScrollBar, int _rowPos);
	virtual void OnScrollBarPositionChanged(UIScrollBar* _pScrollBar, int _prevRowPos, int _curRosPos);

	virtual void OnEditBoxEditingDidBegin(UIEditBox* _pEditBox);
	virtual void OnEditBoxTextChanged(UIEditBox* _pEditBox, const std::string& _str);
	virtual void OnEditBoxReturn(UIEditBox* _pEditBox);
	virtual void OnEditBoxEditingDidEndWithAction(UIEditBox* _pEditBox, SGEditBoxEndAction _endAction);

	// 마스터 그룹에서 마우스 이벤트 발생시
	virtual void OnMouseEnter(SGEventMouse* _pMouseEvent);
	virtual void OnMouseMove(SGEventMouse* _pMouseEvent);
	virtual void OnMouseDown(SGEventMouse* _pMouseEvent);
	virtual void OnMouseUp(SGEventMouse* _pMouseEvent);
	virtual void OnMouseLeave(SGEventMouse* _pMouseEvent);
	virtual void OnMouseScroll(SGEventMouse* _pMouseEvent);

	SGString ToString() override
	{
		return SGStringUtil::Format("마스터그룹(%d)", pBaseInfo_->code_);
	}
};
