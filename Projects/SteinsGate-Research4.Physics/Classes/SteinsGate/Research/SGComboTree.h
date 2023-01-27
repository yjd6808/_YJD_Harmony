/*
 * 작성자: 윤정도
 * 생성일: 1/12/2023 8:08:09 AM
 * =====================
 * 음... 콤보 기능을 구현할려면
 * 여러가지 콤보에 대응가능한 트리가 있으면 좋을 것 같다.
 */

#pragma once

#include <SteinsGate/Research/SGAction.h>

class SGComboTreeNode
{
public:
	SGAction* Action{};
	SGComboTreeNode* Next[ControlKey::Max]{};

	bool isValid()	{ return Action != nullptr; }
	bool empty()	{ return Action == nullptr;	}
	int  count();
};

class SGComboTree
{
public:
	SGComboTree();
	~SGComboTree();
public:
	void removeComboAction(const SGComboKeyList& keys);
	void removeAll();
	void addComboAction(SGAction* action);
	SGAction* getComboAction(const SGComboKeyList& keys);
private:
	static void removeComboNodeRecursive(SGComboTreeNode* parent);
	static void addComboNodeRecursive(
		SGComboTreeNode* parent, 
		SGComboTreeNode* newNode, 
		const SGComboKeyList& keys, 
		int keyIndex, 
		int keyCount
	);
	static SGComboTreeNode* findComboNodeRecursive(
		SGComboTreeNode* parent, 
		const SGComboKeyList& keys, 
		int keyIndex, 
		int keyCount
	);
private:
	SGComboTreeNode* m_pRoot;
	int m_iCount = 0;
};









