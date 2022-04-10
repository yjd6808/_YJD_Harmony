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

	void OnFireTank(Bullet* bullet);				// �� ��ũ�� ���� �� ���
	void OnClickedChatSendButton(ChatBox* chatBox);
	void OnClickedLeaveGameButton(TextButton* btn);
	JCore::String GetRoomMemberName(int characterUID) const;
	bool IsDeath(int characterUID);
	void SetDeath(int characterUID, bool isDeath);
	void UpdateUIModeByRoomState();

	/* =================================================================================
	 *                             ��� ��Ŷ ó��
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
	float m_fRevivalLeftTime = 0.0f;		// �װ��� ��Ȱ�Ҷ����� ���� �ð�

	RoomState m_eRoomState;
	Tank* m_pTank;
	Text* m_pTimeText;						// �����ð� �����ִ� �ؽ�Ʈ
	Text* m_pNoticeText;					// ��Ʋ�ʵ� �̺�Ʈ ���� �ؽ�Ʈ
	Text* m_pRevivalText;					// �װ��� ��Ȱ�Ҷ����� �ߴ� �ؽ�Ʈ
	TextButton* m_pLeaveChannelButton;
	ChatBox* m_pChatBox;
	Button* m_pClickPrevenButton;			// �װ��� ��� Ȱ���� ���ϰ��ϱ� ���� �Ƕ��� �������� �����.

	Text* m_pNameText[ROOM_MAX_PLAYER_COUNT];
	Text* m_pKillText[ROOM_MAX_PLAYER_COUNT];
	Text* m_pDeathText[ROOM_MAX_PLAYER_COUNT];

	JCore::Vector<Bullet*>	m_MyBullets;	// ���� �� �Ѿ˵�
	JCore::Vector<Tank*>	m_OtherPlayers;	// �ٸ� �÷��̾� ��ũ
	JCore::Vector<Bullet*>	m_OtherBullets;	// �ٸ� �÷��̾ �� �Ѿ˵�

	// RoomLayer�� �ִ� �� ���� �״�� �ʿ�
	// ��Ʋ�ʵ嵵 �ᱹ ���̹Ƿ�
	int m_iRoomMemberCount = 0;
	int m_iHostCharacterUID = INVALID_UID;
	RoomCharacterInfo m_RoomMember[ROOM_MAX_PLAYER_COUNT]{};

	inline static const int ms_iPlayerSendMoveStatePacketDelay = 0.1f;		// �÷��̾ ���ʸ��� �ڽ��� ��ġ�� ������ ��������

	static const char* ms_pPlayWaitLeftTimeFormat;
	static const char* ms_pPlayingLeftTimeFormat;
	static const char* ms_pEndWaitLeftTimeFormat;


};
