/*
 * 작성자: 윤정도
 * 생성일: 10/13/2023 2:30:51 PM
 * =====================
 *
 */


#pragma once

#include <TF/Client/Game/Scene/BaseScene.h>

struct CharacterInfo;

class TextButton;

class CharacterSelectScene : public BaseScene
{
public:
	CharacterSelectScene();
	~CharacterSelectScene() override;

	CREATE_FUNC(CharacterSelectScene)

	bool init() override;
	void onEnterTransitionDidFinish() override;

	bool isValidNickName(std::string& nick);
	void onClickedCharacterButton(TextButton* btn);
	void onClickedCreateCharacterButton(TextButton* sender);
	void onClickedDeleteCharacterButton(TextButton* sender);
	void onClickedChannelSelectButton(TextButton* sender);

	void refreshCharacterList(CharacterInfo* characterList, int count);

	Type getType() const override { return CharacterSelect; }
private:
	cocos2d::ui::EditBox* m_pCharacterNickNameEditBox;
	TextButton* m_pCreateCharacterButton;
	TextButton* m_pDeleteCharacterButton;
	TextButton* m_pChannelSelectButton;

	JCore::Vector<TextButton*> m_vCharacterSelectButtonList;
};