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

	void OnClickedChatSendButton(ChatBox* chatBox);
	void OnClickedLeaveGameButton(TextButton* btn);

	/* =================================================================================
	 *                             통신 패킷 처리
	 * ================================================================================*/

	void CmdBattleFieldLoadAck(ICommand* cmd);
	void CmdBattleFieldLeaveAck(ICommand* cmd);
	void CmdBattleFieldTankUpdateSyn(ICommand* cmd);
	void CmdRoomGameStartAck(ICommand* cmd);
	void CmdBattleFieldPlayWaitEndSyn(ICommand* cmd);
	void CmdBattleFieldPlayingEndSyn(ICommand* cmd);
	void CmdBattleFieldEndWaitEndSyn(ICommand* cmd);
	void CmdChatMessageAck(ICommand* cmd);
	void CmdBattleFieldFireAck(ICommand* cmd);
	void CmdBattleFieldDeathAck(ICommand* cmd);
	void CmdBattleFieldRevivalAck(ICommand* cmd);
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

	JCore::Vector<Tank*> m_OtherPlayers;	// 다른 플레이어 탱크
	JCore::Vector<Bullet*> m_OtherBullets;	// 다른 플레이어가 쏜 총알들

	inline static const int ms_iPlayerSendMoveStatePacketDelay = 0.1f;		// 플레이어가 몇초마다 자신의 위치를 서버에 전송할지

	static const char* ms_pPlayWaitLeftTimeFormat;
	static const char* ms_pPlayingLeftTimeFormat;
	static const char* ms_pEndWaitLeftTimeFormat;
};


