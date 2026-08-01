/*
 * 작성자: 윤정도
 * 생성일: 2/15/2023 8:11:04 PM
 * =====================
 *
 */

#include "GameCoreHeader.h"
#include "Game/Contents/UIManager.h"

#include "sg/Core/AppConfig.h"
#include "sg/Struct/SteinsGate_Client.h"

#include "sgcl/Game/UI/UIGroup.h"
#include "sgcl/Game/UI/UIRootGroup.h"
#include "sgcl/Game/UI/UIXmlLoader.h"

#include "sgcl/Game/Contents/PopupManager.h"
#include "sgcl/SteinsGateApp.h"
#include "sgcl/Layer/Layer_UI.h"
#include "sgcl/Game/Texture/ImagePackManager.h"


USING_NS_CC;
USING_NS_JC;

//////////////////////////////////////////////////////////////////////////////////////////
UIManager::UIManager()
: loadedUITexture_(1024)
, popup_(*PopupManager::Get())
{
}

//////////////////////////////////////////////////////////////////////////////////////////
UIManager::~UIManager()
{
}

//////////////////////////////////////////////////////////////////////////////////////////
void UIManager::RegisterUIFactory(const char* _name, UIFactoryFunc _factory)
{
	uiFactoryMap_[std::string(_name)] = _factory;
}

//////////////////////////////////////////////////////////////////////////////////////////
UIRootGroup* UIManager::Show(const char* _name, const CDataMap<>& _param)
{
	if (!pUILayer_)
	{
		_LogError_("UILayer가 설정되지 않았습니다. SetUILayer()를 먼저 호출하세요.");
		return nullptr;
	}

	jc::String filePath = jc::Path::Combine(
		jc::Path::Combine(g_cAppConfig.resDataPath_, "layout"),
		jc::String(_name) + ".xml");

	UIGroupInfo* pGroupInfo = UIXmlLoader::LoadFromFile(filePath.Source());
	if (!pGroupInfo)
	{
		_LogError_("%s UI XML 파일을 로드하는데 실패했습니다.", filePath.Source());
		return nullptr;
	}

	const std::string rootName(pGroupInfo->name_);
	if (rootName.empty())
	{
		_LogError_("%s XML 루트 Group의 name 속성이 비어있습니다.", filePath.Source());
		delete pGroupInfo;
		return nullptr;
	}

	if (rootName != _name)
	{
		_LogDebug_("%s XML 루트 Group name(%s)이(가) 요청한 이름(%s)과 다릅니다.",
			filePath.Source(), rootName.c_str(), _name);
	}

	UIRootGroup* pRootGroup = nullptr;
	auto it = uiFactoryMap_.find(rootName);
	if (it != uiFactoryMap_.end())
	{
		pRootGroup = it->second(pGroupInfo);
	}
	else
	{
		_LogError_("%s XML 루트 Group name(%s)에 해당하는 UI 클래스가 등록되지 않았습니다.",
			filePath.Source(), rootName.c_str());
		delete pGroupInfo;
		return nullptr;
	}

	pRootGroup->autorelease();
	pRootGroup->init();
	pRootGroup->InitFromXml();
	pRootGroup->SetRelativePosition(0, 0);

	CDataMap<> dataMap = _param;
	if (pGroupInfo->pDataMap_)
	{
		dataMap.AddData(*pGroupInfo->pDataMap_, false);
	}

	pRootGroup->OnInit(dataMap);
	pRootGroup->Load();
	pUILayer_->AddUIGroup(pRootGroup);
	pRootGroup->OnAdded();

	return pRootGroup;
}

//////////////////////////////////////////////////////////////////////////////////////////
void UIManager::Init()
{
	// Factory registrations happen via REGISTER_UI macros (static initialization)
	// No more UIRootGroupManager or global root group creation
}

//////////////////////////////////////////////////////////////////////////////////////////
void UIManager::RegisterUITexture(SgaResourceIndex _index)
{
	if (_index.un_.frameIndex_ == InvalidValue_v)
	{
		return;
	}

	const bool inserted = loadedUITexture_.Insert(_index.value_, _index);

	if (inserted)
	{
		g_cImagePackMgr.LogTexture("로드", _index, LoggerAbstract::eDebug);
	}
}

//////////////////////////////////////////////////////////////////////////////////////////
void UIManager::UnloadAll()
{
	ImagePackManager* pPackManager = ImagePackManager::Get();

	loadedUITexture_.ForEachValue([pPackManager](SgaResourceIndex& _resourceIndex)
	{
		pPackManager->ReleaseFrameTexture(_resourceIndex);
		pPackManager->UnloadPackData(_resourceIndex.un_.sgaIndex_);
	});

	loadedUITexture_.Clear();
}

//////////////////////////////////////////////////////////////////////////////////////////
void UIManager::OnUpdate(float _dt)
{
	CallUiElementsUpdateCallback(_dt);
}

//////////////////////////////////////////////////////////////////////////////////////////
void UIManager::CallUiElementsUpdateCallback(float _dt)
{
	uiElementsUpdateEvent_.ForEach([&_dt](Pair<UIElement*, jc::Event<UIElement*, float>>& _elementEventPair)
	{
		_elementEventPair.value_.Invoke((UIElement*)_elementEventPair.key_, (float)_dt);
	});
}

//////////////////////////////////////////////////////////////////////////////////////////
void UIManager::Draginit(const DragState& _state)
{
	dragState_ = _state;
}

//////////////////////////////////////////////////////////////////////////////////////////
void UIManager::DragEnter(const cc::EventMouse* _pMouseEvent)
{
	UIElement* pDragElement = dragState_.pTargetElement_;
	const Vec2 dragDelta = _pMouseEvent->getStartCursorPos() - dragState_.startCursorPosition_;

	pDragElement->setPosition(dragState_.startElementPosition_ + dragDelta);
	pDragElement->GetRootGroup()->OnDragEnter(pDragElement, dragState_);

	dragState_.dragDelta_ = dragDelta;
	dragState_.isDragging_ = true;
}

void UIManager::DragMove(const cc::EventMouse* _pMouseEvent)
{
	UIElement* pDragElement = dragState_.pTargetElement_;
	const Vec2 dragDelta = _pMouseEvent->getStartCursorPos() - dragState_.startCursorPosition_;

	pDragElement->setPosition(dragState_.startElementPosition_ + dragDelta);
	pDragElement->GetRootGroup()->OnDragMove(pDragElement, dragState_);

	dragState_.dragDelta_ = dragDelta;
}

//////////////////////////////////////////////////////////////////////////////////////////
void UIManager::DragEnd()
{
	dragState_.pHostElement_ = nullptr;
	dragState_.pTargetElement_ = nullptr;
	dragState_.isDragging_ = false;
	dragState_.dragDelta_ = {};
}

//////////////////////////////////////////////////////////////////////////////////////////
FrameTexture* UIManager::CreateUITexture(int _sga, int _img, int _frame, bool _linearDodge /* = false  */)
{
	ImagePack* pPack = g_cImagePackMgr.GetPackUnsafe(_sga);

	if (pPack == nullptr)
	{
		g_cImagePackMgr.LogTexture("UIManager::createUITexture()", SgaResourceIndex{ _sga, _img, _frame },
		                                       LoggerAbstract::eWarn);
		return SpriteFrameTexture::GetDefault();
	}

	FrameTexture* pTexture = pPack->CreateFrameTexture(_img, _frame, _linearDodge);
	RegisterUITexture({ _sga, _img, _frame });
	return pTexture;
}

//////////////////////////////////////////////////////////////////////////////////////////
FrameTexture* UIManager::CreateUITextureRetained(int _sga, int _img, int _frame, bool _linearDodge)
{
	FrameTexture* pTexture = CreateUITexture(_sga, _img, _frame, _linearDodge);
	pTexture->retain();
	return pTexture;
}