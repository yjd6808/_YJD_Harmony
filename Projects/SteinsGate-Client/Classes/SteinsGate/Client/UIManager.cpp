/*
 * 작성자: 윤정도
 * 생성일: 2/15/2023 8:11:04 PM
 * =====================
 *
 */

#include "Tutturu.h"
#include "GameCoreHeader.h"
#include "UIManager.h"

#include <SteinsGate/Client/UIGroup.h>
#include <SteinsGate/Client/UIMasterGroup.h>
#include <SteinsGate/Client/Global.h>

#include <SteinsGate/Client/Define_UI.h>

#include <SteinsGate/Client/UI_Inventory.h>
#include <SteinsGate/Client/UI_Login.h>
#include <SteinsGate/Client/UI_Popup.h>
#include <SteinsGate/Client/UI_Test.h>
#include <SteinsGate/Client/UI_ChannelSelect.h>

USING_NS_CC;
USING_NS_JC;


UIManager::UIManager()
	: m_pMaster(nullptr)
	, m_hLoadedUITexture(1024) // 창을 64개까지 만들일이 있을려나 ㅋㅋ
	, m_hUIElements(512)
	, m_hMasterUIGroups(64)
{
	Inventory = nullptr;
	Login = nullptr;
	Popup = nullptr;
	Test = nullptr;
	ChannelSelect = nullptr;
}

UIManager::~UIManager() {

	CC_SAFE_RELEASE(m_pMaster);

	// 마스터 UI 그룹만 제거해주면 된다.
	// 어차피 내부 자식들은 모두 마스터 UI 그룹에 addChild 되어 있기 때문에
	// 이녀석만 제거하면 도미노 마냥 다 제거댐
	m_hMasterUIGroups.ForEachValue([](UIGroup* group) {
		CC_SAFE_RELEASE(group);
	});
}


// =====================================================
//           마스타 유아이 그룹 등록
// =====================================================

void UIManager::init() {
	m_pMaster = UIGroupMaster::createRetain();
	m_pMaster->forEach([this](UIMasterGroup* pMasterGroup) { registerMasterGroup(pMasterGroup); });
	m_pMaster->forEach([this](UIMasterGroup* pMasterGroup) { pMasterGroup->onInit(); });

	initPublic();
}


void UIManager::registerMasterGroup(UIMasterGroup* group) {
	group->retain();	// 그룹마스터에서 생성/관리하기때문에 리테인했고 UIManager에서도 마스터를 참조하므로, 리테인해줘야함. 빼먹고있었넹
	m_hUIElements.Insert(group->getCode(), group);
	m_hMasterUIGroups.Insert(group->getCode(), group);

	group->forEachRecursive([this](UIElement* uiElement) {
		const int code = uiElement->getCode();
		const bool bInsert = m_hUIElements.Insert(code, uiElement);
		if (!bInsert) _LogWarn_("이미 UI 엘리먼트(%d)가 그룹내 포함되어 있습니다.", code);
	});
}

void UIManager::registerUITexture(SgaResourceIndex index) {

	if (index.Un.FrameIndex == InvalidValue_v) {
		return;
	}

	
	const bool bInserted = m_hLoadedUITexture.Insert(index.Value, index);

	if (bInserted) {
		Core::Contents.PackManager->logTexture("로드", index, LoggerAbstract::eDebug);
	}
}


void UIManager::unloadAll() {

	ImagePackManager* pPackManager = ImagePackManager::Get();

	// 이미지 텍스쳐 모두 릴리즈
	m_hMasterUIGroups.ForEachValue([](UIGroup* group) {
		group->unload();
	});
	
	// 관련 캐쉬, 팩 모두 언로드
	m_hLoadedUITexture.ForEachValue([pPackManager](SgaResourceIndex& resourceIndex) {
		pPackManager->releaseFrameTexture(resourceIndex);
		pPackManager->unloadPackData(resourceIndex.Un.SgaIndex);
	});
	m_hLoadedUITexture.Clear();
}

void UIManager::onUpdate(float dt) {
	callUIElementsUpdateCallback(dt);
}

void UIManager::callUIElementsUpdateCallback(float dt) {
	m_hUIElementsUpdateEvent.ForEach([&dt](Pair<UIElement*, SGEventList<UIElement*, float>>& pair) {
		pair.Value.Invoke((UIElement*)pair.Key, (float)dt);
	});
}

void UIManager::draginit(const DragState& state) {
	m_DragState = state;
}

void UIManager::dragEnter(const SGEventMouse* mouseEvent) {
	
	UIElement* pDragElement = m_DragState.TargetElement;
	const Vec2 dragDelta = mouseEvent->getStartCursorPos() - m_DragState.StartCursorPosition;

	pDragElement->setPosition(m_DragState.StartElementPosition + dragDelta);
	pDragElement->getMasterGroup()->onDragEnter(pDragElement, m_DragState);

	m_DragState.DragDelta = dragDelta;
	m_DragState.Dragging = true;
}

void UIManager::dragMove(const SGEventMouse* mouseEvent) {
	UIElement* pDragElement = m_DragState.TargetElement;
	const Vec2 dragDelta = mouseEvent->getStartCursorPos() - m_DragState.StartCursorPosition;

	pDragElement->setPosition(m_DragState.StartElementPosition + dragDelta);
	pDragElement->getMasterGroup()->onDragMove(pDragElement, m_DragState);

	m_DragState.DragDelta = dragDelta;
}


void UIManager::dragEnd() {
	m_DragState.HostElement = nullptr;
	m_DragState.TargetElement = nullptr;
	m_DragState.Dragging = false;
	m_DragState.DragDelta = {};
}

UIMasterGroup* UIManager::getMasterGroup(int groupCode) {
	if (!m_hMasterUIGroups.Exist(groupCode)) {
		_LogWarn_("%d 마스터 UI 그룹이 존재하지 않습니다.", groupCode);
		return nullptr;
	}

	return m_hMasterUIGroups[groupCode];
}

UIElement* UIManager::getElement(int elementCode) {
	if (!m_hUIElements.Exist(elementCode)) {
		return nullptr;
	}

	return m_hUIElements[elementCode];
}

FrameTexture* UIManager::createUITexture(int sga, int img, int frame, bool linearDodge /* = false  */) {
	ImagePack* pPack = Core::Contents.PackManager->getPackUnsafe(sga);

	if (pPack == nullptr) {
		Core::Contents.PackManager->logTexture("UIManager::createUITexture()", SgaResourceIndex{sga, img, frame}, LoggerAbstract::eWarn);
		return Core::Contents.Global->getDefaultFrameTexture();
	}

	FrameTexture* pTexture = pPack->createFrameTexture(img, frame, linearDodge);
	registerUITexture({ sga, img, frame });
	return pTexture;
}

FrameTexture* UIManager::createUITextureRetained(int sga, int img, int frame, bool linearDodge) {
	FrameTexture* pTexture = createUITexture(sga, img, frame, linearDodge);
	pTexture->retain();
	return pTexture;
}

void UIManager::initPublic() {
	Login				= (UI_Login*)getMasterGroup(GROUP_UI_LOGIN);
	Inventory			= (UI_Inventory*)getMasterGroup(GROUP_UI_INVENTORY);
	Popup				= nullptr;		// 팝업 매니저에서 관리
	Test				= (UI_Test*)getMasterGroup(GROUP_UI_TEST);
	ChannelSelect		= (UI_ChannelSelect*)getMasterGroup(GROUP_UI_CHANNEL);
}

