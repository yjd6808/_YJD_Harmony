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

USING_NS_JC;
USING_NS_JNET;

Character Character::Empty;

Character::Character()
	: m_iPrimaryKey(Const::InvalidValue)
	, m_szName(String::Null)
	, m_iWin(0)
	, m_iLose(0)
	, m_iDeath(0)
	, m_iKill(0)
	, m_iMoney(0)
	, m_bDirty(false)
{}

Character::~Character() {}

void Character::OnPopped() {
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

CharacterInfo Character::GetInfo() const {
	CharacterInfo info;
	info.PrimaryKey = m_iPrimaryKey;
	info.Name = m_szName;
	info.Win = m_iWin;
	info.Lose = m_iLose;
	info.Kill = m_iKill;
	info.Death = m_iDeath;
	info.Money = m_iMoney;
	return info;
}

String Character::ToString() { return StringUtil::Format("%s(%d)", m_szName.Source(), m_iPrimaryKey); }
