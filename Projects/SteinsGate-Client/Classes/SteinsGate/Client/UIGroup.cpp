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


#define SG_CURSOR_POSITION_GUARD(mouse_event, cursor_pos) UIGroup::CursorPositionGuard JCORE_CONCAT_COUNTER(__guard__)(mouse_event, cursor_pos)

UIGroup::UIGroup(UIMasterGroup* master, UIGroup* parent)
	: UIElement(master, parent)
	, m_pInfo(nullptr)
{}

UIGroup::UIGroup(UIMasterGroup* master, UIGroup* parent, UIGroupInfo* groupInfo, bool infoOwner)
	: UIElement(master, parent, groupInfo, infoOwner)
	, m_pInfo(groupInfo)
{}

UIGroup::~UIGroup() {
}

UIGroup* UIGroup::create(UIMasterGroup* master, UIGroup* parent) {
	UIGroup* pGroup = dbg_new UIGroup(master, parent);
	pGroup->init();
	pGroup->autorelease();
	return pGroup;
}

UIGroup* UIGroup::create(UIMasterGroup* master, UIGroup* parent, UIGroupInfo* groupInfo, bool infoOwner) {
	UIGroup* pGroup = dbg_new UIGroup(master, parent, groupInfo, infoOwner);
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

	if (m_pInfo == nullptr) {
		logWarnMissingInfo();
		return;
	}

	for (int i = 0; i < m_pInfo->InfoList.Size(); ++i) {
		UIGroupElemInfo* elemInfo = &m_pInfo->InfoList[i];
		addUIElement(elemInfo);
	}

	initChildrenPosition();
}

void UIGroup::initChildrenPosition() {

	for (int i = 0; i < m_pInfo->InfoList.Size(); ++i) {

		UIElement* pElem = static_cast<UIElement*>(_children.at(i));
		const UIGroupElemInfo& elemInfo = m_pInfo->InfoList[i];
		pElem->setRelativePosition(
			elemInfo.Pos.x * Core::ClientInfo->UIScaleXFactor,
			elemInfo.Pos.y * Core::ClientInfo->UIScaleYFactor
		);

		if (pElem->isGroup()) {
			static_cast<UIGroup*>(pElem)->initChildrenPosition();
		}
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
	static SGVector<UIElement*> s_vDeveloperCreated;

	if (m_bLoaded == false)
		return;

	s_vDeveloperCreated.Clear();

	forEachRecursive([](UIElement* elem) {
		if (elem->isDeveloperCreated()) {
			s_vDeveloperCreated.PushBack(elem);
		}

		elem->unload();
	});

	for (int i = 0; i < s_vDeveloperCreated.Size(); ++i) {
		removeChild(s_vDeveloperCreated[i]);
	}

	m_bLoaded = false;

	if (isMasterGroup()) {
		m_pMasterGroup->onUnloaded();
	}
}

void UIGroup::addChild(UIElement* child) {
	Node::addChild(child);
}


bool UIGroup::onMouseDownInternal(SGEventMouse* mouseEvent) {
	const SGVec2 mousePos = mouseEvent->getCursorPos();
	const SGVec2 relativePos = mousePos - _position;
	mouseEvent->setCursorPosition(relativePos);
	SG_CURSOR_POSITION_GUARD(mouseEvent, mousePos);
	

	for (int i = _children.size() - 1; i >= 0; --i) {
		UIElement* pElem = static_cast<UIElement*>(_children.at(i));
		if (!pElem->onMouseDownInternal(mouseEvent)) {
			return false;
		}
	}

	return UIElement::onMouseDownInternal(mouseEvent);
}

bool UIGroup::onMouseMoveInternal(SGEventMouse* mouseEvent) {
	const SGVec2 mousePos = mouseEvent->getCursorPos();
	const SGVec2 relativePos = mousePos - _position;
	mouseEvent->setCursorPosition(relativePos);
	SG_CURSOR_POSITION_GUARD(mouseEvent, mousePos);

	for (int i = _children.size() - 1; i >= 0; --i) {
		UIElement* pElem = static_cast<UIElement*>(_children.at(i));

		if (!pElem->onMouseMoveInternal(mouseEvent))
			return false;
	}

	return UIElement::onMouseMoveInternal(mouseEvent);
}

bool UIGroup::onMouseUpInternal(SGEventMouse* mouseEvent) {
	// 계층적 상대좌표로 변환
	const SGVec2 mousePos = mouseEvent->getCursorPos();
	const SGVec2 relativePos = mousePos - _position;
	mouseEvent->setCursorPosition(relativePos);
	SG_CURSOR_POSITION_GUARD(mouseEvent, mousePos);

	for (int i = _children.size() - 1; i >= 0; --i) {
		UIElement* pElem = static_cast<UIElement*>(_children.at(i));
		if (!pElem->onMouseUpInternal(mouseEvent))
			return false;
	}

	return UIElement::onMouseUpInternal(mouseEvent);
}

bool UIGroup::onMouseScrollInternal(SGEventMouse* mouseEvent) {
	const SGVec2 mousePos = mouseEvent->getCursorPos();
	const SGVec2 relativePos = mousePos - _position;
	mouseEvent->setCursorPosition(relativePos);
	SG_CURSOR_POSITION_GUARD(mouseEvent, mousePos);

	for (int i = _children.size() - 1; i >= 0; --i) {
		UIElement* pElem = static_cast<UIElement*>(_children.at(i));
		if (!pElem->onMouseScrollInternal(mouseEvent))
			return false;
	}

	return UIElement::onMouseScrollInternal(mouseEvent);
}

UIElement* UIGroup::getAt(int index) {
	if (index >= _children.size()) {
		_LogWarn_("%d 그룹에서 %d번째 인덱스 원소를 찾지 못했습니다.", m_pBaseInfo->Code, index);
		return nullptr;
	}

	return static_cast<UIElement*>(_children.at(index));
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
	UIElementInfo* pElemInfo = Core::DataManager->getUIElementInfo(groupElemInfo->Code);
	UIElement* pChildElement = nullptr;

	switch (pElemInfo->Type) {
	case UIElementType::Group: pChildElement = UIGroup::create(m_pMasterGroup, this, static_cast<UIGroupInfo*>(pElemInfo), false); break;
	case UIElementType::Button: pChildElement = UIButton::create(m_pMasterGroup, this, static_cast<UIButtonInfo*>(pElemInfo), false); break;
	case UIElementType::Label: pChildElement = UILabel::create(m_pMasterGroup, this, static_cast<UILabelInfo*>(pElemInfo), false); break;
	case UIElementType::Sprite: pChildElement = UISprite::create(m_pMasterGroup, this, static_cast<UISpriteInfo*>(pElemInfo), false); break;
	case UIElementType::EditBox: pChildElement = UIEditBox::create(m_pMasterGroup, this, static_cast<UIEditBoxInfo*>(pElemInfo), false); break;
	case UIElementType::CheckBox: pChildElement = UICheckBox::create(m_pMasterGroup, this, static_cast<UICheckBoxInfo*>(pElemInfo), false); break;
	case UIElementType::ToggleButton: pChildElement = UIToggleButton::create(m_pMasterGroup, this, static_cast<UIToggleButtonInfo*>(pElemInfo), false); break;
	case UIElementType::ProgressBar: pChildElement = UIProgressBar::create(m_pMasterGroup, this, static_cast<UIProgressBarInfo*>(pElemInfo), false); break;
	case UIElementType::ScrollBar: pChildElement = UIScrollBar::create(m_pMasterGroup, this, static_cast<UIScrollBarInfo*>(pElemInfo), false); break;
	case UIElementType::Static: pChildElement = UIStatic::create(m_pMasterGroup, this, static_cast<UIStaticInfo*>(pElemInfo), false); break;
	default: _LogWarn_("알 수 없는 타입의 엘리먼트를 추가할려고했습니다. (%d)", pElemInfo->Type); return;
	}

	if (pChildElement == nullptr) {
		_LogError_("해당하는 UI 엘리먼트 타입의 자식을 생성하지 못했습니다.");
		return;
	}

	if (pChildElement->isGroup())
		static_cast<UIGroup*>(pChildElement)->initChildren();

	this->addChild(pChildElement);
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

void UIGroup::forEachRecursiveContainedSelf(const SGActionFn<UIElement*>& action) const {
	action((UIElement*)this);
	forEachRecursive(action);
}

void UIGroup::forEach(const SGActionFn<UIElement*>& action) const {
	for (int i = 0; i < _children.size(); ++i) {
		action(static_cast<UIElement*>(_children.at(i)));
	}
}

void UIGroup::forEachContainedSelf(const SGActionFn<UIElement*>& action) const {
	action((UIElement*)this);
	forEach(action);
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

void UIGroup::setInfo(UIElementInfo* info, bool infoOwner) {
	if (info->Type != UIElementType::Group) {
		logWarnInvalidInfo(info->Type);
		return;
	}

	if (m_bInfoOwner) {
		JCORE_DELETE_SAFE(m_pInfo);
	}

	m_pBaseInfo = info;
	m_pInfo = static_cast<UIGroupInfo*>(info);
	m_bInfoOwner = infoOwner;
}

void UIGroup::setInfoGroup(UIGroupInfo* info, bool infoOwner) {
	setInfo(info, infoOwner);
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

