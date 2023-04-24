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

	virtual void onMouseEnter(UIElement* element, SGEventMouse* mouseEvent);
	virtual void onMouseMove(UIElement* element, SGEventMouse* mouseEvent);
	virtual void onMouseDown(UIElement* element, SGEventMouse* mouseEvent);
	virtual void onMouseUp(UIElement* element, SGEventMouse* mouseEvent);
	virtual void onMouseLeave(UIElement* element, SGEventMouse* mouseEvent);
	virtual void onMouseScroll(UIElement* element, SGEventMouse* mouseEvent);
	virtual void onCheckedStateChanged(UICheckBox* checkBox);

	virtual void onScrollBarUpButtonPressed(UIScrollBar* scrollBar, int rowPos);
	virtual void onScrollBarDownButtonPressed(UIScrollBar* scrollBar, int rowPos);
	virtual void onScrollBarThumbButtonPressed(UIScrollBar* scrollBar, int rowPos);
	virtual void onScrollBarPositionChanged(UIScrollBar* scrollBar, int prevRowPos, int curRosPos);

	virtual void onEditBoxEditingDidBegin(UIEditBox* editBox);
	virtual void onEditBoxTextChanged(UIEditBox* editBox, const std::string& str);
	virtual void onEditBoxReturn(UIEditBox* editBox);
	virtual void onEditBoxEditingDidEndWithAction(UIEditBox* editBox);
	
	SGString toString() override { return SGStringUtil::Format("마스터그룹(%d)", m_pInfo->Code); }
};

