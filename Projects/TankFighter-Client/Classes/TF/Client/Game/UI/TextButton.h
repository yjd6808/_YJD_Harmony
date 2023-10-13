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

#include <cocos2d.h>
#include <cocos/ui/CocosGUI.h>

#include <JCore/Functional.h>

class TextButton : public cocos2d::ui::Button
{
public:
	TextButton();

	CREATE_FUNC(TextButton);

	bool init2(float width, float height, const std::string& content, float fontSize);
	static TextButton* create(float width, float height, const std::string& content, float fontSize);

	void setFontColor(const cocos2d::Color3B& color);
	void onMouseMove(cocos2d::EventMouse* mouseEvent);

	bool IsMouseOver() const { return m_bMouseOver; }
	bool IsMousePressed() const { return m_bMousePressed; }

	bool onTouchBegin(cocos2d::Touch* touch, cocos2d::Event* touchEvent);
	void onTouchEnded(cocos2d::Touch* touch, cocos2d::Event* touchEvent);
	void setBackgroundColor(const cocos2d::Color3B& color);
	void setText(const std::string& text) { m_pTextUI->setText(text); }

	void onMouseClick(cocos2d::Ref* ref);
	void setClickEvent(JCore::Action<TextButton*> clientEvent) { m_ClickEvent = clientEvent; }
private:
	JCore::Action<TextButton*> m_ClickEvent;
	cocos2d::ui::Scale9Sprite* m_pBackground;
	cocos2d::ui::Button* m_pButton;
	cocos2d::ui::Text* m_pTextUI;

	bool m_bMouseOver;
	bool m_bMousePressed;
};
