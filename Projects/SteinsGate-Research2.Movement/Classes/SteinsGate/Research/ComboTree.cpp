/*
 * 작성자: 윤정도
 * 생성일: 1/12/2023 8:09:21 AM
 * =====================
 *
 */


#include "ComboTree.h"

int ComboTreeNode::Count() {
	int iCount = 0;

	for (int i = 0; i < ControlKey::Max; ++i)
		if (Next[i] != nullptr && Next[i]->IsValid())
			iCount++;

	return iCount;
}

ComboTree::ComboTree() {
	m_pRoot = new ComboTreeNode{};
}

ComboTree::~ComboTree() {
	RemoveAll();
	DeleteSafe(m_pRoot);
}

void ComboTree::AddComboAction(const ComboActionPtr& action) {
	ComboKeyList& keys = action->m_ComboKeys;
	int iCount = keys.Count();
	DebugAssertMessage(iCount != 0, "키가 없습니다.");

	ComboTreeNode* pNewNode = new ComboTreeNode{};
	pNewNode->Action = action;
	AddComboNodeRecursive(m_pRoot, pNewNode, keys, 0, keys.Count());
	++m_iCount;
}

void ComboTree::AddComboAction(ComboActionPtr&& action) {
	ComboKeyList& keys = action->m_ComboKeys;
	int iCount = keys.Count();
	DebugAssertMessage(iCount != 0, "키가 없습니다.");

	ComboTreeNode* pNewNode = new ComboTreeNode{};
	pNewNode->Action = JCore::Move(action);
	AddComboNodeRecursive(m_pRoot, pNewNode, keys, 0, iCount);
	++m_iCount;
}

void ComboTree::RemoveComboAction(const ComboKeyList& keys) {
	int iCount = keys.Count();
	DebugAssertMessage(iCount != 0, "키가 없습니다.");

	ComboTreeNode* pFindNode = FindComboNodeRecursive(m_pRoot, keys, 0, iCount);
	DebugAssertMessage(pFindNode != nullptr, "해당하는 콤보 액션이 null입니다..");
	DebugAssertMessage(pFindNode->IsValid(), "해당하는 콤보 액션이 비어있습니다.");
	pFindNode->Action = nullptr;
	--m_iCount;
}

void ComboTree::RemoveAll() {
	// 루트빼고 다 삭제
	for (int i = 0; i < ControlKey::Max; ++i) {
		RemoveComboNodeRecursive(m_pRoot->Next[i]);
		m_pRoot->Next[i] = nullptr;
	}
}


// -> -> X 를 입력했으면
// X -> -> 로 들어옴
ComboActionPtr ComboTree::GetComboAction(const ComboKeyList& keys) {
	int iCount = keys.Count();

	if (iCount == 0) 
		return nullptr;

	if (iCount == 2)
		int a = 30;

	if (iCount == 3)
		int a = 40;

	ComboTreeNode* pFindNode = FindComboNodeRecursive(m_pRoot, keys, iCount - 1, iCount);

	if (pFindNode == nullptr || pFindNode->Empty()) {
		return nullptr;
	}

	return pFindNode->Action;
}

void ComboTree::RemoveComboNodeRecursive(ComboTreeNode* parent) {
	if (parent == nullptr)
		return;

	for (int i = 0; i < ControlKey::Max; ++i) {
		RemoveComboNodeRecursive(parent->Next[i]);
	}

	delete parent;
}


void ComboTree::AddComboNodeRecursive(
	ComboTreeNode* parent, 
	ComboTreeNode* newNode, 
	const ComboKeyList& keys, 
	int keyIndex, 
	int keyCount)
{
	ControlKey_t eKey = keys.At(keyIndex);
	ComboTreeNode* pCur = parent->Next[eKey];

	if (keyIndex == keyCount - 1) {
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
		parent->Next[eKey] = new ComboTreeNode;
	}

	AddComboNodeRecursive(parent->Next[eKey], newNode, keys, keyIndex + 1, keyCount);
}



ComboTreeNode* ComboTree::FindComboNodeRecursive(
	ComboTreeNode* parent, 
	const ComboKeyList& keys, 
	int keyIndex,
	int keyCount)
{
	if (parent == nullptr) {
		return nullptr;
	}

	ControlKey_t eKey = keys.At(keyIndex);
	ComboTreeNode* pCur = parent->Next[eKey];

	if (keyIndex == 0) {
		return pCur;
	}

	return FindComboNodeRecursive(pCur, keys, keyIndex - 1, keyCount);
}

