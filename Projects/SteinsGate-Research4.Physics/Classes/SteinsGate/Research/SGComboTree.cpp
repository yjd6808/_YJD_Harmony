/*
 * 작성자: 윤정도
 * 생성일: 1/12/2023 8:09:21 AM
 * =====================
 *
 */


#include "SGComboTree.h"

int SGComboTreeNode::Count() {
	int iCount = 0;

	for (int i = 0; i < ControlKey::Max; ++i)
		if (Next[i] != nullptr && Next[i]->IsValid())
			iCount++;

	return iCount;
}

SGComboTree::SGComboTree() {
	m_pRoot = new SGComboTreeNode{};
}

SGComboTree::~SGComboTree() {
	RemoveAll();
	DeleteSafe(m_pRoot);
}

void SGComboTree::AddComboAction(SGComboAction* action) {
	SGComboKeyList* keys = action->m_pComboKeys;

	for (int i = 0; i < action->getComboKeyCount(); ++i) {
		int iCount = keys[i].Count();
		DebugAssertMessage(iCount != 0, "키가 없습니다.");

		SGComboTreeNode* pNewNode = new SGComboTreeNode{};
		pNewNode->Action = action;
		AddComboNodeRecursive(m_pRoot, pNewNode, keys[i], 0, iCount);
		++m_iCount;
	}
}

void SGComboTree::RemoveComboAction(const SGComboKeyList& keys) {
	int iCount = keys.Count();
	DebugAssertMessage(iCount != 0, "키가 없습니다.");

	SGComboTreeNode* pFindNode = FindComboNodeRecursive(m_pRoot, keys, 0, iCount);
	DebugAssertMessage(pFindNode != nullptr, "해당하는 콤보 액션이 null입니다..");
	DebugAssertMessage(pFindNode->IsValid(), "해당하는 콤보 액션이 비어있습니다.");
	pFindNode->Action = nullptr;
	--m_iCount;
}

void SGComboTree::RemoveAll() {
	// 루트빼고 다 삭제
	for (int i = 0; i < ControlKey::Max; ++i) {
		RemoveComboNodeRecursive(m_pRoot->Next[i]);
		m_pRoot->Next[i] = nullptr;
	}
}


// -> -> X 를 입력했으면
// X -> -> 로 들어옴
SGComboAction* SGComboTree::GetComboAction(const SGComboKeyList& keys) {
	int iCount = keys.Count();

	if (iCount == 0)
		return nullptr;

	SGComboTreeNode* pFindNode = FindComboNodeRecursive(m_pRoot, keys, iCount - 1, iCount);

	if (pFindNode == nullptr || pFindNode->Empty()) {
		return nullptr;
	}

	return pFindNode->Action;
}

void SGComboTree::RemoveComboNodeRecursive(SGComboTreeNode* parent) {
	if (parent == nullptr)
		return;

	for (int i = 0; i < ControlKey::Max; ++i) {
		RemoveComboNodeRecursive(parent->Next[i]);
	}

	delete parent;
}


void SGComboTree::AddComboNodeRecursive(
	SGComboTreeNode* parent, 
	SGComboTreeNode* newNode, 
	const SGComboKeyList& keys, 
	int keyIndex, 
	int keyCount)
{
	ControlKey_t eKey = keys.At(keyIndex);
	SGComboTreeNode* pCur = parent->Next[eKey];

	if (keyIndex == keyCount - 1) {

		// 한줄이면 끝나는데
		// 잘못된 코드 캐치하기 위함.
		if (pCur == nullptr) 
			parent->Next[eKey] = newNode;
		else {
			DebugAssertMessage(pCur->Empty(), "이미 해당 콤보키가 바인딩 되어 있습니다.");
			parent->Next[eKey] = newNode;
		}
		return;
	}

	// 가는 경로에 노드가 없으면 생성
	if (pCur == nullptr) {
		parent->Next[eKey] = new SGComboTreeNode;
	}

	AddComboNodeRecursive(parent->Next[eKey], newNode, keys, keyIndex + 1, keyCount);
}



SGComboTreeNode* SGComboTree::FindComboNodeRecursive(
	SGComboTreeNode* parent, 
	const SGComboKeyList& keys, 
	int keyIndex,
	int keyCount)
{
	if (parent == nullptr) {
		return nullptr;
	}

	ControlKey_t eKey = keys.At(keyIndex);
	SGComboTreeNode* pCur = parent->Next[eKey];

	if (keyIndex == 0) {
		return pCur;
	}

	return FindComboNodeRecursive(pCur, keys, keyIndex - 1, keyCount);
}

