/*
 * 작성자: 윤정도
 * 생성일: 2/15/2023 4:49:38 PM
 * =====================
 *
 */

#include "Tutturu.h"
#include "GameCoreHeader.h"
#include "SGUIGroup.h"

#include <SteinsGate/Client/SGUIButton.h>
#include <SteinsGate/Client/SGUIEditBox.h>
#include <SteinsGate/Client/SGUICheckBox.h>
#include <SteinsGate/Client/SGUIToggleButton.h>
#include <SteinsGate/Client/SGUIProgressBar.h>
#include <SteinsGate/Client/SGUIStatic.h>


SGUIGroup::SGUIGroup(SGUIGroup* parent, SGUIGroupInfo* groupInfo)
	: SGUIElement(parent, groupInfo)
	, m_pInfo(groupInfo)
{}

SGUIGroup::~SGUIGroup() {
}


SGUIGroup* SGUIGroup::createRetain(SGUIGroup* parent, SGUIGroupInfo* groupInfo) {
	SGUIGroup* pGroup = dbg_new SGUIGroup(parent, groupInfo);
	pGroup->init();
	pGroup->retain();
	pGroup->autorelease();
	return pGroup;
}

SGUIGroup* SGUIGroup::create(SGUIGroup* parent, SGUIGroupInfo* groupInfo) {
	SGUIGroup* pGroup = dbg_new SGUIGroup(parent, groupInfo);
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
}

void SGUIGroup::unload() {
	if (m_bLoaded == false)
		return;

	forEachRecursive([](SGUIElement* elem) {
		elem->unload();
	});

	m_bLoaded = false;
}


bool SGUIGroup::onMouseMove(SGEventMouse* mouseEvent) {

	// 마스타 UI 그룹은 셀프 호출
	if (m_pParent == nullptr) {
		SGUIElement::onMouseMove(mouseEvent);
	}

	for (int i = _children.size() - 1; i >= 0; --i) {
		SGUIElement* pElem = static_cast<SGUIElement*>(_children.at(i));
		if (!pElem->onMouseMove(mouseEvent))
			return false;
	}

	return true;
}

bool SGUIGroup::onMouseDown(SGEventMouse* mouseEvent) {

	if (m_pParent == nullptr) {
		SGUIElement::onMouseDown(mouseEvent);
	}
	

	for (int i = _children.size() - 1; i >= 0; --i) {
		SGUIElement* pElem = static_cast<SGUIElement*>(_children.at(i));
		if (!pElem->onMouseDown(mouseEvent))
			return false;
	}
	return true;
}

bool SGUIGroup::onMouseUp(SGEventMouse* mouseEvent) {

	if (m_pParent == nullptr) {
		SGUIElement::onMouseUp(mouseEvent);
	}

	for (int i = _children.size() - 1; i >= 0; --i) {
		SGUIElement* pElem = static_cast<SGUIElement*>(_children.at(i));
		if (!pElem->onMouseUp(mouseEvent))
			return false;
	}
	return true;
}

bool SGUIGroup::onMouseScroll(SGEventMouse* mouseEvent) {
	return true;
}

void SGUIGroup::addUIElement(const SGUIGroupElemInfo& groupElemInfo) {
	SGUIElementInfo* pElemInfo = CoreDataManager_v->getUIElementInfo(groupElemInfo.Code);
	SGUIElement* pChildElement = nullptr;

	/*if (groupElemInfo.Code == 2003)
		return;*/

	switch (pElemInfo->Type) {
	case UIElementType::Group: pChildElement = SGUIGroup::create(this, static_cast<SGUIGroupInfo*>(pElemInfo)); break;
	case UIElementType::Button: pChildElement = SGUIButton::create(this, static_cast<SGUIButtonInfo*>(pElemInfo)); break;
	case UIElementType::Label: pChildElement = SGUILabel::create(this,	static_cast<SGUILabelInfo*>(pElemInfo)); break;
	case UIElementType::Sprite: pChildElement = SGUISprite::create(this, static_cast<SGUISpriteInfo*>(pElemInfo)); break;
	case UIElementType::EditBox: pChildElement = SGUIEditBox::create(this, static_cast<SGUIEditBoxInfo*>(pElemInfo)); break;
	case UIElementType::CheckBox: pChildElement = SGUICheckBox::create(this, static_cast<SGUICheckBoxInfo*>(pElemInfo)); break;
	case UIElementType::ToggleButton: pChildElement = SGUIToggleButton::create(this, static_cast<SGUIToggleButtonInfo*>(pElemInfo)); break;
	case UIElementType::ProgressBar: pChildElement = SGUIProgressBar::create(this, static_cast<SGUIProgressBarInfo*>(pElemInfo)); break;
	case UIElementType::Static: pChildElement = SGUIStatic::create(this, static_cast<SGUIStaticInfo*>(pElemInfo)); break;
	
	default: return;
	// default: break; 임시로 리턴
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

