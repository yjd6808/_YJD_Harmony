/*
 * 작성자: 윤정도
 * 생성일: 2/15/2023 4:54:29 AM
 * =====================
 * UI 구조 예시
 *
 *                 Group -> 이녀석을 RootUIGroup으로 부름
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

#include <sgcl/ImagePack.h>
#include <sgcl/UIRootGroupManager.h>

struct DragState
{
	DragState()
	: pHostElement_(nullptr)
	, pTargetElement_(nullptr)
	, isDragging_(false)
	{
	}

	UIElement* pHostElement_; // 드래그 주체
	UIElement* pTargetElement_; // 실제로 드래깅될 대상
	c2d::vec2 startElementPosition_;
	c2d::vec2 startCursorPosition_;
	c2d::vec2 dragDelta_;
	bool isDragging_;
};

class UI_Inventory;
class UI_Login;
class UI_Popup;
class UI_Test;
class UI_ChannelSelect;

//////////////////////////////////////////////////////////////////////////////////////////
class UIManager final : public jc::SingletonPointer<UIManager>
{
private:
	friend class TSingleton;
	UIManager();
	~UIManager();

public:
	void Init();
	void InitPublic();
	void RegisterMasterGroup(UIRootGroup* _pGroup);
	void RegisterUITexture(SgaResourceIndex _index);
	void UnloadAll();
	void OnUpdate(float _dt);
	void CallUiElementsUpdateCallback(float _dt);

	void Draginit(const DragState& _state);
	void DragEnter(const c2d::EventMouse* _pMouseEvent);
	void DragMove(const c2d::EventMouse* _pMouseEvent);
	void DragEnd();

	bool IsDragging() { return dragState_.isDragging_; }
	const DragState& GetDragState() const { return dragState_; }

	UIRootGroup* GetRootGroup(int _groupCode);
	UIElement* GetElement(int _elementCode);
	UIGroup* GetGroup(int _groupCode) { return GetElementTemplated<UIGroup>(_groupCode); }
	UIButton* GetButton(int _buttonCode) { return GetElementTemplated<UIButton>(_buttonCode); }
	UISprite* GetSprite(int _spriteCode) { return GetElementTemplated<UISprite>(_spriteCode); }
	UILabel* GetLabel(int _labelCode) { return GetElementTemplated<UILabel>(_labelCode); }
	UICheckBox* GetCheckBox(int _checkBoxCode) { return GetElementTemplated<UICheckBox>(_checkBoxCode); }
	UIEditBox* GetEditBox(int _editBoxCode) { return GetElementTemplated<UIEditBox>(_editBoxCode); }
	UIToggleButton* GetToggleButton(int _toggleButtonCode) { return GetElementTemplated<UIToggleButton>(_toggleButtonCode); }
	UIProgressBar* GetProgressBar(int _progressBarCode) { return GetElementTemplated<UIProgressBar>(_progressBarCode); }
	UIScrollBar* GetScrollBar(int _scrollBarCode) { return GetElementTemplated<UIScrollBar>(_scrollBarCode); }
	UIStatic* GetStatic(int _staticCode) { return GetElementTemplated<UIStatic>(_staticCode); }

	FrameTexture* CreateUITexture(int _sga, int _img, int _frame, bool _linearDodge = false);
	FrameTexture* CreateUITextureRetained(int _sga, int _img, int _frame, bool _linearDodge = false);

	UI_Inventory&		GetUI_Inventory() const { return *pInventory_; }
	UI_Login&			GetUI_Login() const { return *pLogin_; }
	UI_Test&			GetUI_Test() const { return *pTest_; }
	UI_ChannelSelect&	GetUI_ChannelSelect() const { return *pChannelSelect_; }

private:
	template <typename TElement>
	TElement* GetElementTemplated(int _code)
	{
		constexpr UIElementType_t targetType = TElement::Type();

		if (!uiElementMap_.Exist(_code))
		{
			_LogWarn_("%s(%d)를 찾지 못했습니다.", UIElementType::Name[targetType], _code);
			return nullptr;
		}

		UIElement* pElement = uiElementMap_[_code];
		const UIElementType_t type = pElement->GetElementType();

		if (type != targetType)
		{
			_LogWarn_("%d가 %s타입이 아니고, %s입니다.", _code, UIElementType::Name[targetType], UIElementType::Name[type]);
			return nullptr;
		}

		return (TElement*)pElement;
	}

	UI_Inventory*		pInventory_;
	UI_Login*			pLogin_;
	UI_Test*			pTest_;
	UI_ChannelSelect*	pChannelSelect_;

	DragState dragState_;
	UIRootGroupManager* pRootGroupMgr_;
	jc::HashMap<Int32U, SgaResourceIndex> loadedUITexture_; // 어떤 이미지 팩 로딩했는지 기록용
	jc::HashMap<int, UIElement*> uiElementMap_;
	jc::HashMap<int, UIRootGroup*> masterUIGroups_;
	jc::HashMap<UIElement*, jc::Event<UIElement*, float>> uiElementsUpdateEvent_;
};

#define g_cUIMgr (*UIManager::Get())