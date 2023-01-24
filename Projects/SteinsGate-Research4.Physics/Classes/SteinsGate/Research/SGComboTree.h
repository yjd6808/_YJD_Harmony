/*
 * 작성자: 윤정도
 * 생성일: 1/12/2023 8:08:09 AM
 * =====================
 * 음... 콤보 기능을 구현할려면
 * 여러가지 콤보에 대응가능한 트리가 있으면 좋을 것 같다.
 */

#pragma once

#include <SteinsGate/Research/SGComboAction.h>

class SGComboTreeNode
{
public:
	SGComboAction* Action{};
	SGComboTreeNode* Next[ControlKey::Max]{};

	bool IsValid()	{ return Action != nullptr; }
	bool Empty()	{ return Action == nullptr;	}
	int  Count();
};

class SGComboTree
{
public:
	SGComboTree();
	~SGComboTree();
public:
	void RemoveComboAction(const SGComboKeyList& keys);
	void RemoveAll();
	void AddComboAction(SGComboAction* action);
	SGComboAction* GetComboAction(const SGComboKeyList& keys);
private:
	static void RemoveComboNodeRecursive(SGComboTreeNode* parent);
	static void AddComboNodeRecursive(
		SGComboTreeNode* parent, 
		SGComboTreeNode* newNode, 
		const SGComboKeyList& keys, 
		int keyIndex, 
		int keyCount
	);
	static SGComboTreeNode* FindComboNodeRecursive(
		SGComboTreeNode* parent, 
		const SGComboKeyList& keys, 
		int keyIndex, 
		int keyCount
	);
private:
	SGComboTreeNode* m_pRoot;
	int m_iCount = 0;
};









