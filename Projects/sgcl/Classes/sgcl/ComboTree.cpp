/*
 * 작성자: 윤정도
 * 생성일: 1/12/2023 8:09:21 AM
 * =====================
 *
 */

#include "ComboTree.h"

//////////////////////////////////////////////////////////////////////////////////////////
void ComboTreeNodeActionList::Add(SGAction* _pAction)
{
	ComboTreeNodeActionList* pCur = this;
	ComboTreeNodeActionList* pEnd = nullptr;

	while (pCur != nullptr)
	{
		pEnd = pCur;
		pCur = pCur->pNext_;
	}

	if (pEnd->action_ == nullptr)
	{
		pEnd->action_ = _pAction;
		pEnd->pNext_ = dbg_new ComboTreeNodeActionList;
	}
}

//////////////////////////////////////////////////////////////////////////////////////////
void ComboTreeNodeActionList::Clear()
{
	ComboTreeNodeActionList* pCur = pNext_;

	while (pCur != nullptr)
	{
		auto pTemp = pCur;
		pCur = pCur->pNext_;
		delete pTemp;
	}

	action_ = nullptr;
	pNext_ = nullptr;
}

//////////////////////////////////////////////////////////////////////////////////////////
int ComboTreeNodeActionList::Count()
{
	if (action_ == nullptr)
	{
		return 0;
	}

	int count = 1;
	ComboTreeNodeActionList* pCur = pNext_;

	while (pCur != nullptr)
	{
		if (pCur->action_)
		{
			++count;
		}

		pCur = pCur->pNext_;
	}

	return count;
}

//////////////////////////////////////////////////////////////////////////////////////////
bool ComboTreeNodeActionList::Exist(SGAction* _pAction)
{
	return FindIf([_pAction](SGAction* _pOtherAction)
	{
		return _pAction == _pOtherAction;
	}) != nullptr;
}

//////////////////////////////////////////////////////////////////////////////////////////
SGAction* ComboTreeNodeActionList::FindIf(const jc::Predicate<SGAction*>& _fn)
{
	ComboTreeNodeActionList* pCur = this;

	while (pCur)
	{
		if (pCur->action_ && _fn(pCur->action_))
		{
			return pCur->action_;
		}

		pCur = pCur->pNext_;
	}

	return nullptr;
}

//////////////////////////////////////////////////////////////////////////////////////////
int SGComboTreeNode::Count()
{
	int childCount = 0;

	for (int i = 0; i < ControlKey::Max; ++i)
	{
		if (pNextArr_[i] != nullptr && pNextArr_[i]->IsValid())
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
	RemoveAll();
	JC_DELETE_SAFE(rootNode_);
}

//////////////////////////////////////////////////////////////////////////////////////////
void SGComboTree::AddComboAction(SGAction* _pAction)
{
	const ComboKeyList& keys = _pAction->GetCommand();
	const ComboKeyList& reversedKeys = keys.Reverse();

	int comboKeyCount = keys.Count();
	jc_assert_msg(comboKeyCount != 0, "정방향 키가 없습니다.");

	SGComboTreeNode* pFoundNode = FindComboNodeRecursive(rootNode_, keys, comboKeyCount - 1, comboKeyCount);

	if (pFoundNode == nullptr)
	{
		SGComboTreeNode* pNewNode = dbg_new SGComboTreeNode{};
		pNewNode->actionList_.Add(_pAction);
		AddComboNodeRecursive(rootNode_, pNewNode, keys, 0, comboKeyCount);
		++nodeCount_;
	}
	else
	{
		pFoundNode->actionList_.Add(_pAction);
	}

	if (keys == reversedKeys)
	{
		return;
	}

	int reversedComboKeyCount = reversedKeys.Count();
	jc_assert_msg(reversedComboKeyCount != 0, "반대 방향 키가 없습니다.");

	pFoundNode = FindComboNodeRecursive(rootNode_, reversedKeys, reversedComboKeyCount - 1, reversedComboKeyCount);

	if (pFoundNode == nullptr)
	{
		SGComboTreeNode* pNewNode = dbg_new SGComboTreeNode{};
		pNewNode->actionList_.Add(_pAction);
		AddComboNodeRecursive(rootNode_, pNewNode, reversedKeys, 0, reversedComboKeyCount);
		++nodeCount_;
	}
	else
	{
		pFoundNode->actionList_.Add(_pAction);
	}
}

//////////////////////////////////////////////////////////////////////////////////////////
void SGComboTree::RemoveAll()
{
	// 루트빼고 다 삭제
	for (int i = 0; i < ControlKey::Max; ++i)
	{
		RemoveComboNodeRecursive(rootNode_->pNextArr_[i]);
		rootNode_->pNextArr_[i] = nullptr;
	}
}

//////////////////////////////////////////////////////////////////////////////////////////
SGAction* SGComboTree::GetComboAction(const ComboKeyList& _keys)
{
	int comboKeyCount = _keys.Count();

	if (comboKeyCount == 0)
	{
		return nullptr;
	}

	SGComboTreeNode* pFoundNode = FindComboNodeRecursive(rootNode_, _keys, comboKeyCount - 1, comboKeyCount);

	if (pFoundNode == nullptr || pFoundNode->Empty())
	{
		return nullptr;
	}

	SGAction* pAction = pFoundNode->actionList_.FindIf([](SGAction* _pAction)
	{
		return _pAction->OnConditionCheck();
	});

	return pAction;
}

//////////////////////////////////////////////////////////////////////////////////////////
void SGComboTree::RemoveComboNodeRecursive(SGComboTreeNode* _pParent)
{
	if (_pParent == nullptr)
	{
		return;
	}

	for (int i = 0; i < ControlKey::Max; ++i)
	{
		RemoveComboNodeRecursive(_pParent->pNextArr_[i]);
	}

	_pParent->actionList_.Clear();
	delete _pParent;
}

//////////////////////////////////////////////////////////////////////////////////////////
void SGComboTree::AddComboNodeRecursive(
	SGComboTreeNode* _pParent,
	SGComboTreeNode* _pNewNode,
	const ComboKeyList& _keys,
	int _keyIndex,
	int _keyCount)
{
	ControlKey_t controlKey = _keys.At(_keyIndex);
	SGComboTreeNode* pCur = _pParent->pNextArr_[controlKey];

	if (_keyIndex == _keyCount - 1)
	{
		// 한줄이면 끝나는데
		// 잘못된 코드 캐치하기 위함.
		if (pCur == nullptr)
		{
			_pParent->pNextArr_[controlKey] = _pNewNode;
		}
		else
		{
			jc_assert_msg(pCur->Empty(), "이미 해당 콤보키가 바인딩 되어 있습니다.");
			_pParent->pNextArr_[controlKey] = _pNewNode;
		}

		return;
	}

	// 가는 경로에 노드가 없으면 생성
	if (pCur == nullptr)
	{
		_pParent->pNextArr_[controlKey] = dbg_new SGComboTreeNode;
	}

	AddComboNodeRecursive(_pParent->pNextArr_[controlKey], _pNewNode, _keys, _keyIndex + 1, _keyCount);
}

//////////////////////////////////////////////////////////////////////////////////////////
SGComboTreeNode* SGComboTree::FindComboNodeRecursive(
	SGComboTreeNode* _pParent,
	const ComboKeyList& _keys,
	int _keyIndex,
	int _keyCount)
{
	if (_pParent == nullptr)
	{
		return nullptr;
	}

	ControlKey_t controlKey = _keys.At(_keyIndex);
	SGComboTreeNode* pCur = _pParent->pNextArr_[controlKey];

	if (_keyIndex == 0)
	{
		return pCur;
	}

	return FindComboNodeRecursive(pCur, _keys, _keyIndex - 1, _keyCount);
}
