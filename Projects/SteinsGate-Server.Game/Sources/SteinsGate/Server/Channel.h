/*
 * 작성자: 윤정도
 * 생성일: 8/21/2023 9:30:34 AM
 * =====================
 *
 */


#pragma once

#include <SteinsGate/Server/Game.h>
#include <SteinsGate/Common/ServerProcessInfo.h>

class Channel
{
public:
	Channel(GameChannelInfo* channelInfo);

	GameChannelInfo* GetInfo() const { return m_pInfo; }
private:
	GameChannelInfo* m_pInfo;
};