#pragma once

#include <TF/SourceHeader.h>
#include <TF/UI/TextButton.h>

class CharacterSelectLayer : public Layer
{
public:
	CharacterSelectLayer() : m_CharacterSelectButtonVec(10) {}
	bool init() override;
	CREATE_FUNC(CharacterSelectLayer);

	bool IsValidNickName(std::string& nick);
	void OnClickedCharacterButton(TextButton* btn);
	void OnClickedCreateCharacterButton(TextButton* sender);
	void OnClickedDeleteCharacterButton(TextButton* sender);


	/* =================================================================================
	 *                             ��� ��Ŷ ó��
	 * ================================================================================*/

	void CmdLoadCharacterInfoAck(ICommand* cmd);
	void CmdSelectCharacterAck(ICommand* cmd);
	void CmdCreateCharacterAck(ICommand* cmd);
	void CmdDeleteCharacterAck(ICommand* cmd);
	
private:
	EditBox* m_pCharacterNickNameEditBox;
	TextButton* m_pCreateCharacterButton;
	TextButton* m_pDeleteCharacterButton;

	// �ε��� ĳ���� ID�� ĳ���� ����
	JCore::Vector<TextButton*> m_CharacterSelectButtonVec;
};
