/*
 * 작성자 : 윤정도
 */

#pragma once

#include <TF/SourceHeader.h>
#include <TF/UI/TextButton.h>

class CharacterSelectLayer : public Layer
{
public:
	CharacterSelectLayer() : m_CharacterSelectButtonVec(10) {}
	bool init() override;
	CREATE_FUNC(CharacterSelectLayer);

	void onEnterTransitionDidFinish() override;

	bool IsValidNickName(std::string& nick);
	void OnClickedCharacterButton(TextButton* btn);
	void OnClickedCreateCharacterButton(TextButton* sender);
	void OnClickedDeleteCharacterButton(TextButton* sender);
	void OnClickedChannelSelectButton(TextButton* sender);


	/* =================================================================================
	 *                             통신 패킷 처리
	 * ================================================================================*/

	void CmdLoadCharacterInfoAck(ICommand* cmd);
	void CmdSelectCharacterAck(ICommand* cmd);
	void CmdCreateCharacterAck(ICommand* cmd);
	void CmdDeleteCharacterAck(ICommand* cmd);
	
private:
	EditBox* m_pCharacterNickNameEditBox;
	TextButton* m_pCreateCharacterButton;
	TextButton* m_pDeleteCharacterButton;
	TextButton* m_pChannelSelectButton;

	// 로딩된 캐릭터 ID와 캐릭터 정보
	JCore::Vector<TextButton*> m_CharacterSelectButtonVec;
};
