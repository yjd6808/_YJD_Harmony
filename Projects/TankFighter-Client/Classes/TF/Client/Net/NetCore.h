/*
 * 작성자: 윤정도
 * 생성일: 10/13/2023 8:19:59 AM
 * =====================
 *
 */


#pragma once


#include <JNetwork/NetGroup.h>

#include <TF/Client/TypeDef.h>

#include <TF/Client/Net/Host/GameClient.h>
#include <TF/Client/Net/Listener/L_GAME.h>

class NetCore : public JNetwork::NetGroup
{
public:
	NetCore();
	~NetCore() override;

	void Initialize() override;
	void Finalize() override;

	void initializeBuffer();
	void initializeIOCP();
	void initializeHost();
	void initializeParser();

	void runCommand(JNetwork::Session* session, JNetwork::ICommand* cmd);
	void clearCommandQueue();
	void update(const JCore::TimeSpan& elpased);
	GameClient* getGameClient() const { return m_spGameClient.GetPtr(); }

private:
	void processCommands();
private:
	L_GAME* m_pListenerGame;
	JCore::SharedPtr<GameClient> m_spGameClient;
	CommandQueue* m_pCommandQueue;
	JNetwork::CommandParser m_Parser;
};
