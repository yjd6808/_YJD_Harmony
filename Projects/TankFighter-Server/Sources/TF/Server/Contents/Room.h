/*
 * 작성자: 윤정도
 * 생성일: 10/14/2023 1:22:52 PM
 * =====================
 *
 */


#pragma once

#include <JCore/Container/Vector.h>

#include <TF/Common/Base/IUpdatable.h>
#include <TF/Common/Base/IBroadcastable.h>

#include <TF/Server/Contents/Player.h>

class Room : public JCore::AccessibleObjectPool<Room>, public IBroadcastable, public IUpdatable
{
public:
	Room() {}

	void OnUpdate(const JCore::TimeSpan& elapsed) override;
	void BroadcastPacket(JNetwork::ISendPacket* packet) override;
	void OnPopped() override;
	void OnPushed() override;

	static Room Empty;
private:
	JCore::Vector<Player*> m_vPlayerList;
};


