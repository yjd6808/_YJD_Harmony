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

#include <SteinsGate/Client/SGImagePack.h>
#include <SteinsGate/Client/SGUIGroupMaster.h>

class SGUIManager
{
public:
	SGUIManager();
	~SGUIManager();

	static SGUIManager* get();

	void init();
	void registerMasterGroup(SGUIMasterGroup* group);
	void registerLoadedUITexture(SgaResourceIndex index);
	void unloadAll();
	void onUpdate(float dt);

	void callUIElementsUpdateCallback(float dt);

	SGUIMasterGroup* getMasterGroup(int groupCode);
	SGUIElement* getElement(int elementCode);
	SGUIGroup* getGroup(int groupCode);
	SGUIButton* getButton(int buttonCode);
	SGUISprite* getSprite(int spriteCode);
	SGUILabel* getLabel(int labelCode);
	SGUICheckBox* getCheckBox(int checkBoxCode);
	SGUIEditBox* getEditBox(int editBoxCode);
	SGUIToggleButton* getToggleButton(int toggleButtonCode);
	SGUIProgressBar* getProgressBar(int progressBarCode);
	SGUIScrollBar* getScrollBar(int scrollBarCode);
	SGUIStatic* getStatic(int staticCode);
public:
	SGUIGroupMaster* Master;

private:
	SGHashMap<Int32U, SgaResourceIndex> m_hLoadedUITexture;		// 어떤 이미지 팩 로딩했는지 기록용
	SGHashMap<int, SGUIElement*> m_hUIElements;
	SGHashMap<int, SGUIMasterGroup*> m_hMasterUIGroups;
	
	SGHashMap<SGUIElement*, SGEventList<SGUIElement*, float>> m_hUIElementsUpdateEvent;
};

