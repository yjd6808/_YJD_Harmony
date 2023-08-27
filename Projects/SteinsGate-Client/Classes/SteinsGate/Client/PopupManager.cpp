/*
 * 작성자: 윤정도
 * 생성일: 4/26/2023 12:33:39 AM
 * =====================
 *
 */

#include "Tutturu.h"
#include "GameCoreHeader.h"
#include "PopupManager.h"

#include <SteinsGate/Client/Define_UI.h>

USING_NS_CC;
USING_NS_CCUI;
USING_NS_JC;

PopupManager::PopupManager()
	: m_fWidth(250.0f)
	, m_fPadding(10.0f)
	, m_qPopupPool(8)
{}

PopupManager::~PopupManager() {
	releaseAll();
}

UI_Popup* PopupManager::createPopup() {
	UIElementInfo* pInfo = Core::DataManager->getUIElementInfo(GROUP_UI_POPUP);
	DebugAssertMsg(pInfo->Type == UIElementType::Group, "그룹 엘리먼트 타입이 아닙니다.");
	UIMasterGroup* popup = dbg_new UI_Popup(static_cast<UIGroupInfo*>(pInfo));
	popup->autorelease();
	popup->setTag(InvalidValue_v);
	popup->init();
	popup->initChildren();
	popup->onInit();
	popup->setRelativePosition(0, 0, HAlignment::Center, VAlignment::Center);
	popup->load();
	
	return (UI_Popup*)popup;
}


void PopupManager::setWidth(float width) {
	m_fWidth = width;
}

void PopupManager::setPadding(float padding) {
	m_fPadding = padding;
}


UI_Popup* PopupManager::pop() {
	UI_Popup* popup = nullptr;
	if (m_qPopupPool.IsEmpty()) {
		popup = createPopup();
		popup->retain();
	} else {
		popup = m_qPopupPool.Front();
		popup->setTag(InvalidValue_v);	// 태그 초기화 해줘야함
		m_qPopupPool.Dequeue();
	}

	popup->setClosed(false);
	return popup;
}

UI_Popup* PopupManager::showYesNo(
	const std::string& text,
	const PopupCallback& yes /* = nullptr */,
	const PopupCallback& no /* = nullptr */,
	bool closeWithEsc /* = false */,
	float timeout /* = SG_POPUP_NO_TIMEOUT */,
	const PopupCallback& timeoutFn /*= nullptr*/,
	HAlignment_t halign /* = HAlignment::Left */,
	VAlignment_t valign /* = VAlignment::Top */
) {
	UI_Popup* popup = pop();
	popup->setText(text);
	popup->setType(UI_Popup::Type::eYesNo);
	popup->setYesCallback(yes);
	popup->setNoCallback(no);
	popup->setCloseWithEsc(closeWithEsc);
	popup->setTimeout(timeout);
	popup->setTimeoutCallback(timeoutFn);
	popup->setTextHAlign(halign);
	popup->setTextVAlign(valign);
	popup->adjust();
	Core::Contents.World->getUILayer()->addUIGroup(popup);
	m_vOpendList.PushBack(popup);
	return popup;
}

UI_Popup* PopupManager::showYesNo(
	const std::string& text,
	int tag,
	const PopupCallback& yes /* = nullptr */,
	const PopupCallback& no /* = nullptr */,
	bool closeWithEsc /* = false */,
	float timeout /* = SG_POPUP_NO_TIMEOUT */,
	const PopupCallback& timeoutFn /*= nullptr*/,
	HAlignment_t halign /* = HAlignment::Left */,
	VAlignment_t valign /* = VAlignment::Top */
) {
	UI_Popup* pOpendPopup = findByTag(tag);
	if (pOpendPopup != nullptr) {
		return pOpendPopup;
	}

	UI_Popup* popup = pop();
	popup->setTag(tag);
	popup->setText(text);
	popup->setType(UI_Popup::Type::eYesNo);
	popup->setYesCallback(yes);
	popup->setNoCallback(no);
	popup->setCloseWithEsc(closeWithEsc);
	popup->setTimeout(timeout);
	popup->setTimeoutCallback(timeoutFn);
	popup->setTextHAlign(halign);
	popup->setTextVAlign(valign);
	popup->adjust();
	Core::Contents.World->getUILayer()->addUIGroup(popup);
	m_vOpendList.PushBack(popup);
	return popup;
}

UI_Popup* PopupManager::showOk(
	const std::string& text,
	const PopupCallback& ok /* = nullptr */,
	bool closeWithEsc /* = false */,
	float timeout /* = SG_POPUP_NO_TIMEOUT */,
	const PopupCallback& timeoutFn /*= nullptr*/,
	HAlignment_t halign /* = HAlignment::Left */,
	VAlignment_t valign /* = VAlignment::Top */
) {
	UI_Popup* popup = pop();
	popup->setText(text);
	popup->setType(UI_Popup::Type::eOk);
	popup->setOkCallback(ok);
	popup->setCloseWithEsc(closeWithEsc);
	popup->setTimeout(timeout);
	popup->setTimeoutCallback(timeoutFn);
	popup->setTextHAlign(halign);
	popup->setTextVAlign(valign);
	popup->adjust();
	Core::Contents.World->getUILayer()->addUIGroup(popup);
	m_vOpendList.PushBack(popup);
	return popup;
}

UI_Popup* PopupManager::showOk(
	const std::string& text,
	int tag,
	const PopupCallback& ok /* = nullptr */,
	bool closeWithEsc /* = false */,
	float timeout /* = SG_POPUP_NO_TIMEOUT */,
	const PopupCallback& timeoutFn /*= nullptr*/,
	HAlignment_t halign /* = HAlignment::Left */,
	VAlignment_t valign /* = VAlignment::Top */
) {
	UI_Popup* pOpendPopup = findByTag(tag);
	if (pOpendPopup != nullptr) {
		return pOpendPopup;
	}

	UI_Popup* popup = pop();
	popup->setTag(tag);
	popup->setText(text);
	popup->setType(UI_Popup::Type::eOk);
	popup->setOkCallback(ok);
	popup->setCloseWithEsc(closeWithEsc);
	popup->setTimeout(timeout);
	popup->setTimeoutCallback(timeoutFn);
	popup->setTextHAlign(halign);
	popup->setTextVAlign(valign);
	popup->adjust();
	Core::Contents.World->getUILayer()->addUIGroup(popup);
	m_vOpendList.PushBack(popup);
	return popup;
}

UI_Popup* PopupManager::showNone(
	const std::string& text,
	bool closeWithEsc /* = false */,
	float timeout /* = SG_POPUP_NO_TIMEOUT */,
	const PopupCallback& timeoutFn /* = nullptr */,
	HAlignment_t halign /* = HAlignment::Center */,
	VAlignment_t valign /* = VAlignment::Center */
) {
	UI_Popup* popup = pop();
	popup->setText(text);
	popup->setType(UI_Popup::Type::eNone);
	popup->setCloseWithEsc(closeWithEsc);
	popup->setTimeout(timeout);
	popup->setTimeoutCallback(timeoutFn);
	popup->setTextHAlign(halign);
	popup->setTextVAlign(valign);
	popup->adjust();
	Core::Contents.World->getUILayer()->addUIGroup(popup);
	m_vOpendList.PushBack(popup);
	return popup;
}

UI_Popup* PopupManager::showNone(
	const std::string& text,
	int tag,
	bool closeWithEsc /* = false */,
	float timeout /* = SG_POPUP_NO_TIMEOUT */,
	const PopupCallback& timeoutFn /* = nullptr */,
	HAlignment_t halign /* = HAlignment::Center */,
	VAlignment_t valign /* = VAlignment::Center */
) {
	UI_Popup* pOpendPopup = findByTag(tag);
	if (pOpendPopup != nullptr) {
		return pOpendPopup;
	}

	UI_Popup* popup = pop();
	popup->setTag(tag);
	popup->setText(text);
	popup->setType(UI_Popup::Type::eNone);
	popup->setCloseWithEsc(closeWithEsc);
	popup->setTimeout(timeout);
	popup->setTimeoutCallback(timeoutFn);
	popup->setTextHAlign(halign);
	popup->setTextVAlign(valign);
	popup->adjust();
	Core::Contents.World->getUILayer()->addUIGroup(popup);
	m_vOpendList.PushBack(popup);
	return popup;
}

bool PopupManager::close(UI_Popup* popup) {

	if (popup->isClosed()) {
		return false;
	}

	popup->setClosed(true);
	Core::Contents.World->getUILayer()->removeUIGroup(popup);
	m_qPopupPool.Enqueue(popup);
	return m_vOpendList.Remove(popup);
}

bool PopupManager::closeByTag(int tag) {
	UI_Popup** pFind = m_vOpendList.Extension().FindIf([tag](UI_Popup* popup) { return popup->getTag() == tag; });

	if (pFind == nullptr) {
		return false;
	}

	return close(*pFind);
}

UI_Popup* PopupManager::findByTag(int tag) {
	for (int i = 0; i < m_vOpendList.Size(); ++i) {
		if (m_vOpendList[i]->getTag() == tag) {
			return m_vOpendList[i];
		}
	}
	return nullptr;
}

int PopupManager::closeAll() {
	int iClosedCount = 0;
	for (int i = 0; i < m_vOpendList.Size(); ++i) {
		if (close(m_vOpendList[i])) {
			--i;
			++iClosedCount;
		}
	}
	return iClosedCount;
}

void PopupManager::releaseAll() {

	// 열린 팝업창 모두 제거
	for (int i = 0; i < m_vOpendList.Size(); ++i) {
		Core::Contents.World->getUILayer()->removeUIGroup(m_vOpendList[i]);
		m_vOpendList[i]->release();
	}

	m_vOpendList.Clear();

	// 풀에 있는 팝업창 모두 제거
	while (!m_qPopupPool.IsEmpty()) {
		m_qPopupPool.Front()->release();
		m_qPopupPool.Dequeue();
	}
}


