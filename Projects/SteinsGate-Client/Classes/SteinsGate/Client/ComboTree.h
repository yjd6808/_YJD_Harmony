/*
 * 작성자: 윤정도
 * 생성일: 1/12/2023 8:08:09 AM
 * =====================
 * 음... 콤보 기능을 구현할려면
 * 여러가지 콤보에 대응가능한 트리가 있으면 좋을 것 같다.
 * TODO: 아직 캐릭터 방향 관련해서 제대로 처리가 안되어있어서 수정 필요
 */

#pragma once

#include <SteinsGate/Client/SGAction.h>

// 동일한 액션이 있을 수 있어서 연결리스트로 구성

struct ComboTreeNodeActionList
{
	SGAction* Action_{};
	ComboTreeNodeActionList* Next{};

	void add(SGAction* _pAction);
	void clear();
	int count();
	bool exist(SGAction* _pAction);
	SGAction* find_if(const SGPredicateFn<SGAction*>& _fn);
};

class SGComboTreeNode
{
public:
	ComboTreeNodeActionList ActionList{};
	SGComboTreeNode* Next[ControlKey::Max]{};

	bool isValid()
	{
		return ActionList.count() != 0;
	}

	bool empty()
	{
		return ActionList.count() == 0;
	}

	int count();
};

class SGComboTree
{
public:
	SGComboTree();
	~SGComboTree();

public:
	void removeAll();
	void addComboAction(SGAction* _pAction);
	SGAction* getComboAction(const ComboKeyList& _keys);

private:
	static void removeComboNodeRecursive(SGComboTreeNode* _pParent);
	static void addComboNodeRecursive(
		SGComboTreeNode* _pParent,
		SGComboTreeNode* _pNewNode,
		const ComboKeyList& _keys,
		int _keyIndex,
		int _keyCount
	);
	static SGComboTreeNode* findComboNodeRecursive(
		SGComboTreeNode* _pParent,
		const ComboKeyList& _keys,
		int _keyIndex,
		int _keyCount
	);

private:
	SGComboTreeNode* rootNode_{};
	int nodeCount_ = 0;
};
