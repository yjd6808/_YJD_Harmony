/*
 * 작성자: 윤정도
 * 생성일: 10/13/2023 12:30:33 PM
 * =====================
 *
 */


#pragma once

#include <JCore/Container/Properties.h>
#include <JNetwork/Host/TcpClient.h>

class GameClient : public JNetwork::TcpClient
{
public:
	GameClient(const JNetwork::IOCPPtr& iocp, const JCore::MemoryPoolAbstractPtr& bufferAllocator);

	void SetReconnectMode(bool mode);
	bool IsReconnectMode() const;

	int GetAccountPrimaryKey() const;
	void SetAccountPrimaryKey(int primaryKey);

	int GetChannelPrimaryKey() const;
	void SetChannelPrimaryKey(int primaryKey);

	int GetCharacterPrimaryKey() const;
	void SetCharacterPrimaryKey(int primaryKey);
private:
	JCore::Properties<int> m_Properties;
};
