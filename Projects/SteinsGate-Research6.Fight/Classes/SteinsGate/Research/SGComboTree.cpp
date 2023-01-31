/*
 * 작성자: 윤정도
 * 생성일: 1/12/2023 8:09:21 AM
 * =====================
 *
 */


#include "SGComboTree.h"

int SGComboTreeNode::count() {
	int iCount = 0;

	for (int i = 0; i < ControlKey::Max; ++i)
		if (Next[i] != nullptr && Next[i]->isValid())
			iCount++;

	return iCount;
}

SGComboTree::SGComboTree() {
	m_pRoot = new SGComboTreeNode{};
}

SGComboTree::~SGComboTree() {
	removeAll();
	DeleteSafe(m_pRoot);
}

void SGComboTree::addComboAction(SGAction* action) {
	const SGComboKeyList& keys = action->getCommand();
	const SGComboKeyList& keysReverse = keys.reverse();

	int iCount = keys.count();
	DebugAssertMessage(iCount != 0, "정방향 키가 없습니다.");
	SGComboTreeNode* pNewNode = new SGComboTreeNode{};
	pNewNode->Action = action;
	addComboNodeRecursive(m_pRoot, pNewNode, keys, 0, iCount);
	++m_iCount;

	if (keys == keysReverse) {
		return;
	}

	int iCountReverse = keysReverse.count();
	DebugAssertMessage(iCountReverse != 0, "반대 방향 키가 없습니다.");
	SGComboTreeNode* pNewNodeReverse = new SGComboTreeNode{};
	pNewNodeReverse->Action = action;
	addComboNodeRecursive(m_pRoot, pNewNodeReverse, keysReverse, 0, iCountReverse);
	++m_iCount;
}

// 안씀
void SGComboTree::removeComboAction(const SGComboKeyList& keys) {
	int iCount = keys.count();
	DebugAssertMessage(iCount != 0, "키가 없습니다.");

	SGComboTreeNode* pFindNode = findComboNodeRecursive(m_pRoot, keys, 0, iCount);
	DebugAssertMessage(pFindNode != nullptr, "해당하는 콤보 액션이 null입니다..");
	DebugAssertMessage(pFindNode->isValid(), "해당하는 콤보 액션이 비어있습니다.");
	pFindNode->Action = nullptr;
	--m_iCount;
}

void SGComboTree::removeAll() {
	// 루트빼고 다 삭제
	for (int i = 0; i < ControlKey::Max; ++i) {
		removeComboNodeRecursive(m_pRoot->Next[i]);
		m_pRoot->Next[i] = nullptr;
	}
}


// -> -> X 를 입력했으면
// X -> -> 로 들어옴
SGAction* SGComboTree::getComboAction(const SGComboKeyList& keys) {
	int iCount = keys.count();

	if (iCount == 0)
		return nullptr;

	SGComboTreeNode* pFindNode = findComboNodeRecursive(m_pRoot, keys, iCount - 1, iCount);

	if (pFindNode == nullptr || pFindNode->empty()) {
		return nullptr;
	}

	return pFindNode->Action;
}

void SGComboTree::removeComboNodeRecursive(SGComboTreeNode* parent) {
	if (parent == nullptr)
		return;

	for (int i = 0; i < ControlKey::Max; ++i) {
		removeComboNodeRecursive(parent->Next[i]);
	}

	delete parent;
}


void SGComboTree::addComboNodeRecursive(
	SGComboTreeNode* parent, 
	SGComboTreeNode* newNode, 
	const SGComboKeyList& keys, 
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
			DebugAssertMessage(pCur->empty(), "이미 해당 콤보키가 바인딩 되어 있습니다.");
			parent->Next[eKey] = newNode;
		}
		return;
	}

	// 가는 경로에 노드가 없으면 생성
	if (pCur == nullptr) {
		parent->Next[eKey] = new SGComboTreeNode;
	}

	addComboNodeRecursive(parent->Next[eKey], newNode, keys, keyIndex + 1, keyCount);
}



SGComboTreeNode* SGComboTree::findComboNodeRecursive(
	SGComboTreeNode* parent, 
	const SGComboKeyList& keys, 
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

