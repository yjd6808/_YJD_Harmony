/*
 * 작성자 : 윤정도
 */

#include <TF/UI/ChatBox.h>
#include <TF/Util/ColorUtil.h>

ChatBox* ChatBox::create(const Color3B& color, const Size& size)
{
	ChatBox *ret = new (std::nothrow) ChatBox();
	if (ret && ret->init(size) && ret->initWithColor(ColorUtil::To4B(color)))
	{
		ret->autorelease();
		return ret;
	}
	else
	{
		CC_SAFE_DELETE(ret);
		return nullptr;
	}
}

bool ChatBox::init(const Size& size)
{
	if (size.height <= ms_fChatEditBoxHeight || size.width <= ms_fChatSendButtonWidth) {
		DebugAssertMsg(false, "채팅창 높이가 40이기 때문에 채팅 박스의 높이는 최소 40이상으로 해주세요");
		return false; 
	}

	if (!LayerColor::init())
		return false;

	this->setContentSize(size);
	Size fullSize = this->getContentSize();
	

	m_pListView = ListView::create();
	m_pListView->setDirection(ScrollView::Direction::VERTICAL);
	m_pListView->setScrollBarEnabled(true);
	m_pListView->setPosition({ 0.0f, ms_fChatEditBoxHeight });
	m_pListView->pushBackDefaultItem();
	m_pListView->setScrollBarAutoHideEnabled(false);
	m_pListView->setAnchorPoint(Vec2::ZERO);
	m_pListView->addEventListener(CC_CALLBACK_2(ChatBox::OnScrollViewEvent, this));
	this->addChild(m_pListView);

	m_pChatEditBox = EditBox::create(Size(fullSize.width - ms_fChatSendButtonWidth, ms_fChatEditBoxHeight), Scale9Sprite::create(RECT_IMG_FILENAME));
	m_pChatEditBox->setFontColor(Color4B::WHITE);
	m_pChatEditBox->setColor(ColorList::Africanviolet_v);
	m_pChatEditBox->setFontSize(15.0f);
	m_pChatEditBox->setPlaceHolder("전송할 채팅 메시지 입력");
	m_pChatEditBox->setPosition(Vec2::ZERO);
	m_pChatEditBox->setAnchorPoint(Vec2::ZERO);
	m_pChatEditBox->setReturnType(EditBox::KeyboardReturnType::SEND);
	m_pChatEditBox->setPlaceholderFontColor(Color4B::BLACK);
	m_pChatEditBox->setInputMode(EditBox::InputMode::EMAIL_ADDRESS);
	this->addChild(m_pChatEditBox);

	m_pSendButton = TextButton::create(ms_fChatSendButtonWidth, ms_fChatEditBoxHeight, "전송", 16);
	m_pSendButton->setPosition({ fullSize.width - ms_fChatSendButtonWidth , 0.0f });
	m_pSendButton->setAnchorPoint(Vec2::ZERO);
	m_pSendButton->setClickEvent(CC_CALLBACK_1(ChatBox::OnClickedSendButton, this));
	this->addChild(m_pSendButton);

	EventListenerKeyboard* keyboardListener = EventListenerKeyboard::create();
	keyboardListener->onKeyPressed = CC_CALLBACK_2(ChatBox::onKeyPressed, this);
	keyboardListener->onKeyReleased = CC_CALLBACK_2(ChatBox::onKeyReleased, this);
	_eventDispatcher->addEventListenerWithSceneGraphPriority(keyboardListener, this);

	return true;
}

void ChatBox::onKeyPressed(EventKeyboard::KeyCode keyCode, cocos2d::Event* event) {

	// 이거 안됨 ㄷㄷ 포커싱이 안먹히네 ㅠㅠ
	// 엔터키 치면 Active Inacitve 전환되도록
	switch (keyCode) {
	case EventKeyboard::KeyCode::KEY_ENTER: {
		if (!m_pChatEditBox->isFocused()) {
			m_pChatEditBox->setFocused(true);
		} else {
			// 채팅 내용이 비어져있거나 전송 이벤트 등록이 안된경우 포커싱 비활성화
			if (!OnClickedSendButton(m_pSendButton)) {
				m_pChatEditBox->setFocused(false);
			}
		}
		break;
	}
	}
}

void ChatBox::onKeyReleased(EventKeyboard::KeyCode keyCode, cocos2d::Event* event) {
	// Not Implemented, Unused
}

void ChatBox::SetContentSize(const Size& size) {
	this->setContentSize(size);
	Size fullSize = this->getContentSize();
	m_pListView->setContentSize(fullSize - Size{ 0.0f, ms_fChatEditBoxHeight });
}

void ChatBox::OnScrollViewEvent(Ref* pref, ScrollView::EventType type) {
	// Not Implemented, Unused
}

void ChatBox::AddChatMessage(const JCore::String& message) {
	auto size = this->getContentSize();

	if (m_pListView->getItems().size() >= 30 ) {
		m_pListView->removeItem(1);
	}

	auto pText = Text::create(message.Source(), FONT_PATH_DEFAULT, 14);
	pText->setColor(ColorList::Black_v);
	pText->setTextAreaSize({ size.width - 30.0f, 0 });	// Wrap

	m_pListView->pushBackCustomItem(pText);
	m_pListView->scrollToBottom(0.0f, false);
	
	Sequence* seq = Sequence::create(FadeOut::create(0.3f), FadeIn::create(0.3f), NULL);
	Repeat* pRepeat = Repeat::create(seq, 1);
	pText->runAction(pRepeat);
}

void ChatBox::AddNoticeMessage(const JCore::String& message) {
	auto size = this->getContentSize();

	if (m_pListView->getItems().size() >= 30) {
		m_pListView->removeItem(1);
	}

	auto pText = Text::create(message.Source(), FONT_PATH_DEFAULT, 16);
	pText->setColor(ColorList::Bluepigment_v);
	pText->setTextAreaSize({ size.width - 30.0f, 0 });	// Wrap
	m_pListView->pushBackCustomItem(pText);
	m_pListView->scrollToBottom(0.0f, false);
	
	Sequence* seq = Sequence::create(FadeOut::create(0.3f), FadeIn::create(0.3f), NULL);
	Repeat* pRepeat = Repeat::create(seq, 1);
	pText->runAction(pRepeat);
}

bool ChatBox::OnClickedSendButton(TextButton* btn) {

	// 텍스트 입력이 안된 경우 스킵
	if (StringUtil::Length(m_pChatEditBox->getText()) <= 0) {
		return false;
	}

	if (m_SendButtonClick) {
		m_SendButtonClick(this);
		m_pChatEditBox->setText("");
		return true;
	}

	return false;
}
