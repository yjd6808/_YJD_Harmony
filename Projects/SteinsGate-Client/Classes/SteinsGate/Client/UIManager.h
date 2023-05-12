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
 *
 *	             
 */


#pragma once

#include <SteinsGate/Client/ImagePack.h>
#include <SteinsGate/Client/UIGroupMaster.h>

struct DragState
{
	DragState()
		: HostElement(nullptr)
		, TargetElement(nullptr)
		, Dragging(false)
	{}

	UIElement* HostElement;			// 드래그 주체
	UIElement* TargetElement;		// 실제로 드래깅될 대상
	SGVec2 StartElementPosition;
	SGVec2 StartCursorPosition;	
	SGVec2 DragDelta;
	bool Dragging;
};

class UI_Inventory;
class UI_Login;
class UI_Popup;
class UI_Test;

class UIManager final : public JCore::SingletonPointer<UIManager>
{
private:
	friend class TSingleton;
	UIManager();
	~UIManager() override;
public:
	void init();
	void initPublic();
	void registerMasterGroup(UIMasterGroup* group);
	void registerLoadedUITexture(SgaResourceIndex index);
	void unloadAll();
	void onUpdate(float dt);
	void callUIElementsUpdateCallback(float dt);

	void draginit(const DragState& state);
	void dragEnter(const SGEventMouse* mouseEvent);
	void dragMove(const SGEventMouse* mouseEvent);
	void dragEnd();

	bool isDragging() { return m_DragState.Dragging; }
	const DragState& getDragState() const { return m_DragState; }

	UIMasterGroup* getMasterGroup(int groupCode);
	UIElement* getElement(int elementCode);
	UIGroup* getGroup(int groupCode) { return getElementTemplated<UIGroup>(groupCode); }
	UIButton* getButton(int buttonCode) { return getElementTemplated<UIButton>(buttonCode); }
	UISprite* getSprite(int spriteCode) { return getElementTemplated<UISprite>(spriteCode); }
	UILabel* getLabel(int labelCode) { return getElementTemplated<UILabel>(labelCode); }
	UICheckBox* getCheckBox(int checkBoxCode) { return getElementTemplated<UICheckBox>(checkBoxCode); }
	UIEditBox* getEditBox(int editBoxCode) { return getElementTemplated<UIEditBox>(editBoxCode); }
	UIToggleButton* getToggleButton(int toggleButtonCode) { return getElementTemplated<UIToggleButton>(toggleButtonCode); }
	UIProgressBar* getProgressBar(int progressBarCode) { return getElementTemplated<UIProgressBar>(progressBarCode); }
	UIScrollBar* getScrollBar(int scrollBarCode) { return getElementTemplated<UIScrollBar>(scrollBarCode); }
	UIStatic* getStatic(int staticCode) { return getElementTemplated<UIStatic>(staticCode); }

	UI_Inventory* Inventory;
	UI_Login* Login;
	UI_Popup* Popup;
	UI_Test* Test;
private:
	template <typename TElement>
	TElement* getElementTemplated(int code) {
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

	DragState m_DragState;
	UIGroupMaster* m_pMaster;
	SGHashMap<Int32U, SgaResourceIndex> m_hLoadedUITexture;		// 어떤 이미지 팩 로딩했는지 기록용
	SGHashMap<int, UIElement*> m_hUIElements;
	SGHashMap<int, UIMasterGroup*> m_hMasterUIGroups;
	SGHashMap<UIElement*, SGEventList<UIElement*, float>> m_hUIElementsUpdateEvent;
};

