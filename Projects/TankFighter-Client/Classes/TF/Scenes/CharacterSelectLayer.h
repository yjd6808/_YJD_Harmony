#pragma once

#include <TF/SourceHeader.h>
#include <TF/UI/TextButton.h>

class CharacterSelectLayer : public Layer
{
public:
	bool init() override;
	CREATE_FUNC(CharacterSelectLayer);

	bool IsValidNickName(std::string& nick);
	void OnClickedCreateCharacterButton(TextButton* sender);
	void OnClickedDeleteCharacterButton(TextButton* sender);

	void CmdLoadCharacterInfoAck(ICommand* cmd);
	void CmdCreateCharacterAck(ICommand* cmd);
	void CmdDeleteCharacterAck(ICommand* cmd);
	
private:
	EditBox* m_pCharacterNickNameEditBox;
	TextButton* m_pCreateCharacterButton;
	TextButton* m_pDeleteCharacterButton;
};
