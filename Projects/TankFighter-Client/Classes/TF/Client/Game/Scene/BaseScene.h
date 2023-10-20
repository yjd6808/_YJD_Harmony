/*
 * 작성자: 윤정도
 * 생성일: 10/13/2023 2:32:57 PM
 * =====================
 *
 */


#pragma once

#include <TF/Client/Game/Layer/GridLayer.h>

#include <cocos/ui/CocosGUI.h>

class BaseScene : public cocos2d::Scene
{
public:
	enum class Type
	{
		Login,
		Channel,
		CharacterSelect,
		Lobby,
		Room,
		BattleField
	};

	BaseScene();
	
	bool init() override;

	void update(float delta) override;
	virtual Type getType() const = 0;

	static constexpr const char* getTypeName(Type type) {
		switch (type) {
		case Type::Login: return "로그인";
		case Type::Channel: return "채널";
		case Type::CharacterSelect: return "캐릭터선택";
		case Type::Lobby: return "로비";
		case Type::Room: return "방";
		case Type::BattleField: return "배틀필드";
		}
		return "무슨?";
	}

	virtual void onKeyPressed(cocos2d::EventKeyboard::KeyCode keyCode, cocos2d::Event* event);
protected:
	cocos2d::Layer* m_pUILayer;
	GridLayer* m_pGridLayer;
	cocos2d::ui::Text* m_pNetInfo;
};