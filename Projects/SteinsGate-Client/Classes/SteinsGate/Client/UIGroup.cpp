/*
 * 작성자: 윤정도
 * 생성일: 2/15/2023 4:49:38 PM
 * =====================
 *
 */

#include "Tutturu.h"
#include "GameCoreHeader.h"
#include "UIGroup.h"

#include <SteinsGate/Client/UIMasterGroup.h>
#include <SteinsGate/Client/UIButton.h>
#include <SteinsGate/Client/UILabel.h>
#include <SteinsGate/Client/UISprite.h>
#include <SteinsGate/Client/UIEditBox.h>
#include <SteinsGate/Client/UICheckBox.h>
#include <SteinsGate/Client/UIToggleButton.h>
#include <SteinsGate/Client/UIScrollBar.h>
#include <SteinsGate/Client/UIProgressBar.h>
#include <SteinsGate/Client/UIStatic.h>

USING_NS_CC;
USING_NS_JC;

UIGroup::UIGroup(UIMasterGroup* master, UIGroup* parent, UIGroupInfo* groupInfo)
	: UIElement(master, parent, groupInfo)
	, m_pInfo(groupInfo)
{}

UIGroup::~UIGroup() {
}


UIGroup* UIGroup::createRetain(UIMasterGroup* master, UIGroup* parent, UIGroupInfo* groupInfo) {
	UIGroup* pGroup = dbg_new UIGroup(master, parent, groupInfo);
	pGroup->init();
	pGroup->retain();
	pGroup->autorelease();
	return pGroup;
}

UIGroup* UIGroup::create(UIMasterGroup* master, UIGroup* parent, UIGroupInfo* groupInfo) {
	UIGroup* pGroup = dbg_new UIGroup(master, parent, groupInfo);
	pGroup->init();
	pGroup->autorelease();
	return pGroup;
}

bool UIGroup::init() {

	if (!UIElement::init())
		return false;

	setInitialUISize(m_pInfo->Size);
	return m_bInitialized = true;
}

void UIGroup::initChildren() {
	for (int i = 0; i < m_pInfo->InfoList.Size(); ++i) {
		UIGroupElemInfo* elemInfo = &m_pInfo->InfoList[i];
		addUIElement(elemInfo);
	}
}

void UIGroup::load() {
	if (m_bLoaded)
		return;

	forEachRecursive([](UIElement* elem) {
		elem->load();
	});

	m_bLoaded = true;

	if (isMasterGroup()) {
		m_pMasterGroup->onLoaded();
	}
}

void UIGroup::unload() {
	if (m_bLoaded == false)
		return;

	forEachRecursive([](UIElement* elem) {
		elem->unload();
	});

	m_bLoaded = false;

	if (isMasterGroup()) {
		m_pMasterGroup->onUnloaded();
	}
}


bool UIGroup::onMouseMove(SGEventMouse* mouseEvent) {
	const SGVec2 mousePos = mouseEvent->getCursorPos();
	const SGVec2 relativePos = mousePos - _position;

	mouseEvent->setCursorPosition(relativePos.x, relativePos.y);

	for (int i = _children.size() - 1; i >= 0; --i) {
		UIElement* pElem = static_cast<UIElement*>(_children.at(i));

		if (!pElem->onMouseMove(mouseEvent))
			return false;
	}

	mouseEvent->setCursorPosition(mousePos.x, mousePos.y);	// 기존 상태로 복구
	return UIElement::onMouseMove(mouseEvent);
}

bool UIGroup::onMouseDown(SGEventMouse* mouseEvent) {
	

	const SGVec2 mousePos = mouseEvent->getCursorPos();
	const SGVec2 relativePos = mousePos - _position;

	mouseEvent->setCursorPosition(relativePos.x, relativePos.y);

	for (int i = _children.size() - 1; i >= 0; --i) {
		UIElement* pElem = static_cast<UIElement*>(_children.at(i));
		if (!pElem->onMouseDown(mouseEvent))
			return false;
	}

	mouseEvent->setCursorPosition(mousePos.x, mousePos.y);	// 기존 상태로 복구
	return UIElement::onMouseDown(mouseEvent);;
}

bool UIGroup::onMouseUp(SGEventMouse* mouseEvent) {
	// 계층적 상대좌표로 변환
	const SGVec2 mousePos = mouseEvent->getCursorPos();
	const SGVec2 relativePos = mousePos - _position;

	mouseEvent->setCursorPosition(relativePos.x, relativePos.y);

	for (int i = _children.size() - 1; i >= 0; --i) {
		UIElement* pElem = static_cast<UIElement*>(_children.at(i));
		if (!pElem->onMouseUp(mouseEvent))
			return false;
	}

	mouseEvent->setCursorPosition(mousePos.x, mousePos.y);	// 기존 상태로 복구
	return UIElement::onMouseUp(mouseEvent);
}

bool UIGroup::onMouseScroll(SGEventMouse* mouseEvent) {
	const SGVec2 mousePos = mouseEvent->getCursorPos();
	const SGVec2 relativePos = mousePos - _position;

	mouseEvent->setCursorPosition(relativePos.x, relativePos.y);

	for (int i = _children.size() - 1; i >= 0; --i) {
		UIElement* pElem = static_cast<UIElement*>(_children.at(i));
		if (!pElem->onMouseScroll(mouseEvent))
			return false;
	}

	mouseEvent->setCursorPosition(mousePos.x, mousePos.y);	// 기존 상태로 복구

	return UIElement::onMouseScroll(mouseEvent);
}

UIElement* UIGroup::findElement(int code) {
	UIElement* find = findElementRecursiveInternal(this, code);

	if (find == nullptr) {
		_LogWarn_("%d를 찾지 못했습니다.", code);
	}

	return find;
}

UIGroup* UIGroup::findGroup(int groupCode) { return findElementTemplated<UIGroup>(this, groupCode); }
UIButton* UIGroup::findButton(int buttonCode) { return findElementTemplated<UIButton>(this, buttonCode); }
UISprite* UIGroup::findSprite(int spriteCode) { return findElementTemplated<UISprite>(this, spriteCode); }
UILabel* UIGroup::findLabel(int labelCode) { return findElementTemplated<UILabel>(this, labelCode); }
UICheckBox* UIGroup::findCheckBox(int checkBoxCode) { return findElementTemplated<UICheckBox>(this, checkBoxCode); }
UIEditBox* UIGroup::findEditBox(int editBoxCode) { return findElementTemplated<UIEditBox>(this, editBoxCode); }
UIToggleButton* UIGroup::findToggleButton(int toggleButtonCode) { return findElementTemplated<UIToggleButton>(this, toggleButtonCode); }
UIProgressBar* UIGroup::findProgressBar(int progressBarCode) { return findElementTemplated<UIProgressBar>(this, progressBarCode); }
UIScrollBar* UIGroup::findScrollBar(int scrollBarCode) { return findElementTemplated<UIScrollBar>(this, scrollBarCode); }
UIStatic* UIGroup::findStatic(int staticCode) { return findElementTemplated<UIStatic>(this, staticCode); }


void UIGroup::addUIElement(UIGroupElemInfo* groupElemInfo) {
	UIElementInfo* pElemInfo = CoreDataManager_v->getUIElementInfo(groupElemInfo->Code);
	UIElement* pChildElement = nullptr;

	switch (pElemInfo->Type) {
	case UIElementType::Group: pChildElement = UIGroup::create(m_pMasterGroup, this, static_cast<UIGroupInfo*>(pElemInfo)); break;
	case UIElementType::Button: pChildElement = UIButton::create(m_pMasterGroup, this, static_cast<UIButtonInfo*>(pElemInfo)); break;
	case UIElementType::Label: pChildElement = UILabel::create(m_pMasterGroup, this, static_cast<UILabelInfo*>(pElemInfo)); break;
	case UIElementType::Sprite: pChildElement = UISprite::create(m_pMasterGroup, this, static_cast<UISpriteInfo*>(pElemInfo)); break;
	case UIElementType::EditBox: pChildElement = UIEditBox::create(m_pMasterGroup, this, static_cast<UIEditBoxInfo*>(pElemInfo)); break;
	case UIElementType::CheckBox: pChildElement = UICheckBox::create(m_pMasterGroup, this, static_cast<UICheckBoxInfo*>(pElemInfo)); break;
	case UIElementType::ToggleButton: pChildElement = UIToggleButton::create(m_pMasterGroup, this, static_cast<UIToggleButtonInfo*>(pElemInfo)); break;
	case UIElementType::ProgressBar: pChildElement = UIProgressBar::create(m_pMasterGroup, this, static_cast<UIProgressBarInfo*>(pElemInfo)); break;
	case UIElementType::ScrollBar: pChildElement = UIScrollBar::create(m_pMasterGroup, this, static_cast<UIScrollBarInfo*>(pElemInfo)); break;
	case UIElementType::Static: pChildElement = UIStatic::create(m_pMasterGroup, this, static_cast<UIStaticInfo*>(pElemInfo)); break;
	default: _LogWarn_("알 수 없는 타입의 엘리먼트를 추가할려고했습니다. (%d)", pElemInfo->Type); return;
	}

	if (pChildElement == nullptr) {
		_LogError_("해당하는 UI 엘리먼트 타입의 자식을 생성하지 못했습니다.");
		return;
	}

	this->addChild(pChildElement);
	pChildElement->setRelativePosition(groupElemInfo->Pos.x * CoreClientInfo_v->UIScaleXFactor, groupElemInfo->Pos.y * CoreClientInfo_v->UIScaleYFactor);
	
	if (pChildElement->isGroup())
		static_cast<UIGroup*>(pChildElement)->initChildren();
}

void UIGroup::forEachRecursive(const SGActionFn<UIElement*>& action) const {
	for (int i = 0; i < _children.size(); ++i) {
		UIElement* pElem = static_cast<UIElement*>(_children.at(i));

		if (pElem->getElementType() == UIElementType::Group) {
			UIGroup* pGroupElem = static_cast<UIGroup*>(pElem);
			action(pGroupElem);
			pGroupElem->forEachRecursive(action);
			continue;
		}

		action(pElem);
	}
}

void UIGroup::forEach(const SGActionFn<UIElement*>& action) const {
	for (int i = 0; i < _children.size(); ++i) {
		action(static_cast<UIElement*>(_children.at(i)));
	}
}


void UIGroup::restoreState(State state) {
	if (m_eState == eDisabled)
		return;

	forEachRecursive([state](UIElement* child) { child->restoreState(state); });
}

void UIGroup::setUISize(const SGSize& size) {

	if (!m_bResizable)
		return;

	Vec2 relativePos = getRelativePosition();
	m_UISize = size;

	const float fScaleX = getScaleX();
	const float fScaleY = getScaleY();

	// 자식요소간의 간격, 자식요소의 크기 모두 변경된 크기에 맞게 변환되어야한다.
	forEach([&](UIElement* child) {
		if (!child->isResizable())
			return;

		child->setUIScale(fScaleX, fScaleY);
	});

	setRelativePosition(relativePos);
}

UIElement* UIGroup::findElementRecursiveInternal(UIGroup* parent, int code) {
	for (int i = 0; i < parent->_children.size(); ++i) {
		UIElement* pElem = static_cast<UIElement*>(parent->_children.at(i));

		if (pElem->getCode() == code) {
			return pElem;
		}

		if (pElem->getElementType() == UIElementType::Group) {
			return findElementRecursiveInternal(static_cast<UIGroup*>(pElem), code);
		}
	}

	return nullptr;
}

