/*
 * �ۼ��� : ������
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
	 *                             ��� ��Ŷ ó��
	 * ================================================================================*/

	void CmdBattleFieldLoadAck(ICommand* cmd);
	void CmdBattleFieldLeaveAck(ICommand* cmd);
	void CmdBattleFieldTankUpdateSyn(ICommand* cmd);
private:
	float m_fLeftTime = 0;
	float m_fSendTankMoveSynTime = 0;

	RoomState m_eRoomState;
	Tank* m_pTank;
	Text* m_pTimeText;			// �����ð� �����ִ� �ؽ�Ʈ
	TextButton* m_pLeaveChannelButton;

	JCore::Vector<Tank*> m_OtherPlayers;

	inline static const int ms_iPlayerSendMoveStatePacketDelay = 0.1f;		// �÷��̾ ���ʸ��� �ڽ��� ��ġ�� ������ ��������;

	static const char* ms_pPlayWaitLeftTimeFormat;
	static const char* ms_pPlayingLeftTimeFormat;
	static const char* ms_pEndWaitLeftTimeFormat;
};
