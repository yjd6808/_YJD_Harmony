/*
 * 작성자 : 윤정도
 */


#pragma once

#include <TF/SourceHeader.h>
#include <TF/UI/TextButton.h>

class ChatBox : public LayerColor
{
public:
	ChatBox() = default;
	virtual ~ChatBox() = default;
	static ChatBox* create(const Color3B& color, const Size& size);
	bool init(const Size& size);

	void onKeyPressed(EventKeyboard::KeyCode keyCode, Event* event) override;
	void onKeyReleased(EventKeyboard::KeyCode keyCode, Event* event) override;
	void SetContentSize(const Size& size);

	ListView* GetListView() const { return m_pListView; }
	EditBox* GetEditBox() const { return m_pChatEditBox; }
	void OnScrollViewEvent(Ref* pref, ScrollView::EventType type);
	void AddChatMessage(const JCore::String& message);
	void AddNoticeMessage(const JCore::String& message);
	void SetSendButtonClickEvent(JCore::Action<ChatBox*> clientEvent) { m_SendButtonClick = clientEvent; }
private:
	bool OnClickedSendButton(TextButton* btn);
private:
	JCore::Action<ChatBox*> m_SendButtonClick = nullptr;
	ListView* m_pListView;
	EditBox* m_pChatEditBox;
	TextButton* m_pSendButton;

	inline static constexpr float ms_fChatEditBoxHeight = 40.0f;
	inline static constexpr float ms_fChatSendButtonWidth = 40.0f;
};


