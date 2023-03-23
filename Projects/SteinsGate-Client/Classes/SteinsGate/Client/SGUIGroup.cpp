/*
 * 작성자: 윤정도
 * 생성일: 2/15/2023 4:49:38 PM
 * =====================
 *
 */

#include "Tutturu.h"
#include "GameCoreHeader.h"
#include "SGUIGroup.h"

#include <SteinsGate/Client/SGUIMasterGroup.h>
#include <SteinsGate/Client/SGUIButton.h>
#include <SteinsGate/Client/SGUILabel.h>
#include <SteinsGate/Client/SGUISprite.h>
#include <SteinsGate/Client/SGUIEditBox.h>
#include <SteinsGate/Client/SGUICheckBox.h>
#include <SteinsGate/Client/SGUIToggleButton.h>
#include <SteinsGate/Client/SGUIScrollBar.h>
#include <SteinsGate/Client/SGUIProgressBar.h>
#include <SteinsGate/Client/SGUIStatic.h>




SGUIGroup::SGUIGroup(SGUIMasterGroup* master, SGUIGroup* parent, SGUIGroupInfo* groupInfo)
	: SGUIElement(master, parent, groupInfo)
	, m_pInfo(groupInfo)
{}

SGUIGroup::~SGUIGroup() {
}


SGUIGroup* SGUIGroup::createRetain(SGUIMasterGroup* master, SGUIGroup* parent, SGUIGroupInfo* groupInfo) {
	SGUIGroup* pGroup = dbg_new SGUIGroup(master, parent, groupInfo);
	pGroup->init();
	pGroup->retain();
	pGroup->autorelease();
	return pGroup;
}

SGUIGroup* SGUIGroup::create(SGUIMasterGroup* master, SGUIGroup* parent, SGUIGroupInfo* groupInfo) {
	SGUIGroup* pGroup = dbg_new SGUIGroup(master, parent, groupInfo);
	pGroup->init();
	pGroup->autorelease();
	return pGroup;
}

bool SGUIGroup::init() {

	if (!SGUIElement::init())
		return false;

	setContentSize(m_pInfo->Size);
	return true;
}

void SGUIGroup::initChildren() {
	for (int i = 0; i < m_pInfo->InfoList.Size(); ++i) {
		SGUIGroupElemInfo& elemInfo = m_pInfo->InfoList[i];
		addUIElement(elemInfo);
	}
}

void SGUIGroup::load() {
	if (m_bLoaded)
		return;

	forEachRecursive([](SGUIElement* elem) {
		elem->load();
	});

	m_bLoaded = true;

	if (isMasterGroup()) {
		m_pMasterGroup->onLoaded();
	}
}

void SGUIGroup::unload() {
	if (m_bLoaded == false)
		return;

	forEachRecursive([](SGUIElement* elem) {
		elem->unload();
	});

	m_bLoaded = false;

	if (isMasterGroup()) {
		m_pMasterGroup->onUnloaded();
	}
}


bool SGUIGroup::onMouseMove(SGEventMouse* mouseEvent) {
	SGUIElement::onMouseMove(mouseEvent);

	const SGVec2 mousePos = mouseEvent->getCursorPos();
	const SGVec2 relativePos = mousePos - getPosition();

	mouseEvent->setCursorPosition(relativePos.x, relativePos.y);

	for (int i = _children.size() - 1; i >= 0; --i) {
		SGUIElement* pElem = static_cast<SGUIElement*>(_children.at(i));

		if (!pElem->onMouseMove(mouseEvent))
			return false;
	}

	mouseEvent->setCursorPosition(mousePos.x, mousePos.y);	// 기존 상태로 복구

	return true;
}

bool SGUIGroup::onMouseDown(SGEventMouse* mouseEvent) {
	SGUIElement::onMouseDown(mouseEvent);

	const SGVec2 mousePos = mouseEvent->getCursorPos();
	const SGVec2 relativePos = mousePos - getPosition();

	mouseEvent->setCursorPosition(relativePos.x, relativePos.y);

	for (int i = _children.size() - 1; i >= 0; --i) {
		SGUIElement* pElem = static_cast<SGUIElement*>(_children.at(i));
		if (!pElem->onMouseDown(mouseEvent))
			return false;
	}

	mouseEvent->setCursorPosition(mousePos.x, mousePos.y);	// 기존 상태로 복구

	return true;
}

bool SGUIGroup::onMouseUp(SGEventMouse* mouseEvent) {
	SGUIElement::onMouseUp(mouseEvent);
	// 계층적 상대좌표로 변환
	const SGVec2 mousePos = mouseEvent->getCursorPos();
	const SGVec2 relativePos = mousePos - getPosition();

	mouseEvent->setCursorPosition(relativePos.x, relativePos.y);

	for (int i = _children.size() - 1; i >= 0; --i) {
		SGUIElement* pElem = static_cast<SGUIElement*>(_children.at(i));
		if (!pElem->onMouseUp(mouseEvent))
			return false;
	}

	mouseEvent->setCursorPosition(mousePos.x, mousePos.y);	// 기존 상태로 복구
	return true;
}

bool SGUIGroup::onMouseScroll(SGEventMouse* mouseEvent) {
	SGUIElement::onMouseScroll(mouseEvent);

	const SGVec2 mousePos = mouseEvent->getCursorPos();
	const SGVec2 relativePos = mousePos - getPosition();

	mouseEvent->setCursorPosition(relativePos.x, relativePos.y);

	for (int i = _children.size() - 1; i >= 0; --i) {
		SGUIElement* pElem = static_cast<SGUIElement*>(_children.at(i));
		if (!pElem->onMouseScroll(mouseEvent))
			return false;
	}

	mouseEvent->setCursorPosition(mousePos.x, mousePos.y);	// 기존 상태로 복구

	return true;
}

void SGUIGroup::addUIElement(const SGUIGroupElemInfo& groupElemInfo) {
	SGUIElementInfo* pElemInfo = CoreDataManager_v->getUIElementInfo(groupElemInfo.Code);
	SGUIElement* pChildElement = nullptr;

	switch (pElemInfo->Type) {
	case UIElementType::Group: pChildElement = SGUIGroup::create(m_pMasterGroup, this, static_cast<SGUIGroupInfo*>(pElemInfo)); break;
	case UIElementType::Button: pChildElement = SGUIButton::create(m_pMasterGroup, this, static_cast<SGUIButtonInfo*>(pElemInfo)); break;
	case UIElementType::Label: pChildElement = SGUILabel::create(m_pMasterGroup, this,	static_cast<SGUILabelInfo*>(pElemInfo)); break;
	case UIElementType::Sprite: pChildElement = SGUISprite::create(m_pMasterGroup, this, static_cast<SGUISpriteInfo*>(pElemInfo)); break;
	case UIElementType::EditBox: pChildElement = SGUIEditBox::create(m_pMasterGroup, this, static_cast<SGUIEditBoxInfo*>(pElemInfo)); break;
	case UIElementType::CheckBox: pChildElement = SGUICheckBox::create(m_pMasterGroup, this, static_cast<SGUICheckBoxInfo*>(pElemInfo)); break;
	case UIElementType::ToggleButton: pChildElement = SGUIToggleButton::create(m_pMasterGroup, this, static_cast<SGUIToggleButtonInfo*>(pElemInfo)); break;
	case UIElementType::ProgressBar: pChildElement = SGUIProgressBar::create(m_pMasterGroup, this, static_cast<SGUIProgressBarInfo*>(pElemInfo)); break;
	case UIElementType::ScrollBar: pChildElement = SGUIScrollBar::create(m_pMasterGroup, this, static_cast<SGUIScrollBarInfo*>(pElemInfo)); break;
	case UIElementType::Static: pChildElement = SGUIStatic::create(m_pMasterGroup, this, static_cast<SGUIStaticInfo*>(pElemInfo)); break;
	default: _LogWarn_("알 수 없는 타입의 엘리먼트를 추가할려고했습니다. (%d)", pElemInfo->Type); return;
	}

	DebugAssertMsg(pChildElement, "해당하는 UI 엘리먼트 타입의 자식을 생성하지 못했습니다.");

	pChildElement->setPositionRelative(groupElemInfo.Pos.x, groupElemInfo.Pos.y);
	if (pChildElement->isGroup())
		static_cast<SGUIGroup*>(pChildElement)->initChildren();

	this->addChild(pChildElement);
}

void SGUIGroup::forEachRecursive(const SGActionFn<SGUIElement*>& action) const {
	for (int i = 0; i < _children.size(); ++i) {
		SGUIElement* pElem = static_cast<SGUIElement*>(_children.at(i));

		if (pElem->getElementType() == UIElementType::Group) {
			SGUIGroup* pGroupElem = static_cast<SGUIGroup*>(pElem);
			action(pGroupElem);
			pGroupElem->forEachRecursive(action);
			continue;
		}

		action(pElem);
	}
}

void SGUIGroup::forEach(const SGActionFn<SGUIElement*>& action) const {
	for (int i = 0; i < _children.size(); ++i) {
		action(static_cast<SGUIElement*>(_children.at(i)));
	}
}


void SGUIGroup::restoreState(State state) {
	if (m_eState == eDisabled)
		return;

	forEachRecursive([state](SGUIElement* uiElement) { uiElement->restoreState(state); });
}

