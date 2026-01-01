/*
 * 작성자: 윤정도
 * 생성일: 8/21/2023 9:30:34 AM
 * =====================
 *
 */


#pragma once

#include <sgs_game/Game.h>
#include <sgs/ServerProcessInfo.h>

class Channel
{
public:
	Channel(GameChannelInfo* _pChannelInfo);

	GameChannelInfo* GetInfo() const { return info_; }
private:
	GameChannelInfo* info_;
};
