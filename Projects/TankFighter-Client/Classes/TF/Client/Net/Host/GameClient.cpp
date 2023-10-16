/*
 * 작성자: 윤정도
 * 생성일: 10/13/2023 12:32:02 PM
 * =====================
 *
 */


#include "Pch.h"
#include "GameClient.h"

USING_NS_JC;

GameClient::GameClient(const JNetwork::IOCPPtr& iocp, const JCore::MemoryPoolAbstractPtr& bufferAllocator)
	: TcpClient(iocp, bufferAllocator)
{
	m_Properties.Add(Const::Properties::GameClient::Reconnecting, PropertyType::Bool);
	m_Properties.Add(Const::Properties::GameClient::AccountPrimaryKey, PropertyType::Int);
	m_Properties.Add(Const::Properties::GameClient::ChannelPrimaryKey, PropertyType::Int);
	m_Properties.Add(Const::Properties::GameClient::CharacterPrimaryKey, PropertyType::Int);
}

void GameClient::SetReconnectMode(bool mode) {
	m_Properties[Const::Properties::GameClient::Reconnecting] = mode;
}

bool GameClient::IsReconnectMode() const {
	return m_Properties[Const::Properties::GameClient::Reconnecting].As<bool>();
}

int GameClient::GetAccountPrimaryKey() const {
	return m_Properties[Const::Properties::GameClient::AccountPrimaryKey].As<int>();
}

void GameClient::SetAccountPrimaryKey(int primaryKey) {
	m_Properties[Const::Properties::GameClient::AccountPrimaryKey] = primaryKey;
}

int GameClient::GetChannelPrimaryKey() const {
	return m_Properties[Const::Properties::GameClient::ChannelPrimaryKey].As<int>();
}

void GameClient::SetChannelPrimaryKey(int primaryKey) {
	m_Properties[Const::Properties::GameClient::ChannelPrimaryKey] = primaryKey;
}

int GameClient::GetCharacterPrimaryKey() const {
	return m_Properties[Const::Properties::GameClient::CharacterPrimaryKey].As<int>();
}

void GameClient::SetCharacterPrimaryKey(int primaryKey) {
	m_Properties[Const::Properties::GameClient::CharacterPrimaryKey] = primaryKey;
}
