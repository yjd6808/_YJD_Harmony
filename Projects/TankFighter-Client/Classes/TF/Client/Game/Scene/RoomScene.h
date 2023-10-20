/*
 * 작성자: 윤정도
 * 생성일: 10/13/2023 2:30:51 PM
 * =====================
 *
 */


#pragma once

#include <TF/Client/Game/Scene/BaseScene.h>
#include <TF/Client/Game/Contents/Room.h>

class RoomScene : public BaseScene
{
public:
	RoomScene(Room* room);
	~RoomScene() override;

	static RoomScene* create(Room* room);

	bool init() override;
	void onEnterTransitionDidFinish() override;

	Type getType() const override { return Type::Room; }

	void onClickedGameStartButton(TextButton* btn);
	void onClickedGameReadyButton(TextButton* btn);
	void onClickedGameReadyCancelButton(TextButton* btn);
	void onClickedGameLeaveRoomButton(TextButton* btn);

	void refreshRoomInfo();
	void refreshRoomMemberInfoList();
	void refreshRoomMemberReadyState(int memberIndex);
	void refreshRoomMemberInfo(int memberIndex);
private:
	Room* m_pRoom;

	TextButton* m_pGameStartBtn;
	TextButton* m_pGameReadyBtn;
	TextButton* m_pGameReadyCancelBtn;
	TextButton* m_pLeaveRoomBtn;
	TextButton* m_pRoomTitle;
	TextButton* m_pSlot[Const::Room::MaxPlayerCount];
	TextButton* m_pMarkSlot[Const::Room::MaxPlayerCount];
};

