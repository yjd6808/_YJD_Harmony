/*
 * 작성자: 윤정도
 * 생성일: 2/15/2023 8:11:04 PM
 * =====================
 *
 */

#include "Tutturu.h"
#include "UIManager.h"

#include <SteinsGate/Client/DataManager.h>
#include <SteinsGate/Client/ImagePackManager.h>
#include <SteinsGate/Client/UIGroup.h>
#include <SteinsGate/Client/UIMasterGroup.h>


USING_NS_CC;
USING_NS_JC;


UIManager::UIManager()
	: Master(nullptr)
	, m_hLoadedUITexture(1024) // 창을 64개까지 만들일이 있을려나 ㅋㅋ
	, m_hUIElements(512)
	, m_hMasterUIGroups(64)
{}

UIManager::~UIManager() {

	CC_SAFE_RELEASE(Master);

	// 마스터 UI 그룹만 제거해주면 된다.
	// 어차피 내부 자식들은 모두 마스터 UI 그룹에 addChild 되어 있기 때문에
	// 이녀석만 제거하면 도미노 마냥 다 제거댐
	m_hMasterUIGroups.Values().Extension().ForEach([](UIGroup* group) {
		CC_SAFE_RELEASE(group);
	});
}


// =====================================================
//           마스타 유아이 그룹 등록
// =====================================================

void UIManager::init() {
	Master = UIGroupMaster::createRetain();
	Master->forEach([this](UIMasterGroup* pMasterGroup) { registerMasterGroup(pMasterGroup); });
	Master->forEach([this](UIMasterGroup* pMasterGroup) { pMasterGroup->onInit(); });
}

void UIManager::registerMasterGroup(UIMasterGroup* group) {
	m_hUIElements.Insert(group->getCode(), group);
	m_hMasterUIGroups.Insert(group->getCode(), group);

	group->forEachRecursive([this](UIElement* uiElement) {
		const int code = uiElement->getCode();
		const bool bInsert = m_hUIElements.Insert(code, uiElement);
		DebugAssertMsg(bInsert, "이미 %d UI 엘리먼트가 그룹내 포함되어 있습니다.", code);
	});
}

void UIManager::registerLoadedUITexture(SgaResourceIndex index) {

	if (index.Un.FrameIndex == InvalidValue_v)
		return;

	m_hLoadedUITexture.Insert(index.Value, index);
}


void UIManager::unloadAll() {

	ImagePackManager* pPackManager = ImagePackManager::Get();

	// 이미지 텍스쳐 모두 릴리즈
	m_hMasterUIGroups.Values().Extension().ForEach([](UIGroup* group) {
		group->unload();
	});

	// 관련 캐쉬, 팩 모두 언로드
	m_hLoadedUITexture.Values().Extension().ForEach([pPackManager](SgaResourceIndex& resourceIndex) {
		pPackManager->releaseFrameTexture(resourceIndex);
		pPackManager->unloadPackData(resourceIndex.Un.SgaIndex);
	});
	m_hLoadedUITexture.Clear();
}

void UIManager::onUpdate(float dt) {
	callUIElementsUpdateCallback(dt);
}

void UIManager::callUIElementsUpdateCallback(float dt) {
	m_hUIElementsUpdateEvent.Extension().ForEach([&dt](Pair<UIElement*, SGEventList<UIElement*, float>>& pair) {
		pair.Value.Invoke((UIElement*)pair.Key, (float)dt);
	});
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

UIGroup* UIManager::getGroup(int groupCode) {
	constexpr UIElementType_t eTargetType = UIElementType::Group;

	if (!m_hUIElements.Exist(groupCode)) {
		_LogWarn_("%s(%d)를 찾지 못했습니다.", UIElementType::Name[eTargetType], groupCode);
		return nullptr;
	}

	UIElement* pElem = m_hUIElements[groupCode];
	const UIElementType_t eType = pElem->getElementType();

	if (eType != eTargetType) {
		_LogWarn_("%d가 %s타입이 아니고, %s입니다.", groupCode, UIElementType::Name[eTargetType], UIElementType::Name[eType]);
		return nullptr;
	}

	return (UIGroup*)pElem;
}

UIButton* UIManager::getButton(int buttonCode) {
	constexpr UIElementType_t eTargetType = UIElementType::Button;

	if (!m_hUIElements.Exist(buttonCode)) {
		_LogWarn_("%s(%d)를 찾지 못했습니다.", UIElementType::Name[eTargetType], buttonCode);
		return nullptr;
	}
	
	UIElement* pElem = m_hUIElements[buttonCode];
	const UIElementType_t eType = pElem->getElementType();

	if (eType != eTargetType) {
		_LogWarn_("%d가 %s타입이 아니고, %s입니다.", buttonCode, UIElementType::Name[eTargetType], UIElementType::Name[eType]);
		return nullptr;
	}

	return (UIButton*)pElem;
}

UISprite* UIManager::getSprite(int spriteCode) {
	constexpr UIElementType_t eTargetType = UIElementType::Sprite;

	if (!m_hUIElements.Exist(spriteCode)) {
		_LogWarn_("%s(%d)를 찾지 못했습니다.", UIElementType::Name[eTargetType], spriteCode);
		return nullptr;
	}

	UIElement* pElem = m_hUIElements[spriteCode];
	const UIElementType_t eType = pElem->getElementType();

	if (eType != eTargetType) {
		_LogWarn_("%d가 %s타입이 아니고, %s입니다.", spriteCode, UIElementType::Name[eTargetType], UIElementType::Name[eType]);
		return nullptr;
	}

	return (UISprite*)pElem;
}

UILabel* UIManager::getLabel(int labelCode) {
	constexpr UIElementType_t eTargetType = UIElementType::Label;

	if (!m_hUIElements.Exist(labelCode)) {
		_LogWarn_("%s(%d)를 찾지 못했습니다.", UIElementType::Name[eTargetType], labelCode);
		return nullptr;
	}

	UIElement* pElem = m_hUIElements[labelCode];
	const UIElementType_t eType = pElem->getElementType();

	if (eType != eTargetType) {
		_LogWarn_("%d가 %s타입이 아니고, %s입니다.", labelCode, UIElementType::Name[eTargetType], UIElementType::Name[eType]);
		return nullptr;
	}

	return (UILabel*)pElem;
}

UICheckBox* UIManager::getCheckBox(int checkBoxCode) {
	constexpr UIElementType_t eTargetType = UIElementType::CheckBox;

	if (!m_hUIElements.Exist(checkBoxCode)) {
		_LogWarn_("%s(%d)를 찾지 못했습니다.", UIElementType::Name[eTargetType], checkBoxCode);
		return nullptr;
	}

	UIElement* pElem = m_hUIElements[checkBoxCode];
	const UIElementType_t eType = pElem->getElementType();

	if (eType != eTargetType) {
		_LogWarn_("%d가 %s타입이 아니고, %s입니다.", checkBoxCode, UIElementType::Name[eTargetType], UIElementType::Name[eType]);
		return nullptr;
	}

	return (UICheckBox*)pElem;
}

UIEditBox* UIManager::getEditBox(int editBoxCode) {
	constexpr UIElementType_t eTargetType = UIElementType::EditBox;

	if (!m_hUIElements.Exist(editBoxCode)) {
		_LogWarn_("%s(%d)를 찾지 못했습니다.", UIElementType::Name[eTargetType], editBoxCode);
		return nullptr;
	}

	UIElement* pElem = m_hUIElements[editBoxCode];
	const UIElementType_t eType = pElem->getElementType();

	if (eType != eTargetType) {
		_LogWarn_("%d가 %s타입이 아니고, %s입니다.", editBoxCode, UIElementType::Name[eTargetType], UIElementType::Name[eType]);
		return nullptr;
	}

	return (UIEditBox*)pElem;
}

UIToggleButton* UIManager::getToggleButton(int toggleButtonCode) {
	constexpr UIElementType_t eTargetType = UIElementType::ToggleButton;

	if (!m_hUIElements.Exist(toggleButtonCode)) {
		_LogWarn_("%s(%d)를 찾지 못했습니다.", UIElementType::Name[eTargetType], toggleButtonCode);
		return nullptr;
	}

	UIElement* pElem = m_hUIElements[toggleButtonCode];
	const UIElementType_t eType = pElem->getElementType();

	if (eType != eTargetType) {
		_LogWarn_("%d가 %s타입이 아니고, %s입니다.", toggleButtonCode, UIElementType::Name[eTargetType], UIElementType::Name[eType]);
		return nullptr;
	}

	return (UIToggleButton*)pElem;
}

UIProgressBar* UIManager::getProgressBar(int progressBarCode) {
	constexpr UIElementType_t eTargetType = UIElementType::ProgressBar;

	if (!m_hUIElements.Exist(progressBarCode)) {
		_LogWarn_("%s(%d)를 찾지 못했습니다.", UIElementType::Name[eTargetType], progressBarCode);
		return nullptr;
	}

	UIElement* pElem = m_hUIElements[progressBarCode];
	const UIElementType_t eType = pElem->getElementType();

	if (eType != eTargetType) {
		_LogWarn_("%d가 %s타입이 아니고, %s입니다.", progressBarCode, UIElementType::Name[eTargetType], UIElementType::Name[eType]);
		return nullptr;
	}


	return (UIProgressBar*)pElem;
}

UIScrollBar* UIManager::getScrollBar(int scrollBarCode) {
	constexpr UIElementType_t eTargetType = UIElementType::ScrollBar;

	if (!m_hUIElements.Exist(scrollBarCode)) {
		_LogWarn_("%s(%d)를 찾지 못했습니다.", UIElementType::Name[eTargetType], scrollBarCode);
		return nullptr;
	}

	UIElement* pElem = m_hUIElements[scrollBarCode];
	const UIElementType_t eType = pElem->getElementType();

	if (eType != eTargetType) {
		_LogWarn_("%d가 %s타입이 아니고, %s입니다.", scrollBarCode, UIElementType::Name[eTargetType], UIElementType::Name[eType]);
		return nullptr;
	}

	return (UIScrollBar*)pElem;
}

UIStatic* UIManager::getStatic(int staticCode) {
	constexpr UIElementType_t eTargetType = UIElementType::Static;

	if (!m_hUIElements.Exist(staticCode)) {
		_LogWarn_("%s(%d)를 찾지 못했습니다.", UIElementType::Name[eTargetType], staticCode);
		return nullptr;
	}

	UIElement* pElem = m_hUIElements[staticCode];
	const UIElementType_t eType = pElem->getElementType();

	if (eType != eTargetType) {
		_LogWarn_("%d가 %s타입이 아니고, %s입니다.", staticCode, UIElementType::Name[eTargetType], UIElementType::Name[eType]);
		return nullptr;
	}


	return (UIStatic*)pElem;
}


