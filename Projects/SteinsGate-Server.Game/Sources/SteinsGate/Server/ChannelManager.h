/*
 * 작성자: 윤정도
 * 생성일: 8/21/2023 9:14:19 AM
 * =====================
 *
 */


#pragma once

#include <JCore/Pattern/Singleton.h>

#include <SteinsGate/Common/IBroadcastable.h>

class ChannelManager
	: public JCore::SingletonStaticPointer<ChannelManager>
	, public IBroadcastable
{
	friend class TSingleton;
	ChannelManager();
	~ChannelManager() override;
public:
	void Initialize();
	void BroadcastPacket(JNetwork::IPacket* packet) override;
};