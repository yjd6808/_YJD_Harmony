/*
 * 작성자: 윤정도
 * 생성일: 10/16/2023 6:33:22 PM
 * =====================
 *
 */


#pragma once


#include <JCore/Pool/AccessibleObjectPool.h>
#include <JCore/Primitives/String.h>

#include <TF/Common/Base/IUpdatable.h>

struct CharacterInfo;

class Character : public JCore::AccessibleObjectPool<Character>
{
public:
	Character();
	~Character() override;

	void OnPopped() override;
	void OnPushed() override;

	int GetPrimaryKey() const { return m_iPrimaryKey; }
	JCore::String GetName() const { return m_szName; }
	int GetWin() const { return m_iWin; }
	int GetLose() const { return m_iLose; }
	int GetDeath() const { return m_iDeath; }
	int GetKill() const { return m_iKill; }

	void SetPrimaryKey(int primaryKey);
	void SetName(const JCore::String& name, bool dirty = true);
	void SetWin(int win, bool dirty = true);
	void SetLose(int lose, bool dirty = true);
	void SetDeath(int death, bool dirty = true);
	void SetKill(int kill, bool dirty = true);

	void AddWin(int win, bool dirty = true);
	void AddLose(int lose, bool dirty = true);
	void AddDeath(int death, bool dirty = true);
	void AddKill(int kill, bool dirty = true);

	void ApplyToDatabase();

	void SetInfo(const CharacterInfo& info, bool dirty = true);
	CharacterInfo GetInfo() const;

	JCore::String ToString();


	static Character Empty;
private:
	int m_iPrimaryKey;
	JCore::String m_szName;

	// 외부 쓰레드에서 변경될 수 있으므로 아토믹으로 둠
	JCore::AtomicInt m_iWin;
	JCore::AtomicInt m_iLose;
	JCore::AtomicInt m_iDeath;
	JCore::AtomicInt m_iKill;
	JCore::AtomicInt m_iMoney;
	bool m_bDirty;
};
