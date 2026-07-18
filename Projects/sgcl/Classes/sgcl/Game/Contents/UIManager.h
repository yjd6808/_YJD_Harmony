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
 */


#pragma once

#include <string>
#include <unordered_map>
#include "sgcl/Game/Texture/ImagePack.h"
#include "jc/Container/DataMap.h"
#include "sg/Struct/SteinsGate_UI.h"

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
	cc::vec2 startElementPosition_;
	cc::vec2 startCursorPosition_;
	cc::vec2 dragDelta_;
	bool isDragging_;
};

class UI_Inventory;
class UI_Login;
class UI_Popup;
class UI_Test;
class UI_ChannelSelect;
class UIRootGroup;
class UIGroupInfo;
class UILayer;

using UIFactoryFunc = UIRootGroup* (*)(UIGroupInfo* _pInfo);

class PopupManager;

class UIManager final : public jc::SingletonPointer<UIManager>
{
private:
	friend class TSingleton;
	UIManager();
	~UIManager();

public:
	void Init();
	void RegisterUITexture(SgaResourceIndex _index);
	void UnloadAll();
	void OnUpdate(float _dt);
	void CallUiElementsUpdateCallback(float _dt);

	void Draginit(const DragState& _state);
	void DragEnter(const cc::EventMouse* _pMouseEvent);
	void DragMove(const cc::EventMouse* _pMouseEvent);
	void DragEnd();

	bool IsDragging() { return dragState_.isDragging_; }
	const DragState& GetDragState() const { return dragState_; }

	void RegisterUIFactory(const char* _name, UIFactoryFunc _factory);
	UIRootGroup* Show(const char* _name, const jc::CDataMap<>& _param = jc::CDataMap<>());
	void SetUILayer(UILayer* _pLayer) { pUILayer_ = _pLayer; }

	FrameTexture* CreateUITexture(int _sga, int _img, int _frame, bool _linearDodge = false);
	FrameTexture* CreateUITextureRetained(int _sga, int _img, int _frame, bool _linearDodge = false);

private:
	PopupManager&		popup_;

	DragState dragState_;
	UILayer* pUILayer_ = nullptr;
	std::unordered_map<std::string, UIFactoryFunc> uiFactoryMap_;
	jc::HashMap<_u32, SgaResourceIndex> loadedUITexture_;
	jc::HashMap<UIElement*, jc::Event<UIElement*, float>> uiElementsUpdateEvent_;
};

#define g_cUIMgr (*UIManager::Get())