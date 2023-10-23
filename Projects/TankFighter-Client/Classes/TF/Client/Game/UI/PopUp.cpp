
/*
	생성일 : 2019/03/05
	작성자 : 윤정도
	수정일 : 2022/04/03
 */

#include "Pch.h"
#include "PopUp.h"

#include <TF/Client/Game/UI/TextButton.h>
#include <TF/Client/Util/Color.h>

USING_NS_CC;
USING_NS_CCUI;

#define SPRITE_POPUPWINDOW		0
#define LABEL_POPUPTEXT			1
#define BUTTON_POPUP_CONFIRM	2
#define BUTTON_POPUP_YES		3
#define BUTTON_POPUP_NO 		4

PopUp* PopUp::createInRunningScene(const char* text, bool bgClickable, std::function<void()> callback) {
	auto pScene = Director::getInstance()->getRunningScene();
	if (pScene == nullptr) {
		_LogWarn_("%s", text);
		return nullptr;
	}
	return createInParent(text, pScene, bgClickable, callback);
}

PopUp* PopUp::createInRunningScene(const char* text, bool bgClickable, std::function<void()> yesFunc, std::function<void()> noFunc) {
	auto pScene = Director::getInstance()->getRunningScene();
	if (pScene == nullptr) {
		_LogWarn_("%s", text);
		return nullptr;
	}
	return createInParent(text, pScene, bgClickable, yesFunc, noFunc);
}

PopUp* PopUp::createInParent(const char * text, Node* parent, bool bgClickable, std::function<void()> callback)
{
	PopUp *ret = new (std::nothrow) PopUp();
	ret->setType(CONFIRM);
	ret->setText(text);
	ret->setParent(parent);
	ret->setConfirm(callback);
	if (ret &&ret->init(bgClickable))
	{
		ret->autorelease();
		parent->addChild(ret, 6000);
		return ret;
	}
	else
	{
		CC_SAFE_DELETE(ret);
		return nullptr;
	}
}


PopUp * PopUp::createInParent(const char * text, Node * parent, bool bgClickable, std::function<void()> yesFunc, std::function<void()> noFunc)
{
	PopUp *ret = new (std::nothrow) PopUp();
	ret->setType(YESNO);
	ret->setText(text);
	ret->setParent(parent);
	ret->setYes(yesFunc);
	ret->setNo(noFunc);
	if (ret && ret->init(bgClickable))
	{
		ret->autorelease();
		parent->addChild(ret, 6000);
		return ret;
	}
	else
	{
		CC_SAFE_DELETE(ret);
		return nullptr;
	}
}


bool PopUp::init(bool bgClickable)
{
	if (!Layer::init())
		return false;

	Size winSize = Director::getInstance()->getWinSize();

	if (bgClickable == false) {
		Button* pBackground = Button::create(Const::Resource::WhiteRectFileName);
		pBackground->setColor(Color3B::BLACK);
		pBackground->setOpacity(150);
		pBackground->setAnchorPoint(ZERO);
		pBackground->setPosition(ZERO);
		pBackground->setScale(25.0f);
		this->addChild(pBackground, 0);
	}

	Scale9Sprite* pWindoiw = Scale9Sprite::create(Const::Resource::WhiteRectFileName);
	pWindoiw->setColor(ColorList::Englishlavender_v);
	pWindoiw->setAnchorPoint(CENTER);
	pWindoiw->setPosition(Vec2(winSize.width / 2, winSize.height / 2));
	pWindoiw->setContentSize(Size(300.0f, 300.0f));
	this->addChild(pWindoiw, 6000, SPRITE_POPUPWINDOW);

	Text* pText = Text::create(getText(), Const::Resource::FontName, 20);
	pText->setTextAreaSize(pWindoiw->getContentSize() - Size(40.0f, 40.0f));
	pText->setTextHorizontalAlignment(TextHAlignment::CENTER);
	pText->setColor(ColorList::Black_v);
	pText->setPosition(Vec2(winSize.width / 2, winSize.height / 2));
	this->addChild(pText, 6001, LABEL_POPUPTEXT);

	if (getType() == CONFIRM) {
		TextButton* confBtn = TextButton::create(100, 50, "확인", 15);
		confBtn->setPosition(Vec2(pWindoiw->getPosition().x, pWindoiw->getPosition().y - 90.0f));
		confBtn->setBackgroundColor(ColorList::Beige_v);
		confBtn->setFontColor(ColorList::Black_v);
		confBtn->setClickEvent([&](TextButton* btn) {
			auto func = this->getConfirm();
			if (func != nullptr) 
				this->getConfirm()();

			this->getParent()->removeChild(this);
		});
		confBtn->setAnchorPoint(CENTER);
		this->addChild(confBtn, 6001, BUTTON_POPUP_CONFIRM);
	} else if (getType() == YESNO) {
		TextButton* yesBtn = TextButton::create(100, 50, "예", 15);
		yesBtn->setPosition(Vec2(pWindoiw->getPosition().x - 60.0f, pWindoiw->getPosition().y - 90.0f));
		yesBtn->setBackgroundColor(ColorList::Beige_v);
		yesBtn->setFontColor(ColorList::Black_v);
		yesBtn->setClickEvent( [&](TextButton* btn) {
			this->getYes()();
			this->getParent()->removeChild(this);
		});
		yesBtn->setAnchorPoint(CENTER);
		this->addChild(yesBtn, 6001, BUTTON_POPUP_YES);

		TextButton* noBtn = TextButton::create(100, 50, "아니오", 15);
		noBtn->setPosition(Vec2(pWindoiw->getPosition().x + 60.0f, pWindoiw->getPosition().y - 90.0f));
		noBtn->setBackgroundColor(ColorList::Beige_v);
		noBtn->setFontColor(ColorList::Black_v);
		noBtn->setClickEvent([&](TextButton* btn) {
			this->getNo()();
			this->getParent()->removeChild(this);
		});
		noBtn->setAnchorPoint(CENTER);
		this->addChild(noBtn, 6001, BUTTON_POPUP_NO);
	}
	return true;
}

bool PopUp::init() {
	return Layer::init();
}



