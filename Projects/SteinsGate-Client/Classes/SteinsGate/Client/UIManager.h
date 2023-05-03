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

class UIManager final : public JCore::SingletonPointer<UIManager>
{
private:
	friend class TSingleton;
	UIManager();
	~UIManager() override;
public:
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

private:
	template <typename TElement>
	TElement* getElementTemplated(int code) {
		static_assert(JCore::IsPointerType_v<TElement>, "... TElem must be pointer type");
		constexpr UIElementType_t eTargetType = TElement::type();

		if (!m_hUIElements.Exist(code)) {
			_LogWarn_("%s(%d)를 찾지 못했습니다.", UIElementType::Name[eTargetType], code);
			return nullptr;
		}

		UIElement* pElem = m_hUIElements[code];
		const UIElementType_t eType = pElem->getElementType();

		if (eType != eTargetType) {
			_LogWarn_("%d가 %s타입이 아니고, %s입니다.", code, UIElementType::Name[eTargetType], UIElementType::Name[eType]);
			return nullptr;
		}
		return (TElement*)pElem;
	}


	UIGroupMaster* m_pMaster;
	SGHashMap<Int32U, SgaResourceIndex> m_hLoadedUITexture;		// 어떤 이미지 팩 로딩했는지 기록용
	SGHashMap<int, UIElement*> m_hUIElements;
	SGHashMap<int, UIMasterGroup*> m_hMasterUIGroups;
	SGHashMap<UIElement*, SGEventList<UIElement*, float>> m_hUIElementsUpdateEvent;
};

