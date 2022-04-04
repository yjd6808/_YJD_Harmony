#pragma once

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
	Disconnected	= -1,
	LoginStage		= 0,
	ChannelSelect	= 1,
	Lobby			= 2,
	Room			= 3
};

enum class RoomState
{
	ReadyWait,		// ��� �÷��̾ �غ��ϱ⸦ ��ٸ��� �ܰ�
	StartWait,		// ���� ���� ��
	Playing			// ���� ���� ��
};

enum class ChannelState
{
	Clean,		// ������
	Busy,		// ȥ����
	Full		// ����
};