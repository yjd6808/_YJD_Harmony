/*
 * 작성자: 윤정도
 * 생성일: 1/12/2023 8:09:21 AM
 * =====================
 *
 */

#include "Tutturu.h"
#include "ComboTree.h"

void ComboTreeNodeActionList::add(SGAction* action) {
	ComboTreeNodeActionList* pCur = this;
	ComboTreeNodeActionList* pEnd = nullptr;

	while (pCur != nullptr) {
		pEnd = pCur;
		pCur = pCur->Next;
	}

	if (pEnd->Action_ == nullptr) {
		pEnd->Action_ = action;
		pEnd->Next = dbg_new ComboTreeNodeActionList;
	}
}

void ComboTreeNodeActionList::clear() {
	ComboTreeNodeActionList* pCur = Next;

	while (pCur != nullptr) {
		auto pTemp = pCur;
		pCur = pCur->Next;
		delete pTemp;
	}

	Action_ = nullptr;
	Next = nullptr;
}

int ComboTreeNodeActionList::count() {

	if (Action_ == nullptr)
		return 0;

	int iCount = 1;
	ComboTreeNodeActionList* pCur = Next;
	while (pCur != nullptr) {

		if (pCur->Action_)
			++iCount;

		pCur = pCur->Next;
	}

	return iCount;
}

bool ComboTreeNodeActionList::exist(SGAction* action) {
	return find_if([action](SGAction* a) { return action == a; }) != nullptr;
}

SGAction* ComboTreeNodeActionList::find_if(const SGPredicateFn<SGAction*>& fn) {
	ComboTreeNodeActionList* pCur = this;

	while (pCur) {
		if (pCur->Action_ && fn(pCur->Action_)) {
			return pCur->Action_;
		}
		pCur = pCur->Next;
	}

	return nullptr;
}

int SGComboTreeNode::count() {
	int iCount = 0;

	for (int i = 0; i < ControlKey::Max; ++i)
		if (Next[i] != nullptr && Next[i]->isValid())
			iCount++;

	return iCount;
}

SGComboTree::SGComboTree() {
	m_pRoot = dbg_new SGComboTreeNode{};
}

SGComboTree::~SGComboTree() {
	removeAll();
	DeleteSafe(m_pRoot);
}

void SGComboTree::addComboAction(SGAction* action) {
	const ComboKeyList& keys = action->getCommand();
	const ComboKeyList& keysReverse = keys.reverse();

	int iCount = keys.count();
	DebugAssertMsg(iCount != 0, "정방향 키가 없습니다.");

	SGComboTreeNode* pFindNode = findComboNodeRecursive(m_pRoot, keys, iCount - 1, iCount);

	if (pFindNode == nullptr) {
		SGComboTreeNode* pNewNode = dbg_new SGComboTreeNode{};
		pNewNode->ActionList.add(action);
		addComboNodeRecursive(m_pRoot, pNewNode, keys, 0, iCount);
		++m_iNodeCount;
	} else {
		pFindNode->ActionList.add(action);
	}

	if (keys == keysReverse) {
		return;
	}

	int iCountReverse = keysReverse.count();
	DebugAssertMsg(iCountReverse != 0, "반대 방향 키가 없습니다.");

	pFindNode = findComboNodeRecursive(m_pRoot, keysReverse, iCountReverse - 1, iCountReverse);

	if (pFindNode == nullptr) {
		SGComboTreeNode* pNewNode = dbg_new SGComboTreeNode{};
		pNewNode->ActionList.add(action);
		addComboNodeRecursive(m_pRoot, pNewNode, keysReverse, 0, iCountReverse);
		++m_iNodeCount;
	} else {
		pFindNode->ActionList.add(action);
	}
}

// 안씀

void SGComboTree::removeAll() {
	// 루트빼고 다 삭제
	for (int i = 0; i < ControlKey::Max; ++i) {
		removeComboNodeRecursive(m_pRoot->Next[i]);
		m_pRoot->Next[i] = nullptr;
	}
}


// -> -> X 를 입력했으면
// X -> -> 로 들어옴
SGAction* SGComboTree::getComboAction(const ComboKeyList& keys) {
	int iCount = keys.count();

	if (iCount == 0)
		return nullptr;

	SGComboTreeNode* pFindNode = findComboNodeRecursive(m_pRoot, keys, iCount - 1, iCount);

	if (pFindNode == nullptr || pFindNode->empty()) {
		return nullptr;
	}


	SGAction* pAction = pFindNode->ActionList.find_if([](SGAction* action) {
		return action->onConditionCheck();
	});

	return pAction;
}

void SGComboTree::removeComboNodeRecursive(SGComboTreeNode* parent) {
	if (parent == nullptr)
		return;

	for (int i = 0; i < ControlKey::Max; ++i) {
		removeComboNodeRecursive(parent->Next[i]);
	}

	parent->ActionList.clear();
	delete parent;
}


void SGComboTree::addComboNodeRecursive(
	SGComboTreeNode* parent, 
	SGComboTreeNode* newNode, 
	const ComboKeyList& keys, 
	int keyIndex, 
	int keyCount)
{
	ControlKey_t eKey = keys.at(keyIndex);
	SGComboTreeNode* pCur = parent->Next[eKey];

	if (keyIndex == keyCount - 1) {
		// 한줄이면 끝나는데
		// 잘못된 코드 캐치하기 위함.
		if (pCur == nullptr) 
			parent->Next[eKey] = newNode;
		else {
			DebugAssertMsg(pCur->empty(), "이미 해당 콤보키가 바인딩 되어 있습니다.");
			parent->Next[eKey] = newNode;
		}
		return;
	}

	// 가는 경로에 노드가 없으면 생성
	if (pCur == nullptr) {
		parent->Next[eKey] = dbg_new SGComboTreeNode;
	}

	addComboNodeRecursive(parent->Next[eKey], newNode, keys, keyIndex + 1, keyCount);
}



SGComboTreeNode* SGComboTree::findComboNodeRecursive(
	SGComboTreeNode* parent, 
	const ComboKeyList& keys, 
	int keyIndex,
	int keyCount)
{
	if (parent == nullptr) {
		return nullptr;
	}

	ControlKey_t eKey = keys.at(keyIndex);
	SGComboTreeNode* pCur = parent->Next[eKey];

	if (keyIndex == 0) {
		return pCur;
	}

	return findComboNodeRecursive(pCur, keys, keyIndex - 1, keyCount);
}

