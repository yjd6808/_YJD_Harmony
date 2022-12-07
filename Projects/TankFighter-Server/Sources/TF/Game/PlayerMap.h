/*
 * 작성자 : 윤정도
 */

#pragma once

#include <JCore/Container/HashMap.h>
#include <TF/Game/Player.h>

class PlayerMap
{
public:
	void AddNewPlayer(int accountUID, Player* player);
	bool RemovePlayer(int accountUID);
	bool IsPlayerExist(int accountUID);
	Player* FindIf(JCore::Func<bool, Player*> foreachAction);
	int Count();
private:
	JCore::HashMap<int, Player*> m_PlayerMap;
	JCore::NormalLock m_Mutex;
};

