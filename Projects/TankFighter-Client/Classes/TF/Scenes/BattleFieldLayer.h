/*
 * 작성자 : 윤정도
 */


#pragma once

#include <TF/SourceHeader.h>
#include <TF/UI/TextButton.h>
#include <JCore/Container/Vector.h>

class BattleFieldLayer : public Layer
{
public:
	BattleFieldLayer() {}
	bool init() override;
	void update(float delta) override;
	void onEnterTransitionDidFinish() override;
	
	CREATE_FUNC(BattleFieldLayer);

	void UpdateTankMove(int characterUID, TankMove& move);

	void OnClickedLeaveGameButton(TextButton* btn);

	/* =================================================================================
	 *                             통신 패킷 처리
	 * ================================================================================*/

	void CmdBattleFieldLoadAck(ICommand* cmd);
	void CmdBattleFieldLeaveAck(ICommand* cmd);
	void CmdBattleFieldTankUpdateSyn(ICommand* cmd);
private:
	float m_fLeftTime = 0;
	float m_fSendTankMoveSynTime = 0;

	RoomState m_eRoomState;
	Tank* m_pTank;
	Text* m_pTimeText;			// 남은시간 보여주는 텍스트
	TextButton* m_pLeaveChannelButton;

	JCore::Vector<Tank*> m_OtherPlayers;

	inline static const int ms_iPlayerSendMoveStatePacketDelay = 0.1f;		// 플레이어가 몇초마다 자신의 위치를 서버에 전송할지;

	static const char* ms_pPlayWaitLeftTimeFormat;
	static const char* ms_pPlayingLeftTimeFormat;
	static const char* ms_pEndWaitLeftTimeFormat;
};
