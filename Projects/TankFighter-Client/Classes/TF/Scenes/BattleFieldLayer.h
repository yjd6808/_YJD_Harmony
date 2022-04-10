/*
 * 작성자 : 윤정도
 */


#pragma once

#include <TF/SourceHeader.h>
#include <TF/UI/TextButton.h>
#include <TF/UI/ChatBox.h>
#include <JCore/Container/Vector.h>

class BattleFieldLayer : public Layer
{
public:
	BattleFieldLayer() {}
	bool init() override;
	void update(float delta) override;
	void onEnterTransitionDidFinish() override;
	


	CREATE_FUNC(BattleFieldLayer);

	void UpdateTankMove(TankMove& move);

	void OnFireTank(Bullet* bullet);				// 내 탱크가 총을 쏜 경우
	void OnClickedChatSendButton(ChatBox* chatBox);
	void OnClickedLeaveGameButton(TextButton* btn);
	JCore::String GetRoomMemberName(int characterUID) const;
	bool IsDeath(int characterUID);
	void SetDeath(int characterUID, bool isDeath);
	void UpdateUIModeByRoomState();

	/* =================================================================================
	 *                             통신 패킷 처리
	 * ================================================================================*/

	void CmdUpdateRoomInfoAck(ICommand* cmd);
	void CmdUpdateCharacterInfoAck(ICommand* cmd);
	void CmdBattleFieldLoadAck(ICommand* cmd);
	void CmdBattleFieldLeaveAck(ICommand* cmd) const;
	void CmdBattleFieldTankUpdateSyn(ICommand* cmd);
	void CmdRoomGameStartAck(ICommand* cmd) const;
	void CmdBattleFieldPlayWaitEndSyn(ICommand* cmd);
	void CmdBattleFieldPlayingEndSyn(ICommand* cmd);
	static void CmdBattleFieldEndWaitEndSyn(ICommand* cmd);
	void CmdChatMessageAck(ICommand* cmd) const;
	void CmdBattleFieldFireAck(ICommand* cmd);
	void CmdBattleFieldDeathAck(ICommand* cmd);
	void CmdBattleFieldRevivalSyn(ICommand* cmd);
	void CmdBattleFieldStatisticsUpdateSyn(ICommand* cmd) const;
private:
	float m_fLeftTime = 0;
	float m_fSendTankMoveSynTime = 0;
	float m_fRevivalLeftTime = 0.0f;		// 죽고나서 부활할때까지 남은 시간

	RoomState m_eRoomState;
	Tank* m_pTank;
	Text* m_pTimeText;						// 남은시간 보여주는 텍스트
	Text* m_pNoticeText;					// 배틀필드 이벤트 공지 텍스트
	Text* m_pRevivalText;					// 죽고나서 부활할때까지 뜨는 텍스트
	TextButton* m_pLeaveChannelButton;
	ChatBox* m_pChatBox;
	Button* m_pClickPrevenButton;			// 죽고나서 모든 활동을 못하게하기 위해 판때기 제일위에 쒸운다.

	Text* m_pNameText[ROOM_MAX_PLAYER_COUNT];
	Text* m_pKillText[ROOM_MAX_PLAYER_COUNT];
	Text* m_pDeathText[ROOM_MAX_PLAYER_COUNT];

	JCore::Vector<Bullet*>	m_MyBullets;	// 내가 쏜 총알들
	JCore::Vector<Tank*>	m_OtherPlayers;	// 다른 플레이어 탱크
	JCore::Vector<Bullet*>	m_OtherBullets;	// 다른 플레이어가 쏜 총알들

	// RoomLayer에 있는 방 정보 그대로 필요
	// 배틀필드도 결국 방이므로
	int m_iRoomMemberCount = 0;
	int m_iHostCharacterUID = INVALID_UID;
	RoomCharacterInfo m_RoomMember[ROOM_MAX_PLAYER_COUNT]{};

	inline static const int ms_iPlayerSendMoveStatePacketDelay = 0.1f;		// 플레이어가 몇초마다 자신의 위치를 서버에 전송할지

	static const char* ms_pPlayWaitLeftTimeFormat;
	static const char* ms_pPlayingLeftTimeFormat;
	static const char* ms_pEndWaitLeftTimeFormat;


};
