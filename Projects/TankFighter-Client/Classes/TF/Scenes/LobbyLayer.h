/*
 * 작성자 : 윤정도
 */

#pragma once

#include <TF/SourceHeader.h>
#include <TF/UI/TextButton.h>
#include <TF/UI/ColoredListView.h>
#include <TF/UI/ChatBox.h>

class LobbyLayer : public Layer
{
public:
	
	bool init() override;
	void onEnterTransitionDidFinish() override;
	
	CREATE_FUNC(LobbyLayer);

	void OnClickedCreateRoomButton(TextButton* btn);				// 방생성	
	void OnClickedTerminateGameButton(TextButton* btn);				// 게임 종료 
	void OnClickedSelectChannelButton(TextButton* btn);				// 채널 선택 
	void OnClickedJoinRoomButton(TextButton* btn);					// 방 참가
	void OnClickedFriendListButton(TextButton* btn);				// 친구 목록 클릭시
	void OnClickedAddFriendButton(TextButton* btn);					// 친구 추가
	void OnClickedDeleteFriendButton(TextButton* btn);				// 친구 제거
	void OnClickedMyInfoButton(TextButton* btn);					// 내 정보 클릭시
	void OnClickedChatSendButton(ChatBox* chatBox);

	/* =================================================================================
	 *                             통신 패킷 처리
	 * ================================================================================*/

	void CmdUpdateCharacterInfoAck(ICommand* cmd);
	void CmdUpdateRoomListAck(ICommand* cmd);
	void CmdUpdateFriendListAck(ICommand* cmd);
	void CmdCreateRoomAck(ICommand* cmd);
	void CmdJoinRoomAck(ICommand* cmd);
	void CmdAddFriendAck(ICommand* cmd);
	void CmdAddFriendRequestSyn(ICommand* cmd);
	void CmdChatMessageAck(ICommand* cmd);
private:
	ChatBox* m_pChatBox;
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


