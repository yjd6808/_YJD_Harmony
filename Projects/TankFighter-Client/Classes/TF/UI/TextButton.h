/*
 * 작성자 : 윤정도
 * 단순한 텍스트가 보이는 버튼
 *
 * 이 게임에서만 사용할만한 UI 오브젝트
 *
 * 마우스를 이 버튼위에 올리면 단순히 투명도 살짝 낮춰준다.
 * 클릭하는 동안은 Scale을 1.2배정도 커지게한다.
 */

#pragma once

#include <TF/SourceHeader.h>
#include <JCore/Functional.h>

class TextButton : public Button
{
public:
	CREATE_FUNC(TextButton);

	bool init2(float width, float height, const std::string& content, float fontSize);
	static TextButton* create(float width, float height, const std::string& content, float fontSize);

	void setFontColor(const Color3B& color);
	void onMouseMove(EventMouse* mouseEvent);

	bool IsMouseOver() const { return m_bMouseOver; }
	bool IsMousePressed() const { return m_bMousePressed; }

	bool onTouchBegin(Touch* touch, cocos2d::Event* touchEvent);
	void onTouchEnded(Touch* touch, cocos2d::Event* touchEvent);
	void setBackgroundColor(const Color3B& color);
	void setText(const std::string& text) { m_pTextUI->setText(text); }

	void onMouseClick(Ref* ref);
	void setClickEvent(JCore::Action<TextButton*> clientEvent) { m_ClickEvent = clientEvent; }
private:
	JCore::Action<TextButton*> m_ClickEvent;
	Scale9Sprite* m_pBackground;
	Button* m_pButton;
	Text* m_pTextUI;


	bool m_bMouseOver = false;;
	bool m_bMousePressed = false;
};
