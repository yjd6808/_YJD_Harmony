/*
 * �ۼ��� : ������
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
	 *                             ��� ��Ŷ ó��
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
	float m_fRevivalLeftTime = 0.0f;		// �װ��� ��Ȱ�Ҷ����� ���� �ð�

	RoomState m_eRoomState;
	Tank* m_pTank;
	Text* m_pTimeText;						// �����ð� �����ִ� �ؽ�Ʈ
	Text* m_pNoticeText;					// ��Ʋ�ʵ� �̺�Ʈ ���� �ؽ�Ʈ
	Text* m_pRevivalText;					// �װ��� ��Ȱ�Ҷ����� �ߴ� �ؽ�Ʈ
	TextButton* m_pLeaveChannelButton;
	ChatBox* m_pChatBox;
	Button* m_pClickPrevenButton;			// �װ��� ��� Ȱ���� ���ϰ��ϱ� ���� �Ƕ��� �������� �����.

	JCore::Vector<Tank*> m_OtherPlayers;	// �ٸ� �÷��̾� ��ũ
	JCore::Vector<Bullet*> m_OtherBullets;	// �ٸ� �÷��̾ �� �Ѿ˵�

	inline static const int ms_iPlayerSendMoveStatePacketDelay = 0.1f;		// �÷��̾ ���ʸ��� �ڽ��� ��ġ�� ������ ��������

	static const char* ms_pPlayWaitLeftTimeFormat;
	static const char* ms_pPlayingLeftTimeFormat;
	static const char* ms_pEndWaitLeftTimeFormat;
};


