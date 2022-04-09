/*0
 * �ۼ��� : ������
 */

#pragma once

enum class SceneType
{
	Login,
	Channel,
	CharacterSelect,
	Lobby,
	Room,
	BattleField
};

enum class MoveDirection
{
	None,
	Forward,
	Backward
};

enum class RotateDirection
{
	None,
	Right,
	Left
};

enum class ObstacleShape
{
	None,
	Rectangle,			// �簢��
	Circle,				// ����
	Rhombus				// ������
};

enum class PlayerState
{
	Disconnected	= -1,		// ���� ���� ���� Ȥ�� �ʱ�ȭ�� ����
	LoginStage		= 0,		// �α��� ��������
	ChannelSelect	= 1,		// ä�� ���� ��������
	CharacterSelect = 2,		// ĳ���� ���� ��������
	Lobby			= 3,		// ä�� �κ�
	RoomLobby		= 4,		// �� �κ� �ִ� ����
	RoomBattle		= 5			// �濡�� ������ ������ ����
};

enum class RoomState
{
	ReadyWait,		// �� ���� ��� �÷��̾ �غ��ϱ⸦ ��ٸ��� �ܰ�
	PlayWait,		// �� ���� ��� �÷��̾ ��Ʋ�ʵ� ������ ��ٸ��� �ܰ� - �� ��Ʋ�ʵ� Ÿ�̸� 5�� ���� - 5�ʳ��� ��� �÷��̾� ���Ծ��ϸ� �׳� ����
					// 5�� ������ ���� ���� ���� - ��� �÷��̾�� ���� ��������� �˸��� Playing ���·� �̵� �� Ÿ�̸� ����
	Playing,		// ���� ���� ��
	EndWait			// ���ӽð��� ��� ����ǰ� ���� ����� �����ִ� �ܰ� Ÿ�̸� 10�ʵ��� �����ϰ� �ٵǸ� ��� �κ�� �̵���Ŵ
};

enum class ChannelState
{
	Clean,		// ������
	Busy,		// ȥ����
	Full		// ����
};