/*
 * �ۼ��� : ������
 */

#pragma once

#include <TF/SourceHeader.h>
#include <TF/UI/TextButton.h>
#include <TF/UI/ColoredListView.h>

class LobbyLayer : public Layer
{
public:
	bool init() override;
	void onEnterTransitionDidFinish() override;
	CREATE_FUNC(LobbyLayer);

	void OnClickedCreateRoomButton(TextButton* btn);				// �����	
	void OnClickedTerminateGameButton(TextButton* btn);				// ���� ���� 
	void OnClickedSelectChannelButton(TextButton* btn);				// ä�� ���� 
	void OnClickedJoinRoomButton(TextButton* btn);					// �� ����
	void OnClickedFriendListButton(TextButton* btn);				// ģ�� ��� Ŭ����
	void OnClickedAddFriendButton(TextButton* btn);					// ģ�� �߰�
	void OnClickedDeleteFriendButton(TextButton* btn);				// ģ�� ����
	void OnClickedMyInfoButton(TextButton* btn);					// �� ���� Ŭ����

	/* =================================================================================
	 *                             ��� ��Ŷ ó��
	 * ================================================================================*/

	void CmdUpdateCharacterInfoAck(ICommand* cmd);
	void CmdUpdateRoomListAck(ICommand* cmd);
	void CmdUpdateFriendListAck(ICommand* cmd);
	void CmdCreateRoomAck(ICommand* cmd);
	void CmdJoinRoomAck(ICommand* cmd);
	void CmdAddFriendAck(ICommand* cmd);
	void CmdAddFriendRequestSyn(ICommand* cmd);
private:
	ColoredListView* m_pRoomListView;
	EditBox* m_pRoomTitleEditBox;
	TextButton* m_pCreateRoomBtn;
	EditBox* m_pFriendNameEditBox;
	TextButton* m_AddFriendNameButton;
	TextButton* m_pFriendListText;
	ColoredListView* m_pFriendListView;
	TextButton* m_pTerminateGameButton;
	TextButton* m_pSelectChannelButton;
	TextButton* m_pNyInfoButton;
};


