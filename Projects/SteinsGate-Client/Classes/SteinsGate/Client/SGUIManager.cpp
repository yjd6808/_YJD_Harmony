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
#include <SteinsGate/Client/SGUIMasterGroup.h>


USING_NS_CC;
USING_NS_JC;


SGUIManager::SGUIManager()
	: Master(nullptr)
	, m_hLoadedUITexture(1024) // 창을 64개까지 만들일이 있을려나 ㅋㅋ
	, m_hUIElements(512)
	, m_hMasterUIGroups(64)
{}

SGUIManager::~SGUIManager() {

	CC_SAFE_RELEASE(Master);

	// 마스터 UI 그룹만 제거해주면 된다.
	// 어차피 내부 자식들은 모두 마스터 UI 그룹에 addChild 되어 있기 때문에
	// 이녀석만 제거하면 도미노 마냥 다 제거댐
	m_hMasterUIGroups.Values().Extension().ForEach([](SGUIGroup* group) {
		CC_SAFE_RELEASE(group);
	});
}

SGUIManager* SGUIManager::get() {
	static SGUIManager* manager;

	if (manager == nullptr) {
		manager = dbg_new SGUIManager;
	}
	return manager;
}


// =====================================================
//           마스타 유아이 그룹 등록
// =====================================================

void SGUIManager::init() {
	Master = SGUIGroupMaster::createRetain();
	Master->forEach([this](SGUIMasterGroup* pMasterGroup) { registerMasterGroup(pMasterGroup); });
	Master->forEach([this](SGUIMasterGroup* pMasterGroup) { pMasterGroup->onInit(); });
}

void SGUIManager::registerMasterGroup(SGUIMasterGroup* group) {
	m_hUIElements.Insert(group->getCode(), group);
	m_hMasterUIGroups.Insert(group->getCode(), group);

	group->forEachRecursive([this](SGUIElement* uiElement) {
		const int code = uiElement->getCode();
		const bool bInsert = m_hUIElements.Insert(code, uiElement);
		DebugAssertMsg(bInsert, "이미 %d UI 엘리먼트가 그룹내 포함되어 있습니다.", code);
	});
}

void SGUIManager::registerLoadedUITexture(SgaResourceIndex index) {

	if (index.Un.FrameIndex == InvalidValue_v)
		return;

	m_hLoadedUITexture.Insert(index.Value, index);
}


void SGUIManager::unloadAll() {

	SGImagePackManager* pPackManager = SGImagePackManager::get();

	// 이미지 텍스쳐 모두 릴리즈
	m_hMasterUIGroups.Values().Extension().ForEach([](SGUIGroup* group) {
		group->unload();
	});

	// 관련 캐쉬, 팩 모두 언로드
	m_hLoadedUITexture.Values().Extension().ForEach([pPackManager](SgaResourceIndex& resourceIndex) {
		pPackManager->releaseFrameTexture(resourceIndex);
		pPackManager->unloadPackData(resourceIndex.Un.SgaIndex);
	});
	m_hLoadedUITexture.Clear();
}

void SGUIManager::onUpdate(float dt) {
	callUIElementsUpdateCallback(dt);
}

void SGUIManager::callUIElementsUpdateCallback(float dt) {
	m_hUIElementsUpdateEvent.Extension().ForEach([&dt](Pair<SGUIElement*, SGEventList<SGUIElement*, float>> pair) {
		pair.Value.Invoke((SGUIElement*)pair.Key, (float)dt);
	});
}

SGUIMasterGroup* SGUIManager::getMasterGroup(int groupCode) {
	if (!m_hMasterUIGroups.Exist(groupCode)) {
		DebugAssertMsg(false, "%d 마스터 UI 그룹이 존재하지 않습니다.", groupCode);
		return nullptr;
	}

	return m_hMasterUIGroups[groupCode];
}

SGUIElement* SGUIManager::getElement(int elementCode) {
	if (!m_hUIElements.Exist(elementCode)) {
		return nullptr;
	}

	return m_hUIElements[elementCode];
}

SGUIGroup* SGUIManager::getGroup(int groupCode) {
	constexpr UIElementType_t eTargetType = UIElementType::Group;

	if (!m_hUIElements.Exist(groupCode)) {
		DebugAssertMsg("%s(%d)를 찾지 못했습니다.", UIElementType::Name[eTargetType], groupCode);
		return nullptr;
	}

	SGUIElement* pElem = m_hUIElements[groupCode];
	const UIElementType_t eType = pElem->getElementType();

	if (eType != eTargetType) {
		DebugAssertMsg(false, "%d가 %s타입이 아니고, %s입니다.", groupCode, UIElementType::Name[eTargetType], UIElementType::Name[eType]);
		return nullptr;
	}

	return (SGUIGroup*)pElem;
}

SGUIButton* SGUIManager::getButton(int buttonCode) {
	constexpr UIElementType_t eTargetType = UIElementType::Button;

	if (!m_hUIElements.Exist(buttonCode)) {
		DebugAssertMsg("%s(%d)를 찾지 못했습니다.", UIElementType::Name[eTargetType], buttonCode);
		return nullptr;
	}
	
	SGUIElement* pElem = m_hUIElements[buttonCode];
	const UIElementType_t eType = pElem->getElementType();

	if (eType != eTargetType) {
		DebugAssertMsg(false, "%d가 %s타입이 아니고, %s입니다.", buttonCode, UIElementType::Name[eTargetType], UIElementType::Name[eType]);
		return nullptr;
	}

	return (SGUIButton*)pElem;
}

SGUISprite* SGUIManager::getSprite(int spriteCode) {
	constexpr UIElementType_t eTargetType = UIElementType::Sprite;

	if (!m_hUIElements.Exist(spriteCode)) {
		DebugAssertMsg("%s(%d)를 찾지 못했습니다.", UIElementType::Name[eTargetType], spriteCode);
		return nullptr;
	}

	SGUIElement* pElem = m_hUIElements[spriteCode];
	const UIElementType_t eType = pElem->getElementType();

	if (eType != eTargetType) {
		DebugAssertMsg(false, "%d가 %s타입이 아니고, %s입니다.", spriteCode, UIElementType::Name[eTargetType], UIElementType::Name[eType]);
		return nullptr;
	}

	return (SGUISprite*)pElem;
}

SGUILabel* SGUIManager::getLabel(int labelCode) {
	constexpr UIElementType_t eTargetType = UIElementType::Label;

	if (!m_hUIElements.Exist(labelCode)) {
		DebugAssertMsg("%s(%d)를 찾지 못했습니다.", UIElementType::Name[eTargetType], labelCode);
		return nullptr;
	}

	SGUIElement* pElem = m_hUIElements[labelCode];
	const UIElementType_t eType = pElem->getElementType();

	if (eType != eTargetType) {
		DebugAssertMsg(false, "%d가 %s타입이 아니고, %s입니다.", labelCode, UIElementType::Name[eTargetType], UIElementType::Name[eType]);
		return nullptr;
	}

	return (SGUILabel*)pElem;
}

SGUICheckBox* SGUIManager::getCheckBox(int checkBoxCode) {
	constexpr UIElementType_t eTargetType = UIElementType::CheckBox;

	if (!m_hUIElements.Exist(checkBoxCode)) {
		DebugAssertMsg("%s(%d)를 찾지 못했습니다.", UIElementType::Name[eTargetType], checkBoxCode);
		return nullptr;
	}

	SGUIElement* pElem = m_hUIElements[checkBoxCode];
	const UIElementType_t eType = pElem->getElementType();

	if (eType != eTargetType) {
		DebugAssertMsg(false, "%d가 %s타입이 아니고, %s입니다.", checkBoxCode, UIElementType::Name[eTargetType], UIElementType::Name[eType]);
		return nullptr;
	}

	return (SGUICheckBox*)pElem;
}

SGUIEditBox* SGUIManager::getEditBox(int editBoxCode) {
	constexpr UIElementType_t eTargetType = UIElementType::EditBox;

	if (!m_hUIElements.Exist(editBoxCode)) {
		DebugAssertMsg("%s(%d)를 찾지 못했습니다.", UIElementType::Name[eTargetType], editBoxCode);
		return nullptr;
	}

	SGUIElement* pElem = m_hUIElements[editBoxCode];
	const UIElementType_t eType = pElem->getElementType();

	if (eType != eTargetType) {
		DebugAssertMsg(false, "%d가 %s타입이 아니고, %s입니다.", editBoxCode, UIElementType::Name[eTargetType], UIElementType::Name[eType]);
		return nullptr;
	}

	return (SGUIEditBox*)pElem;
}

SGUIToggleButton* SGUIManager::getToggleButton(int toggleButtonCode) {
	constexpr UIElementType_t eTargetType = UIElementType::ToggleButton;

	if (!m_hUIElements.Exist(toggleButtonCode)) {
		DebugAssertMsg("%s(%d)를 찾지 못했습니다.", UIElementType::Name[eTargetType], toggleButtonCode);
		return nullptr;
	}

	SGUIElement* pElem = m_hUIElements[toggleButtonCode];
	const UIElementType_t eType = pElem->getElementType();

	if (eType != eTargetType) {
		DebugAssertMsg(false, "%d가 %s타입이 아니고, %s입니다.", toggleButtonCode, UIElementType::Name[eTargetType], UIElementType::Name[eType]);
		return nullptr;
	}

	return (SGUIToggleButton*)pElem;
}

SGUIProgressBar* SGUIManager::getProgressBar(int progressBarCode) {
	constexpr UIElementType_t eTargetType = UIElementType::ProgressBar;

	if (!m_hUIElements.Exist(progressBarCode)) {
		DebugAssertMsg("%s(%d)를 찾지 못했습니다.", UIElementType::Name[eTargetType], progressBarCode);
		return nullptr;
	}

	SGUIElement* pElem = m_hUIElements[progressBarCode];
	const UIElementType_t eType = pElem->getElementType();

	if (eType != eTargetType) {
		DebugAssertMsg(false, "%d가 %s타입이 아니고, %s입니다.", progressBarCode, UIElementType::Name[eTargetType], UIElementType::Name[eType]);
		return nullptr;
	}


	return (SGUIProgressBar*)pElem;
}

SGUIScrollBar* SGUIManager::getScrollBar(int scrollBarCode) {
	constexpr UIElementType_t eTargetType = UIElementType::ScrollBar;

	if (!m_hUIElements.Exist(scrollBarCode)) {
		DebugAssertMsg("%s(%d)를 찾지 못했습니다.", UIElementType::Name[eTargetType], scrollBarCode);
		return nullptr;
	}

	SGUIElement* pElem = m_hUIElements[scrollBarCode];
	const UIElementType_t eType = pElem->getElementType();

	if (eType != eTargetType) {
		DebugAssertMsg(false, "%d가 %s타입이 아니고, %s입니다.", scrollBarCode, UIElementType::Name[eTargetType], UIElementType::Name[eType]);
		return nullptr;
	}

	return (SGUIScrollBar*)pElem;
}

SGUIStatic* SGUIManager::getStatic(int staticCode) {
	constexpr UIElementType_t eTargetType = UIElementType::Static;

	if (!m_hUIElements.Exist(staticCode)) {
		DebugAssertMsg("%s(%d)를 찾지 못했습니다.", UIElementType::Name[eTargetType], staticCode);
		return nullptr;
	}

	SGUIElement* pElem = m_hUIElements[staticCode];
	const UIElementType_t eType = pElem->getElementType();

	if (eType != eTargetType) {
		DebugAssertMsg(false, "%d가 %s타입이 아니고, %s입니다.", staticCode, UIElementType::Name[eTargetType], UIElementType::Name[eType]);
		return nullptr;
	}


	return (SGUIStatic*)pElem;
}


