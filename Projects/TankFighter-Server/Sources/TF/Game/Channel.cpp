#define _WINSOCKAPI_

#include <TF/Game/Channel.h>

using namespace JCore;

Channel::Channel(int channedUID, const String& channelName, int maxPlayerCount)
	: m_ChannelUID(channedUID)
	, m_ChannelName(channelName)
	, m_iMaxPlayerCount(maxPlayerCount)
	, m_iPlayerCount(0)
	, m_PlayerList(30) {
}

String Channel::GetChnnelName() {
	return m_ChannelName;
}

int Channel::GetChannelUID() {
	return m_ChannelUID;
}

int Channel::GetPlayerCount() {
	CriticalSectionLockGuard guard(m_ChannelLock);
	return m_iPlayerCount;
}

int Channel::GetMaxPlayerCount() {
	return m_iMaxPlayerCount;
}

bool Channel::TryAddPlayer(Player* player) {
	CriticalSectionLockGuard guard(m_ChannelLock);
	if (m_iPlayerCount < m_iMaxPlayerCount) {
		m_PlayerList.PushBack(player);
		m_iPlayerCount++;
		return true;
	}

	return false;
}

bool Channel::RemovePlayer(Player* player) {
	CriticalSectionLockGuard guard(m_ChannelLock);
	return m_PlayerList.Remove(player);
}
