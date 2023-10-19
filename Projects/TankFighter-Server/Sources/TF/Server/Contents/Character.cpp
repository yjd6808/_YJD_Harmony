/*
 * 작성자: 윤정도
 * 생성일: 10/16/2023 6:33:34 PM
 * =====================
 *
 */

#include "Pch.h"
#include "Character.h"

#include <TF/Common/Structure.h>

#include <TF/Server/Const.h>
#include <TF/Server/Query/Q_GAME.h>
#include <TF/Server/Contents/Character.h>
#include <TF/Server/Send/S_GAME.h>

#include "Host/GameSession.h"

USING_NS_JC;
USING_NS_JNET;

Character::Character()
	: m_pPlayer(nullptr)
	, m_iPrimaryKey(Const::InvalidValue)
	, m_szName(String::Null)
	, m_iWin(0)
	, m_iLose(0)
	, m_iDeath(0)
	, m_iKill(0)
	, m_iMoney(0)
	, m_bReady(false)
	, m_bDeath(false)
	, m_bDirty(false)
{}

Character::~Character() {}

void Character::OnPopped() {
	m_pPlayer = nullptr;
	m_iPrimaryKey = Const::InvalidValue;
	m_szName = String::Null;
	m_iWin = 0;
	m_iLose = 0;
	m_iDeath = 0;
	m_iKill = 0;
	m_iMoney = 0;
}

void Character::OnPushed() {}

void Character::SetPrimaryKey(int primaryKey) {
	m_iPrimaryKey = primaryKey;
}

void Character::SetName(const String& name, bool dirty /* = true */) {
	m_szName = name;
	m_bDirty = dirty;
}

void Character::SetWin(int win, bool dirty /* = true */) {
	m_iWin = win;
	m_bDirty = dirty;
}

void Character::SetLose(int lose, bool dirty /* = true */) {
	m_iLose = lose;
	m_bDirty = dirty;
}

void Character::SetDeath(int death, bool dirty /* = true */) {
	m_iDeath = death;
	m_bDirty = dirty;
}

void Character::SetKill(int kill, bool dirty /* = true */) {
	m_iKill = kill;
	m_bDirty = dirty;
}

void Character::AddWin(int win, bool dirty /* = true */) {
	m_iWin += win;
	m_bDirty = dirty;
}

void Character::AddLose(int lose, bool dirty /* = true */) {
	m_iWin += lose;
	m_bDirty = dirty;
}

void Character::AddDeath(int death, bool dirty /* = true */) {
	m_iWin += death;
	m_bDirty = dirty;
}

void Character::AddKill(int kill, bool dirty /* = true */) {
	m_iWin += kill;
	m_bDirty = dirty;
}

void Character::ApplyToDatabase() {
	if (m_iPrimaryKey <= Const::InvalidValue) {
		_LogWarn_("비활성화된 캐릭터 정보를 DB 반영할려고 시도했습니다.");
		return;
	}

	if (!m_bDirty)
		return;

	m_bDirty = false;
}

int Character::GetFriendCount() const {
	JCORE_LOCK_GUARD(m_FriendLock);
	return m_vFriendList.Size();
}

void Character::GetFriendList(JCORE_OUT FriendCharacterInfo* friendArray, int capacity) {
	JCORE_LOCK_GUARD(m_FriendLock);
	for (int i = 0; i < m_vFriendList.Size() && i < capacity; ++i) {
		friendArray[i] = m_vFriendList[i];
	}
}

bool Character::IsFriend(int characterPrimaryKey) {
	JCORE_LOCK_GUARD(m_FriendLock);
	for (int i = 0; i < m_vFriendList.Size(); ++i) {
		if (m_vFriendList[i].PrimaryKey == characterPrimaryKey) {
			return true;
		}
	}
	return false;
}

void Character::AddFriend(Character* friendCharactor) {
	FriendCharacterInfo friendInfo;
	friendCharactor->GetFriendInfo(friendInfo);
	JCORE_LOCK_GUARD(m_FriendLock);
	m_vFriendList.PushBack(Move(friendInfo));
}

void Character::DeleteFriend(int characterPrimaryKey) {
	JCORE_LOCK_GUARD(m_FriendLock);
	m_vFriendList.RemoveIf([=](FriendCharacterInfo& info) { return info.PrimaryKey == characterPrimaryKey; });
}

void Character::ChangeFriendState(int friendCharacterPrimaryKey, bool login) {
	JCORE_LOCK_GUARD(m_FriendLock);
	FriendCharacterInfo* pInfo = m_vFriendList.FindIf([=](FriendCharacterInfo& info) { return info.PrimaryKey == friendCharacterPrimaryKey; });

	if (pInfo == nullptr) {
		return;
	}
	pInfo->LoggedIn = login;
}

void Character::NotifyLoginStateToFriends(bool login) {
	Vector<int> vFriendPrimaryKeyList(0);

	{
		JCORE_LOCK_GUARD(m_FriendLock);
		vFriendPrimaryKeyList.Resize(m_vFriendList.Size());

		for (int i = 0; i < m_vFriendList.Size(); ++i) {
			vFriendPrimaryKeyList.PushBack(m_vFriendList[i].PrimaryKey);
		}
	}

	const int iFriendCount = vFriendPrimaryKeyList.Size();
	if (iFriendCount <= 0) {
		return;
	}

	ChannelLobby* pLobby = m_pPlayer->GetChannelLobby();
	if (pLobby == nullptr) {
		DebugAssert(false);
		return;
	}

	// 로그인 중인 친구 플레이어 목록
	Vector<Player*> vLoginedFriendPlayerList(iFriendCount);
	pLobby->GetPlayerListByCharacterPrimaryKeyList(vFriendPrimaryKeyList, vLoginedFriendPlayerList);

	for (int i = 0; i < vLoginedFriendPlayerList.Size(); ++i) {
		Player* pFriendPlayer = vLoginedFriendPlayerList[i];
		Character* pFriendCharacter = pFriendPlayer->GetCharacter();
		if (pFriendCharacter == nullptr) {
			continue;
		}

		pFriendCharacter->ChangeFriendState(m_iPrimaryKey, login);
		S_GAME::SetInformation(pFriendPlayer->GetSession(), SendStrategy::SendAsync);
		S_GAME::SEND_SC_UpdateFriendList(pFriendCharacter);
	}
}

void Character::LoadFriendList(ChannelLobby* channelLobby) {
	if (m_iPrimaryKey <= Const::InvalidValue) {
		return;
	}

	Qry::SelectFriendCharacterInfoListResult result = Q_GAME::SelectFriendCharacterInfoList(m_iPrimaryKey);

	JCORE_LOCK_GUARD(m_FriendLock);
	m_vFriendList.Clear();

	for (int i = 0; i < result.RowCount; ++i, result.FetchNextRow()) {
		FriendCharacterInfo info;
		info.Name = result.Name;
		info.PrimaryKey = result.PrimaryKey;
		info.LoggedIn = false;

		const Player* pFriend = channelLobby->GetPlayerByCharacterPrimaryKey(result.PrimaryKey);

		if (pFriend != nullptr) {
			const Character* pFriendCharacter = pFriend->GetCharacter();

			if (pFriendCharacter == nullptr)
				continue;

			info.LoggedIn = true;
		}

		m_vFriendList.PushBack(Move(info));
	}

}

void Character::SetInfo(const CharacterInfo& info, bool dirty /*= true*/) {
	m_iPrimaryKey = info.PrimaryKey;
	m_szName = info.Name;
	m_iWin = info.Win;
	m_iLose = info.Lose;
	m_iKill = info.Kill;
	m_iDeath = info.Death;
	m_iMoney = info.Money;
	m_bDirty = dirty;
}

void Character::GetInfo(JCORE_REF_OUT CharacterInfo& info) {
	info.AccessId = m_iAccessId;
	info.PrimaryKey = m_iPrimaryKey;
	info.Name = m_szName;
	info.Win = m_iWin;
	info.Lose = m_iLose;
	info.Kill = m_iKill;
	info.Death = m_iDeath;
	info.Money = m_iMoney;
	info.LoggedIn = true;
}

void Character::GetRoomInfo(RoomCharacterInfo& info) {
	GetInfo(info);
	info.Ready = m_bReady;
	info.Death = m_bDeath;
}

void Character::GetFriendInfo(JCORE_REF_OUT FriendCharacterInfo& info) {
	info.PrimaryKey = m_iPrimaryKey;
	info.Name = m_szName;
	info.LoggedIn = true;
}

String Character::ToString() { return StringUtil::Format("%s(%d)", m_szName.Source(), m_iPrimaryKey); }
