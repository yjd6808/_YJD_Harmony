/*
 * 작성자: 윤정도
 * 생성일: 2/15/2023 4:54:29 AM
 * =====================
 * UI 구조 예시
 *
 *                SGUIGroup -> 이녀석을 MasterUIGroup으로 부름
 *		              │
 *		 ┌────────────┼───────────────┐
 *	SGUIGroup    SGUIButton     SGUIGroup
 *	  ....                      ┌────┴─────┐
 *							SGUIButton  SGUILabel
 *	             
 */


#pragma once

#include <SteinsGate/Client/SGUIElement.h>
#include <SteinsGate/Client/SGUIButton.h>
#include <SteinsGate/Client/SGUISprite.h>
#include <SteinsGate/Client/SGUILabel.h>
#include <SteinsGate/Client/SGImagePack.h>

class SGUIManager
{
public:
	SGUIManager();
	~SGUIManager();

	static SGUIManager* get();

	void init();
	void registerGroup(SGUIGroup* group);
	void registerLoadedUITexture(SGNpkResourceIndex index);
	void unloadAll();

	SGUIGroup* getGroup(int groupCode);
	SGUIElement* getElement(int elementCode);
	SGUIButton* getButton(int buttonCode);
	SGUISprite* getSprite(int spriteCode);
	SGUILabel* getLabel(int labelCode);
private:
	SGHashMap<Int32U, SGNpkResourceIndex> m_hLoadedUITexture;		// 어떤 이미지 팩 로딩했는지 기록용
	SGHashMap<int, SGUIElement*> m_hUIElements;
	SGHashMap<int, SGUIGroup*> m_hMasterUIGroups;
};

