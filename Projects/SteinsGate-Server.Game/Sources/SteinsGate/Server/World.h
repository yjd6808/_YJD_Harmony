/*
 * 작성자: 윤정도
 * 생성일: 8/21/2023 9:14:19 AM
 * =====================
 *
 */


#pragma once

#include <JCore/Pattern/Singleton.h>

#include <SteinsGate/Common/IBroadcastable.h>

class World
	: public JCore::SingletonStaticPointer<World>
	, public IBroadcastable
{
	friend class TSingleton;
	World();
	~World() override;
public:
	void Initialize();
	void BroadcastPacket(JNetwork::ISendPacket* packet) override;
};