/*
 * 작성자: 윤정도
 * 생성일: 2/15/2023 8:11:04 PM
 * =====================
 *
 */

#include "Tutturu.h"
#include "SGUIManager.h"

#include <SteinsGate/Client/SGDataManager.h>
#include <SteinsGate/Client/SGImagePackManager.h>
#include <SteinsGate/Client/SGUIGroup.h>
#include <SteinsGate/Client/UILogin.h>

USING_NS_CC;
USING_NS_JC;


SGUIManager::SGUIManager()
	: m_hMasterUIGroups(64)		// 창을 64개까지 만들일이 있을려나 ㅋㅋ
	, m_hUIElements(512)
	, m_hLoadedUITexture(1024)
{}

SGUIManager::~SGUIManager() {
	m_hMasterUIGroups.Values().Extension().ForEach([](SGUIGroup* group) { CC_SAFE_RELEASE(group); });
	m_hUIElements.Values().Extension().ForEach([](SGUIElement* element) { CC_SAFE_RELEASE(element); });
}

SGUIManager* SGUIManager::get() {
	static SGUIManager* manager;

	if (manager == nullptr) {
		manager = dbg_new SGUIManager;
		manager->init();
	}
	return manager;
}


// =====================================================
//           마스타 유아이 그룹 등록
// =====================================================
void SGUIManager::init() {
	registerGroup(UILogin::createRetain());
}

void SGUIManager::registerGroup(SGUIGroup* group) {
	m_hUIElements.Insert(group->getCode(), group);
	m_hMasterUIGroups.Insert(group->getCode(), group);

	group->forEach([this](SGUIElement* uiElement) {
		bool bInsert = m_hUIElements.Insert(uiElement->getCode(), uiElement);
		DebugAssertMsg(bInsert, "이미 해당 UI 엘리먼트가 그룹내 포함되어 있습니다.");
	});
}

void SGUIManager::registerLoadedUITexture(SGNpkResourceIndex index) {
	m_hLoadedUITexture.Insert(index.Value, index);
}


void SGUIManager::unloadAll() {

	SGImagePackManager* pPackManager = SGImagePackManager::get();

	// 이미지 텍스쳐 모두 릴리즈
	m_hMasterUIGroups.Values().Extension().ForEach([](SGUIGroup* group) {
		group->unload();
	});

	// 관련 캐쉬, 팩 모두 언로드
	m_hLoadedUITexture.Values().Extension().ForEach([pPackManager](SGNpkResourceIndex& resourceIndex) {
		pPackManager->releaseFrameTexture(resourceIndex);
		pPackManager->unloadPackData(resourceIndex.Un.NpkIndex);
	});
	m_hLoadedUITexture.Clear();
}

SGUIGroup* SGUIManager::getGroup(int groupCode) {
	if (!m_hUIElements.Exist(groupCode)) {
		DebugAssertMsg(false, "해당 UI 그룹이 존재하지 않습니다.");
		return nullptr;
	}

	SGUIElement* pElem = m_hUIElements[groupCode];
	DebugAssertMsg(pElem->getElementType() == UIElementType::Group, "UI 그룹 타입이 아닙니다.");
	return (SGUIGroup*)m_hUIElements[groupCode];
}

SGUIElement* SGUIManager::getElement(int elementCode) {
	if (!m_hUIElements.Exist(elementCode)) {
		DebugAssertMsg(false, "해당 UI 엘리먼트가 존재하지 않습니다.");
		return nullptr;
	}

	return m_hUIElements[elementCode];
}

SGUIButton* SGUIManager::getButton(int buttonCode) {
	if (!m_hUIElements.Exist(buttonCode)) {
		DebugAssertMsg(false, "해당 UI 버튼이 존재하지 않습니다.");
		return nullptr;
	}

	SGUIElement* pElem = m_hUIElements[buttonCode];
	DebugAssertMsg(pElem->getElementType() == UIElementType::Button, "UI 버튼 타입이 아닙니다.");
	return (SGUIButton*)m_hUIElements[buttonCode];
}


