﻿/*
 * 작성자 : 윤정도
 */

#include <TF/PrecompiledHeader.h>
#include <TF/Game/PlayerMap.h>

using namespace JCore;

void PlayerMap::AddNewPlayer(int accountUID, Player* player) {
	NormalLockGuard guard(m_Mutex);
	m_PlayerMap.Insert(accountUID, player);
}

bool PlayerMap::RemovePlayer(int accountUID) {
	NormalLockGuard guard(m_Mutex);
	if (!m_PlayerMap.Exist(accountUID))
		return false;

	return m_PlayerMap.Remove(accountUID);
}

bool PlayerMap::IsPlayerExist(int accountUID) {
	NormalLockGuard guard(m_Mutex);
	if (m_PlayerMap.Exist(accountUID))
		return true;

	return false;
}


Player* PlayerMap::FindIf(Func<bool, Player*> predicate) {
	NormalLockGuard guard(m_Mutex);

	const auto find = m_PlayerMap.Values().Extension().FindIf(predicate);
	if (find != nullptr) {
		return *find;
	}

	return nullptr;
}

int PlayerMap::Count() {
	NormalLockGuard guard(m_Mutex);
	return m_PlayerMap.Size();
}

