/*
 * 작성자 : 윤정도
 */

#pragma once

#include <TF/SourceHeader.h>
#include <TF/UI/TextButton.h>

class RoomLayer : public Layer
{
public:
	RoomLayer(){}
	bool init() override;
	void onEnterTransitionDidFinish() override;
	
	CREATE_FUNC(RoomLayer);

	RoomCharacterInfo* MyInfo();

	void OnClickedGameStartButton(TextButton* btn);
	void OnClickedGameReadyButton(TextButton* btn);
	void OnClickedGameReadyCancelButton(TextButton* btn);
	void OnClickedGameLeaveRoomButton(TextButton* btn);

	/* =================================================================================
	 *                             통신 패킷 처리
	 * ================================================================================*/

	void CmdLoadRoomInfoAck(ICommand* cmd);
	void CmdUpdateRoomInfoAck(ICommand* cmd);
	void CmdRoomLeaveAck(ICommand* cmd);
	void CmdRoomGameStartAck(ICommand* cmd);
private:
	TextButton* m_pGameStartBtn;
	TextButton* m_pGameReadyBtn;
	TextButton* m_pGameReadyCancelBtn;
	TextButton* m_pLeaveRoomBtn;
	TextButton* m_pRoomTitle;

	TextButton* m_pSlot[ROOM_MAX_PLAYER_COUNT];
	TextButton* m_pMarkSlot[ROOM_MAX_PLAYER_COUNT];

	int m_iRoomMemberCount = 0;
	int m_iHostCharacterUID = INVALID_UID;
	RoomCharacterInfo m_RoomMember[ROOM_MAX_PLAYER_COUNT]{};
	RoomInfo m_RoomInfo{};
};
