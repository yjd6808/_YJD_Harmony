/*
 * 작성자: 윤정도
 * 생성일: 10/13/2023 2:30:51 PM
 * =====================
 *
 */


#pragma once

#include <TF/Client/Game/Scene/BaseScene.h>

class RoomScene : public BaseScene
{
public:
	RoomScene();
	~RoomScene() override;

	CREATE_FUNC(RoomScene)

	bool init() override;
	void onEnterTransitionDidFinish() override;

	Type getType() const override { return Room; }


	void onClickedGameStartButton(TextButton* btn);
	void onClickedGameReadyButton(TextButton* btn);
	void onClickedGameReadyCancelButton(TextButton* btn);
	void onClickedGameLeaveRoomButton(TextButton* btn);

	void refreshRoomInfo(RoomInfo& info);
	void refreshRoomMemberInfoList(RoomCharacterInfo* roomCharacterInfoList, int count, int hostCharacterPrimaryKey);
	void refreshRoomMemberReadyState(int memberIndex);
	void refreshRoomMemberInfo(int memberIndex, RoomCharacterInfo* info);

	void updateRoomMemberReadyState(int characterPrimaryKey, bool ready);

	RoomCharacterInfo* getRoomMemberByPrimaryKey(int characterPrimaryKey);
	int getRoomMemberIndexByPrimaryKey(int characterPrimaryKey);
private:
	TextButton* m_pGameStartBtn;
	TextButton* m_pGameReadyBtn;
	TextButton* m_pGameReadyCancelBtn;
	TextButton* m_pLeaveRoomBtn;
	TextButton* m_pRoomTitle;
	TextButton* m_pSlot[Const::Room::MaxPlayerCount];
	TextButton* m_pMarkSlot[Const::Room::MaxPlayerCount];

	int m_iHostCharacterPrimaryKey;
	RoomCharacterInfo m_RoomMember[Const::Room::MaxPlayerCount];
	RoomInfo m_RoomInfo;
};

