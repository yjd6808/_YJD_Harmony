/*
 * 작성자: 윤정도
 * 생성일: 2/15/2023 4:49:38 PM
 * =====================
 *
 */

#include "Tutturu.h"
#include "SGUIGroup.h"

#include <SteinsGate/Client/SGDataManager.h>
#include <SteinsGate/Client/SGUIButton.h>
#include <SteinsGate/Client/SGGlobal.h>


SGUIGroup::SGUIGroup()
	: SGUIElement(nullptr)
	, m_pInfo(nullptr)
{}

SGUIGroup::SGUIGroup(SGUIGroup* parent, SGUIGroupInfo* groupInfo)
	: SGUIElement(parent)
	, m_pInfo(groupInfo)
{}


SGUIGroup* SGUIGroup::createRetain(SGUIGroup* parent, SGUIGroupInfo* groupInfo) {
	SGUIGroup* pGroup = dbg_new SGUIGroup(parent, groupInfo);
	pGroup->init();
	pGroup->retain();
	pGroup->autorelease();
	return pGroup;
}

bool SGUIGroup::init() {

	if (!SGUIElement::init())
		return false;

	
	for (int i = 0; i < m_pInfo->InfoList.Size(); ++i) {
		SGUIGroupElemInfo& elemInfo = m_pInfo->InfoList[i];
		addUIElement(elemInfo);
	}

	setContentSize(m_pInfo->Rect.getSize());
	setPositionReal(m_pInfo->Rect.origin);

	
	return true;
}

void SGUIGroup::load() {
	if (m_bLoaded)
		return;

	forEach([](SGUIElement* elem) {
		elem->load();
	});

	m_bLoaded = true;
}

void SGUIGroup::unload() {
	if (m_bLoaded == false)
		return;

	forEach([](SGUIElement* elem) {
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
		SGUIElement* pElem = (SGUIElement*)_children.at(i);
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
		SGUIElement* pElem = (SGUIElement*)_children.at(i);
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
		SGUIElement* pElem = (SGUIElement*)_children.at(i);
		if (!pElem->onMouseUp(mouseEvent))
			return false;
	}
	return true;
}

bool SGUIGroup::onMouseScroll(SGEventMouse* mouseEvent) {
	return true;
}

int SGUIGroup::getCode() {
	return m_pInfo->Code;
}

void SGUIGroup::addUIElement(const SGUIGroupElemInfo& groupElemInfo) {
	SGDataManager* pDataManager = SGDataManager::get();
	SGUIElementInfo* pElemInfo = pDataManager->getUIElementInfo(groupElemInfo.Code);
	SGUIElement* pChildElement = nullptr;

	switch (pElemInfo->Type) {
	case UIElementType::Button: pChildElement = SGUIButton::create(this, (SGUIButtonInfo*)pElemInfo); break;
	default: break;
	}

	pChildElement->setPosition(groupElemInfo.X, groupElemInfo.Y);
	this->addChild(pChildElement);
}

void SGUIGroup::forEach(const SGActionFn<SGUIElement*>& iteration) {
	for (int i = _children.size() - 1; i >= 0; --i) {
		SGUIElement* pElem = (SGUIElement*)_children.at(i);

		if (pElem->getElementType() == UIElementType::Group) {
			SGUIGroup* pGroupElem = (SGUIGroup*)pElem;
			iteration(pGroupElem);
			pGroupElem->forEach(iteration);
			continue;
		}

		iteration(pElem);
	}
}

SGVec2 SGUIGroup::getPositionInRect(
	const SGRect& rc,
	float origin_x,
	float origin_y)
{
	float xPos = 0;
	float yPos = 0;

	switch (m_pInfo->HorizontalAlignment) {
	case HorizontalAlignment::Left:		xPos = 0;											break;
	case HorizontalAlignment::Center:	xPos = rc.getWidth() / 2 - getContentWidth() / 2;	break;
	case HorizontalAlignment::Right:	xPos = rc.getWidth() - getContentWidth();			break;
	}

	switch (m_pInfo->VerticalAlignment) {
	case VerticalAlignment::Bottom:	yPos = 0;											break;
	case VerticalAlignment::Center:	yPos = rc.getHeight() / 2 - getContentHeight() / 2;	break;
	case VerticalAlignment::Top:	yPos = rc.getHeight() - getContentHeight();			break;
	}

	return { xPos + origin_x, yPos + origin_y};
}


void SGUIGroup::setPositionReal(float x, float y) {
	if (m_pParent == nullptr) {
		setPositionRealNoParent(x, y);
		return;
	}

	setPositionRealHasParent(x, y);
}

void SGUIGroup::setPositionRealNoParent(float x, float y) {
	SGClientInfo* pClientInfo = SGDataManager::get()->getClientInfo();
	SGRect rect{ 0, 0, pClientInfo->ResolutionWidth, pClientInfo->ResolutionHeight };
	SGVec2 realPos = getPositionInRect(rect, x, y);
	setPosition(realPos);
}

void SGUIGroup::setPositionRealHasParent(float x, float y) {
	SGRect rect = getWorldBoundingBox();
	SGVec2 realPos = getPositionInRect(rect, x, y);
	setPosition(realPos);
}

void SGUIGroup::setPositionReal(const SGVec2& pos) {
	setPositionReal(pos.x, pos.y);
}

void SGUIGroup::restoreState(State state) {
	if (m_eState == eDisabled)
		return;

	forEach([state](SGUIElement* uiElement) { uiElement->restoreState(state); });
}

