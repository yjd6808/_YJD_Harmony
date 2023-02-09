
/*
	생성일 : 2019/03/05
	작성자 : 윤정도
	수정일 : 2022/04/03
 */

#include <TF/UI/PopUp.h>
#include <TF/UI/TextButton.h>
#include <TF/Util/ColorUtil.h>

#define SPRITE_POPUPWINDOW		0
#define LABEL_POPUPTEXT			1
#define BUTTON_POPUP_CONFIRM	2
#define BUTTON_POPUP_YES		3
#define BUTTON_POPUP_NO 		4

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

	auto  paren = this->getParent();
	Size winSize = Director::getInstance()->getWinSize();

	if (bgClickable == false) {
		Button* bg = Button::create(RECT_IMG_FILENAME);
		bg->setColor(Color3B::BLACK);
		bg->setOpacity(150);
		bg->setAnchorPoint(ZERO);
		bg->setPosition(ZERO);
		bg->setScale(25.0f);
		this->addChild(bg, 0);
	}

	Scale9Sprite* popUpWindow = Scale9Sprite::create(RECT_IMG_FILENAME);
	popUpWindow->setColor(ColorList::Englishlavender_v);
	popUpWindow->setAnchorPoint(CENTER);
	popUpWindow->setPosition(Vec2(winSize.width / 2, winSize.height / 2));
	popUpWindow->setContentSize(Size(300.0f, 300.0f));
	this->addChild(popUpWindow, 6000, SPRITE_POPUPWINDOW);

	Text* popUpText = Text::create(getText(), FONT_PATH_DNF, 20);
	popUpText->setTextAreaSize(popUpWindow->getContentSize() - Size(40.0f, 40.0f));
	popUpText->setTextHorizontalAlignment(TextHAlignment::CENTER);
	popUpText->setColor(ColorList::Black_v);
	popUpText->setPosition(Vec2(winSize.width / 2, winSize.height / 2));
	this->addChild(popUpText, 6001, LABEL_POPUPTEXT);

	if (getType() == CONFIRM) {
		TextButton* confBtn = TextButton::create(100, 50, "확인", 15);
		confBtn->setPosition(Vec2(popUpWindow->getPosition().x, popUpWindow->getPosition().y - 90.0f));
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
		yesBtn->setPosition(Vec2(popUpWindow->getPosition().x - 60.0f, popUpWindow->getPosition().y - 90.0f));
		yesBtn->setClickEvent( [&](TextButton* btn) {
			this->getYes()();
			this->getParent()->removeChild(this);
		});
		yesBtn->setAnchorPoint(CENTER);
		this->addChild(yesBtn, 6001, BUTTON_POPUP_YES);

		TextButton* noBtn = TextButton::create(100, 50, "아니오", 15);
		noBtn->setPosition(Vec2(popUpWindow->getPosition().x + 60.0f, popUpWindow->getPosition().y - 90.0f));
		noBtn->setClickEvent(
			[&](TextButton* btn)
		{
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



