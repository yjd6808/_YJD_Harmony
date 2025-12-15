/*
 * 작성자: 윤정도
 * 생성일: 1/12/2023 8:09:21 AM
 * =====================
 *
 */

#include "Tutturu.h"
#include "ComboTree.h"

//////////////////////////////////////////////////////////////////////////////////////////
void ComboTreeNodeActionList::add(SGAction* _pAction)
{
	ComboTreeNodeActionList* pCur = this;
	ComboTreeNodeActionList* pEnd = nullptr;

	while (pCur != nullptr)
	{
		pEnd = pCur;
		pCur = pCur->Next;
	}

	if (pEnd->Action_ == nullptr)
	{
		pEnd->Action_ = _pAction;
		pEnd->Next = dbg_new ComboTreeNodeActionList;
	}
}

//////////////////////////////////////////////////////////////////////////////////////////
void ComboTreeNodeActionList::clear()
{
	ComboTreeNodeActionList* pCur = Next;

	while (pCur != nullptr)
	{
		auto pTemp = pCur;
		pCur = pCur->Next;
		delete pTemp;
	}

	Action_ = nullptr;
	Next = nullptr;
}

//////////////////////////////////////////////////////////////////////////////////////////
int ComboTreeNodeActionList::count()
{
	if (Action_ == nullptr)
	{
		return 0;
	}

	int count = 1;
	ComboTreeNodeActionList* pCur = Next;

	while (pCur != nullptr)
	{
		if (pCur->Action_)
		{
			++count;
		}

		pCur = pCur->Next;
	}

	return count;
}

//////////////////////////////////////////////////////////////////////////////////////////
bool ComboTreeNodeActionList::exist(SGAction* _pAction)
{
	return find_if([_pAction](SGAction* pAction)
	{
		return _pAction == pAction;
	}) != nullptr;
}

//////////////////////////////////////////////////////////////////////////////////////////
SGAction* ComboTreeNodeActionList::find_if(const SGPredicateFn<SGAction*>& _fn)
{
	ComboTreeNodeActionList* pCur = this;

	while (pCur)
	{
		if (pCur->Action_ && _fn(pCur->Action_))
		{
			return pCur->Action_;
		}

		pCur = pCur->Next;
	}

	return nullptr;
}

//////////////////////////////////////////////////////////////////////////////////////////
int SGComboTreeNode::count()
{
	int childCount = 0;

	for (int i = 0; i < ControlKey::Max; ++i)
	{
		if (Next[i] != nullptr && Next[i]->isValid())
		{
			++childCount;
		}
	}

	return childCount;
}

//////////////////////////////////////////////////////////////////////////////////////////
SGComboTree::SGComboTree()
{
	rootNode_ = dbg_new SGComboTreeNode{};
}

//////////////////////////////////////////////////////////////////////////////////////////
SGComboTree::~SGComboTree()
{
	removeAll();
	JCORE_DELETE_SAFE(rootNode_);
}

//////////////////////////////////////////////////////////////////////////////////////////
void SGComboTree::addComboAction(SGAction* _pAction)
{
	const ComboKeyList& keys = _pAction->getCommand();
	const ComboKeyList& reversedKeys = keys.reverse();

	int comboKeyCount = keys.count();
	DebugAssertMsg(comboKeyCount != 0, "정방향 키가 없습니다.");

	SGComboTreeNode* pFoundNode = findComboNodeRecursive(rootNode_, keys, comboKeyCount - 1, comboKeyCount);

	if (pFoundNode == nullptr)
	{
		SGComboTreeNode* pNewNode = dbg_new SGComboTreeNode{};
		pNewNode->ActionList.add(_pAction);
		addComboNodeRecursive(rootNode_, pNewNode, keys, 0, comboKeyCount);
		++nodeCount_;
	}
	else
	{
		pFoundNode->ActionList.add(_pAction);
	}

	if (keys == reversedKeys)
	{
		return;
	}

	int reversedComboKeyCount = reversedKeys.count();
	DebugAssertMsg(reversedComboKeyCount != 0, "반대 방향 키가 없습니다.");

	pFoundNode = findComboNodeRecursive(rootNode_, reversedKeys, reversedComboKeyCount - 1, reversedComboKeyCount);

	if (pFoundNode == nullptr)
	{
		SGComboTreeNode* pNewNode = dbg_new SGComboTreeNode{};
		pNewNode->ActionList.add(_pAction);
		addComboNodeRecursive(rootNode_, pNewNode, reversedKeys, 0, reversedComboKeyCount);
		++nodeCount_;
	}
	else
	{
		pFoundNode->ActionList.add(_pAction);
	}
}

//////////////////////////////////////////////////////////////////////////////////////////
void SGComboTree::removeAll()
{
	// 루트빼고 다 삭제
	for (int i = 0; i < ControlKey::Max; ++i)
	{
		removeComboNodeRecursive(rootNode_->Next[i]);
		rootNode_->Next[i] = nullptr;
	}
}

//////////////////////////////////////////////////////////////////////////////////////////
SGAction* SGComboTree::getComboAction(const ComboKeyList& _keys)
{
	int comboKeyCount = _keys.count();

	if (comboKeyCount == 0)
	{
		return nullptr;
	}

	SGComboTreeNode* pFoundNode = findComboNodeRecursive(rootNode_, _keys, comboKeyCount - 1, comboKeyCount);

	if (pFoundNode == nullptr || pFoundNode->empty())
	{
		return nullptr;
	}

	SGAction* pAction = pFoundNode->ActionList.find_if([](SGAction* pAction)
	{
		return pAction->onConditionCheck();
	});

	return pAction;
}

//////////////////////////////////////////////////////////////////////////////////////////
void SGComboTree::removeComboNodeRecursive(SGComboTreeNode* _pParent)
{
	if (_pParent == nullptr)
	{
		return;
	}

	for (int i = 0; i < ControlKey::Max; ++i)
	{
		removeComboNodeRecursive(_pParent->Next[i]);
	}

	_pParent->ActionList.clear();
	delete _pParent;
}

//////////////////////////////////////////////////////////////////////////////////////////
void SGComboTree::addComboNodeRecursive(
	SGComboTreeNode* _pParent,
	SGComboTreeNode* _pNewNode,
	const ComboKeyList& _keys,
	int _keyIndex,
	int _keyCount)
{
	ControlKey_t controlKey = _keys.at(_keyIndex);
	SGComboTreeNode* pCur = _pParent->Next[controlKey];

	if (_keyIndex == _keyCount - 1)
	{
		// 한줄이면 끝나는데
		// 잘못된 코드 캐치하기 위함.
		if (pCur == nullptr)
		{
			_pParent->Next[controlKey] = _pNewNode;
		}
		else
		{
			DebugAssertMsg(pCur->empty(), "이미 해당 콤보키가 바인딩 되어 있습니다.");
			_pParent->Next[controlKey] = _pNewNode;
		}

		return;
	}

	// 가는 경로에 노드가 없으면 생성
	if (pCur == nullptr)
	{
		_pParent->Next[controlKey] = dbg_new SGComboTreeNode;
	}

	addComboNodeRecursive(_pParent->Next[controlKey], _pNewNode, _keys, _keyIndex + 1, _keyCount);
}

//////////////////////////////////////////////////////////////////////////////////////////
SGComboTreeNode* SGComboTree::findComboNodeRecursive(
	SGComboTreeNode* _pParent,
	const ComboKeyList& _keys,
	int _keyIndex,
	int _keyCount)
{
	if (_pParent == nullptr)
	{
		return nullptr;
	}

	ControlKey_t controlKey = _keys.at(_keyIndex);
	SGComboTreeNode* pCur = _pParent->Next[controlKey];

	if (_keyIndex == 0)
	{
		return pCur;
	}

	return findComboNodeRecursive(pCur, _keys, _keyIndex - 1, _keyCount);
}
