#pragma once

#include <JNetwork/Host/TcpClient.h>
#include <TF/Network/GameClientEventListener.h>
#include <TF/Network/CommandParser.h>
#include <Common/Command.h>

class GameClient : public JNetwork::TcpClient
{
public:
	~GameClient() override;

	static GameClient* GetInstance();
	void InitializeParser();

	void SetAccountUID(int AccountUID)			{ m_iAccountUID = AccountUID; }
	int	 GetAccountUID() const					{ return m_iAccountUID; }
	void SetChannelUID(int channelUID)			{ m_iChannelUID = channelUID; }
	int  GetChannelUID() const					{ return m_iChannelUID; }
	void SetCharacterUID(int characterUID)		{ m_CharacterInfo.CharacterUID = characterUID; }
	int  GetCharacterUID() const				{ return m_CharacterInfo.CharacterUID; }
	void SetRoomUID(int roomUID)				{ m_RoomInfo.RoomUID = roomUID; }
	int  GetRoomUID() const						{ return m_RoomInfo.RoomUID; }
	void UpdateCharacterInfo(CharacterInfo& info);
	void UpdateRoomInfo(RoomInfo& info);
private:
	CommandParser* m_Parser;
	GameClientEventListener* m_EventListener;

	int m_iAccountUID = INVALID_UID;
	int m_iChannelUID = INVALID_UID;

	RoomInfo m_RoomInfo{};
	CharacterInfo m_CharacterInfo{};

	inline static GameClient* ms_pInstance = nullptr;
};