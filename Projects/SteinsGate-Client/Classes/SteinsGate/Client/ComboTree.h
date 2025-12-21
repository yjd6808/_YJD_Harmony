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

//////////////////////////////////////////////////////////////////////////////////////////
// 동일한 액션이 있을 수 있어서 연결리스트로 구성
struct ComboTreeNodeActionList
{
	SGAction* action_{};
	ComboTreeNodeActionList* pNext_{};

	void Add(SGAction* _pAction);
	void Clear();
	int Count();
	bool Exist(SGAction* _pAction);
	SGAction* FindIf(const SGPredicateFn<SGAction*>& _fn);
};

//////////////////////////////////////////////////////////////////////////////////////////
class SGComboTreeNode
{
public:
	ComboTreeNodeActionList actionList_{};
	SGComboTreeNode* pNextArr_[ControlKey::Max]{};

	bool IsValid()
	{
		return actionList_.Count() != 0;
	}

	bool Empty()
	{
		return actionList_.Count() == 0;
	}

	int Count();
};

//////////////////////////////////////////////////////////////////////////////////////////
class SGComboTree
{
public:
	SGComboTree();
	~SGComboTree();

public:
	void RemoveAll();
	void AddComboAction(SGAction* _pAction);
	SGAction* GetComboAction(const ComboKeyList& _keys);

private:
	static void RemoveComboNodeRecursive(SGComboTreeNode* _pParent);
	static void AddComboNodeRecursive(
		SGComboTreeNode* _pParent,
		SGComboTreeNode* _pNewNode,
		const ComboKeyList& _keys,
		int _keyIndex,
		int _keyCount);

	static SGComboTreeNode* FindComboNodeRecursive(
		SGComboTreeNode* _pParent,
		const ComboKeyList& _keys,
		int _keyIndex,
		int _keyCount);

private:
	SGComboTreeNode* rootNode_{};
	int nodeCount_ = 0;
};
