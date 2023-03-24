/*
 * 작성자: 윤정도
 * 생성일: 3/22/2023 12:50:32 AM
 * =====================
 *
 */


#pragma once

#include <SteinsGate/Client/SGUIGroup.h>
#include <SteinsGate/Client/SGUIButton.h>
#include <SteinsGate/Client/SGUISprite.h>
#include <SteinsGate/Client/SGUILabel.h>
#include <SteinsGate/Client/SGUICheckBox.h>
#include <SteinsGate/Client/SGUIEditBox.h>
#include <SteinsGate/Client/SGUIToggleButton.h>
#include <SteinsGate/Client/SGUIProgressBar.h>
#include <SteinsGate/Client/SGUIScrollBar.h>
#include <SteinsGate/Client/SGUIStatic.h>

class SGUIMasterGroup : public SGUIGroup
{
public:
	SGUIMasterGroup(SGUIGroupInfo* groupInfo);

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

	virtual void onMouseEnter(SGUIElement* element, SGEventMouse* mouseEvent);
	virtual void onMouseMove(SGUIElement* element, SGEventMouse* mouseEvent);
	virtual void onMouseDown(SGUIElement* element, SGEventMouse* mouseEvent);
	virtual void onMouseUp(SGUIElement* element, SGEventMouse* mouseEvent);
	virtual void onMouseLeave(SGUIElement* element, SGEventMouse* mouseEvent);
	virtual void onMouseScroll(SGUIElement* element, SGEventMouse* mouseEvent);
	virtual void onCheckedStateChanged(SGUICheckBox* checkBox);

	virtual void onScrollBarUpButtonPressed(SGUIScrollBar* scrollBar, int rowPos);
	virtual void onScrollBarDownButtonPressed(SGUIScrollBar* scrollBar, int rowPos);
	virtual void onScrollBarThumbButtonPressed(SGUIScrollBar* scrollBar, int rowPos);
	virtual void onScrollBarPositionChanged(SGUIScrollBar* scrollBar, int prevRowPos, int curRosPos);

	virtual void onEditBoxEditingDidBegin(SGUIEditBox* editBox);
	virtual void onEditBoxTextChanged(SGUIEditBox* editBox, const std::string& str);
	virtual void onEditBoxReturn(SGUIEditBox* editBox);
	virtual void onEditBoxEditingDidEndWithAction(SGUIEditBox* editBox);
	
	SGString toString() override { return SGStringUtil::Format("마스터그룹(%d)", m_pInfo->Code); }
};

