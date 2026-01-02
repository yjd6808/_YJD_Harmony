/*
 * 작성자: 윤정도
 * 생성일: 8/21/2023 9:14:19 AM
 * =====================
 *
 */


#pragma once



class ChannelManager
	: public jc::SingletonStaticPointer<ChannelManager>
	, public IBroadcastable
{
	friend class TSingleton;
	ChannelManager();
	~ChannelManager() override;
public:
	void Initialize();
	void BroadcastPacket(jnet::IPacket* _pPacket) override;
};
