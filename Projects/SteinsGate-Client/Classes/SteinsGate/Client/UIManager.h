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
 *						  Button     Label
 *
 *
 *	             
 */


#pragma once

#include <SteinsGate/Client/ImagePack.h>
#include <SteinsGate/Client/UIRootGroupManager.h>

struct DragState
{
	DragState()
	: HostElement(nullptr)
	, TargetElement(nullptr)
	, Dragging(false)
	{
	}

	UIElement* HostElement; // 드래그 주체
	UIElement* TargetElement; // 실제로 드래깅될 대상
	SGVec2 StartElementPosition;
	SGVec2 StartCursorPosition;
	SGVec2 DragDelta;
	bool Dragging;
};

class UI_Inventory;
class UI_Login;
class UI_Popup;
class UI_Test;
class UI_ChannelSelect;

class UIManager final : public JCore::SingletonPointer<UIManager>
{
private:
	friend class TSingleton;
	UIManager();
	~UIManager();

public:
	void init();
	void initPublic();
	void registerMasterGroup(UIRootGroup* _pGroup);
	void registerUITexture(SgaResourceIndex _index);
	void unloadAll();
	void onUpdate(float _dt);
	void callUIElementsUpdateCallback(float _dt);

	void draginit(const DragState& _state);
	void dragEnter(const SGEventMouse* _pMouseEvent);
	void dragMove(const SGEventMouse* _pMouseEvent);
	void dragEnd();

	bool isDragging() { return dragState_.Dragging; }
	const DragState& getDragState() const { return dragState_; }

	UIRootGroup* getMasterGroup(int _groupCode);
	UIElement* getElement(int _elementCode);
	UIGroup* getGroup(int _groupCode) { return getElementTemplated<UIGroup>(_groupCode); }
	UIButton* getButton(int _buttonCode) { return getElementTemplated<UIButton>(_buttonCode); }
	UISprite* getSprite(int _spriteCode) { return getElementTemplated<UISprite>(_spriteCode); }
	UILabel* getLabel(int _labelCode) { return getElementTemplated<UILabel>(_labelCode); }
	UICheckBox* getCheckBox(int _checkBoxCode) { return getElementTemplated<UICheckBox>(_checkBoxCode); }
	UIEditBox* getEditBox(int _editBoxCode) { return getElementTemplated<UIEditBox>(_editBoxCode); }

	UIToggleButton* getToggleButton(int _toggleButtonCode)
	{
		return getElementTemplated<UIToggleButton>(_toggleButtonCode);
	}

	UIProgressBar* getProgressBar(int _progressBarCode) { return getElementTemplated<UIProgressBar>(_progressBarCode); }
	UIScrollBar* getScrollBar(int _scrollBarCode) { return getElementTemplated<UIScrollBar>(_scrollBarCode); }
	UIStatic* getStatic(int _staticCode) { return getElementTemplated<UIStatic>(_staticCode); }

	FrameTexture* createUITexture(int _sga, int _img, int _frame, bool _linearDodge = false);
	FrameTexture* createUITextureRetained(int _sga, int _img, int _frame, bool _linearDodge = false);

	UI_Inventory* Inventory;
	UI_Login* Login;
	UI_Popup* Popup;
	UI_Test* Test;
	UI_ChannelSelect* ChannelSelect;

private:
	template <typename TElement>
	TElement* getElementTemplated(int _code)
	{
		constexpr UIElementType_t targetType = TElement::type();

		if (!uiElements_.Exist(_code))
		{
			_LogWarn_("%s(%d)를 찾지 못했습니다.", UIElementType::Name[targetType], _code);
			return nullptr;
		}

		UIElement* pElement = uiElements_[_code];
		const UIElementType_t type = pElement->GetElementType();

		if (type != targetType)
		{
			_LogWarn_("%d가 %s타입이 아니고, %s입니다.", _code, UIElementType::Name[targetType], UIElementType::Name[type]);
			return nullptr;
		}

		return (TElement*)pElement;
	}

	DragState dragState_;
	UIRootGroupManager* master_;
	SGHashMap<Int32U, SgaResourceIndex> loadedUiTexture_; // 어떤 이미지 팩 로딩했는지 기록용
	SGHashMap<int, UIElement*> uiElements_;
	SGHashMap<int, UIRootGroup*> masterUiGroups_;
	SGHashMap<UIElement*, SGEventList<UIElement*, float>> uiElementsUpdateEvent_;
};
