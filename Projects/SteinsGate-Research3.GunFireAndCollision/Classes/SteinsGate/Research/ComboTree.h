/*
 * 작성자: 윤정도
 * 생성일: 1/12/2023 8:08:09 AM
 * =====================
 * 음... 콤보 기능을 구현할려면
 * 여러가지 콤보에 대응가능한 트리가 있으면 좋을 것 같다.
 */

#pragma once

#include <SteinsGate/Research/ComboAction.h>

class ComboTreeNode
{
public:
	ComboAction* Action{};
	ComboTreeNode* Next[ControlKey::Max]{};

	bool IsValid()	{ return Action != nullptr; }
	bool Empty()	{ return Action == nullptr;	}
	int  Count();
};

class ComboTree
{
public:
	ComboTree();
	~ComboTree();
public:
	void RemoveComboAction(const ComboKeyList& keys);
	void RemoveAll();
	void AddComboAction(ComboAction* action);
	ComboAction* GetComboAction(const ComboKeyList& keys);
private:
	static void RemoveComboNodeRecursive(ComboTreeNode* parent);
	static void AddComboNodeRecursive(ComboTreeNode* parent, ComboTreeNode* newNode, const ComboKeyList& keys, int keyIndex, int keyCount);
	static ComboTreeNode* FindComboNodeRecursive(ComboTreeNode* parent, const ComboKeyList& keys, int keyIndex, int keyCount);
private:
	ComboTreeNode* m_pRoot;
	int m_iCount = 0;
};









