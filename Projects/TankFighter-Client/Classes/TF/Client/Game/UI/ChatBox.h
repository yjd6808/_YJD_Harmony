/*
 * 작성자 : 윤정도
 */


#pragma once

#include <JCore/Primitives/String.h>

#include <TF/Client/Game/UI/TextButton.h>

class ChatBox : public cocos2d::LayerColor
{
public:
	ChatBox() = default;
	virtual ~ChatBox() = default;
	static ChatBox* create(const cocos2d::Color3B& color, const cocos2d::Size& size);
	bool init(const cocos2d::Size& size);

	void onKeyPressed(cocos2d::EventKeyboard::KeyCode keyCode, cocos2d::Event* event) override;
	void onKeyReleased(cocos2d::EventKeyboard::KeyCode keyCode, cocos2d::Event* event) override;
	void SetContentSize(const cocos2d::Size& size);

	cocos2d::ui::ListView* GetListView() const { return m_pListView; }
	cocos2d::ui::EditBox* GetEditBox() const { return m_pChatEditBox; }
	void OnScrollViewEvent(Ref* pref, cocos2d::ui::ScrollView::EventType type);
	void AddChatMessage(const JCore::String& message);
	void AddNoticeMessage(const JCore::String& message);
	void SetSendButtonClickEvent(JCore::Action<ChatBox*> clientEvent) { m_SendButtonClick = clientEvent; }
private:
	bool OnClickedSendButton(TextButton* btn);

	JCore::Action<ChatBox*> m_SendButtonClick = nullptr;
	cocos2d::ui::ListView* m_pListView;
	cocos2d::ui::EditBox* m_pChatEditBox;
	TextButton* m_pSendButton;

	inline static constexpr float ms_fChatEditBoxHeight = 50.0f;
	inline static constexpr float ms_fChatSendButtonWidth = 60.0f;
};


