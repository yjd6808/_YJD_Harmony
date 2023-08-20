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
class UIMasterGroup : public UIGroup
{
public:
	UIMasterGroup(UIGroupInfo* groupInfo);

	bool isMasterGroup() override { return true; }
	bool init() final; 

	// 로딩전, 자식 컨트롤들 init만 전부 이뤄진상태
	// 처음 한번만 호출 하도록 함
	virtual void onInit() = 0;

	// 로딩후, 자식 컨트롤들 텍스쳐, 스프라이트 모두 로딩되었을 때
	// 텍스쳐 재로딩시 다시 호출 됨
	virtual void onLoaded();
	virtual void onUnloaded();

	// UILayer에 부착/해제시
	virtual void onAdded();
	virtual void onRemoved();

	// 마스터 그룹 자식들중에서 발생하는 이벤트들
	virtual void onMouseEnterTarget(UIElement* element, SGEventMouse* mouseEvent);
	virtual void onMouseMoveTarget(UIElement* element, SGEventMouse* mouseEvent);
	virtual void onMouseDownTarget(UIElement* element, SGEventMouse* mouseEvent);
	virtual void onMouseUpTarget(UIElement* element, SGEventMouse* mouseEvent);
	virtual void onMouseLeaveTarget(UIElement* element, SGEventMouse* mouseEvent);
	virtual void onMouseScrollTarget(UIElement* element, SGEventMouse* mouseEvent);

	
	virtual void onDragEnter(UIElement* element, const DragState& dragState);
	virtual void onDragMove(UIElement* element, const DragState& dragState);
	virtual void onDragEnd(UIElement* element);

	virtual void onCheckedStateChanged(UICheckBox* checkBox, bool checked);
	virtual void onToggleStateChanged(UIToggleButton* toggleBtn, ToggleState state);

	virtual void onScrollBarUpButtonPressed(UIScrollBar* scrollBar, int rowPos);
	virtual void onScrollBarDownButtonPressed(UIScrollBar* scrollBar, int rowPos);
	virtual void onScrollBarThumbButtonPressed(UIScrollBar* scrollBar, int rowPos);
	virtual void onScrollBarPositionChanged(UIScrollBar* scrollBar, int prevRowPos, int curRosPos);

	virtual void onEditBoxEditingDidBegin(UIEditBox* editBox);
	virtual void onEditBoxTextChanged(UIEditBox* editBox, const std::string& str);
	virtual void onEditBoxReturn(UIEditBox* editBox);
	virtual void onEditBoxEditingDidEndWithAction(UIEditBox* editBox, SGEditBoxEndAction endAction);

	// 마스터 그룹에서 마우스 이벤트 발생시
	virtual void onMouseEnter(SGEventMouse* mouseEvent);
	virtual void onMouseMove(SGEventMouse* mouseEvent);
	virtual void onMouseDown(SGEventMouse* mouseEvent);
	virtual void onMouseUp(SGEventMouse* mouseEvent);
	virtual void onMouseLeave(SGEventMouse* mouseEvent);
	virtual void onMouseScroll(SGEventMouse* mouseEvent);


	SGString toString() override { return SGStringUtil::Format("마스터그룹(%d)", m_pInfo->Code); }
};

