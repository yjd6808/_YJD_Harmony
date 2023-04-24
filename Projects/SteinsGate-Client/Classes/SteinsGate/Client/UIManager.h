/*
 * 작성자: 윤정도
 * 생성일: 2/15/2023 4:54:29 AM
 * =====================
 * UI 구조 예시
 *
 *                 Group -> 이녀석을 MasterUIGroup으로 부름
 *		              │
 *		 ┌────────────┼───────────────┐
 *	   Group        Button          Group
 *	  ....                      ┌────┴─────┐
 *							  Button     Label
 *	             
 */


#pragma once

#include <SteinsGate/Client/ImagePack.h>
#include <SteinsGate/Client/UIGroupMaster.h>

class UIManager
{
public:
	UIManager();
	~UIManager();

	static UIManager* get();

	void init();
	void registerMasterGroup(UIMasterGroup* group);
	void registerLoadedUITexture(SgaResourceIndex index);
	void unloadAll();
	void onUpdate(float dt);

	void callUIElementsUpdateCallback(float dt);

	UIMasterGroup* getMasterGroup(int groupCode);
	UIElement* getElement(int elementCode);
	UIGroup* getGroup(int groupCode);
	UIButton* getButton(int buttonCode);
	UISprite* getSprite(int spriteCode);
	UILabel* getLabel(int labelCode);
	UICheckBox* getCheckBox(int checkBoxCode);
	UIEditBox* getEditBox(int editBoxCode);
	UIToggleButton* getToggleButton(int toggleButtonCode);
	UIProgressBar* getProgressBar(int progressBarCode);
	UIScrollBar* getScrollBar(int scrollBarCode);
	UIStatic* getStatic(int staticCode);
public:
	UIGroupMaster* Master;

private:
	SGHashMap<Int32U, SgaResourceIndex> m_hLoadedUITexture;		// 어떤 이미지 팩 로딩했는지 기록용
	SGHashMap<int, UIElement*> m_hUIElements;
	SGHashMap<int, UIMasterGroup*> m_hMasterUIGroups;
	
	SGHashMap<UIElement*, SGEventList<UIElement*, float>> m_hUIElementsUpdateEvent;
};

