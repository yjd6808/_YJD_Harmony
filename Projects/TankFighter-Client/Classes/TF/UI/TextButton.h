/*
 * �ۼ��� : ������
 * �ܼ��� �ؽ�Ʈ�� ���̴� ��ư
 *
 * �� ���ӿ����� ����Ҹ��� UI ������Ʈ
 *
 * ���콺�� �� ��ư���� �ø��� �ܼ��� ���� ��¦ �����ش�.
 * Ŭ���ϴ� ������ Scale�� 1.2������ Ŀ�����Ѵ�.
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

	bool onTouchBegin(Touch* touch, Event* touchEvent);
	void onTouchEnded(Touch* touch, Event* touchEvent);
	void setBackgroundColor(const Color3B& color);

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
