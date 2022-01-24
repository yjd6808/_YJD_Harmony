/*
	작성자 : 윤정도
	이전에 작성한 AVL 트리 성능 개선
	 - 메모이제이션을 적용합니다.
*/

#include <JCoreTest/CoreTest.h>
#include <JCore/Core.h>
#include <JCore/TypeTraits.h>
#include <JCore/Random.h>
#include <JCore/Math.h>

using namespace std;
using namespace JCore;

#if TEST_AVLTreeMemoImplTest == ON

#define INSERTION_BALANCE ON		// 밸런싱 사용할지
#if INSERTION_BALANCE == ON
	#define DELETION_BALANCE ON		// 삭제시 밸런싱 사용할지
#endif

namespace AVLMemo {

struct Node
{
	int Data = 0;
	int Height = 0;
	Node* Left = nullptr;
	Node* Right = nullptr;
	Node* Parent = nullptr;

	Node* Any() {
		if (Left) {
			return Left;
		}

		if (Right) {
			return Right;
		}

		return nullptr;
	}

	bool IsTerminal() {
		return Left == nullptr && Right == nullptr;
	}

	int ChildCount() {
		if (Left && Right) {
			return 2;
		}

		if (Left || Right) {
			return 1;
		}

		return 0;
	}

	void DeleteChild(Node* node) {
		if (Left == node) {
			DeleteSafe(Left);
		} else {
			DeleteSafe(Right);
		}
	}

	void UpdateHeight() {
		int iLeftHeight = Left ? Left->Height + 1 : 0;
		int iRightHeight = Right ? Right->Height + 1 : 0;
		
		this->Height = Math::Max(iLeftHeight, iRightHeight);
	}

	int LeftSubtreeHeight() {
		return Left ? Left->Height : 0;
	}

	int RightSubtreeHeight() {
		return Right ? Right->Height : 0;
	}
};

class AVLTree
{
public:
	AVLTree() {
		m_pVirtualRoot = new Node;
		m_iSize = 0;
	}
	~AVLTree() {
		Clear();
		DeleteSafe(m_pVirtualRoot);
	}
public:
	void Add(int data) {
		if (m_iSize == 0) {
			m_pVirtualRoot->Left = new Node{ data };
			m_pVirtualRoot->Left->Parent = m_pVirtualRoot;
			m_iSize++;
			return;
		}

		Node* pNewNode = Add(m_pVirtualRoot->Any(), data);
	#if INSERTION_BALANCE == ON
		BalanceStart(pNewNode);
	#endif
		m_iSize++;
	}

	bool Remove(int data) {

		if (m_iSize == 0) {
			return false;
		}

		Node* pParent = m_pVirtualRoot;
		Node* pDel = m_pVirtualRoot->Any();

		while (pDel != nullptr) {
			if (pDel->Data == data) {
				break;
			}

			if (data > pDel->Data)
				pDel = pDel->Right;
			else
				pDel = pDel->Left;
		}

		if (pDel == nullptr) {
			return false;
		}

		pParent = pDel->Parent;
		int iChildCount = pDel->ChildCount();
		Node* pBalanceStartingNode = nullptr;	// 밸런싱을 진행할 노드

		if (iChildCount == 0) {
			pParent->DeleteChild(pDel);
			pBalanceStartingNode = pParent;	
		} else if (iChildCount == 1) {
			Node* pChild = pDel->Any();

			if (pParent == m_pVirtualRoot) {
				pParent->Left = pChild;
			} else {
				if (pParent->Right == pDel) {
					pParent->Right = pChild;
				} else {
					pParent->Left = pChild;
				}
				
			}

			pBalanceStartingNode = pParent;
			pChild->Parent = pParent;
			DeleteSafe(pDel);
		} else {
			Node* pSmallestParent = pDel;
			Node* pSmallest = pDel->Right;

			while (pSmallest->Left != nullptr) {
				pSmallest = pSmallest->Left;
			}

			pSmallestParent = pSmallest->Parent;
			pDel->Data = Move(pSmallest->Data);

			if (pSmallestParent->Right == pSmallest) {
				pSmallestParent->Right = pSmallest->Right;
				if (pSmallest->Right)
					pSmallest->Right->Parent = pSmallestParent;
			} else {
				pSmallestParent->Left = pSmallest->Right;
				if (pSmallest->Right)
					pSmallest->Right->Parent = pSmallestParent;
			}
			pBalanceStartingNode = pSmallestParent;
			DeleteSafe(pSmallest);
		}

	#if DELETION_BALANCE == ON
		if (pBalanceStartingNode)
			BalanceStart(pBalanceStartingNode);
	#endif
		m_iSize--;
		return true;
	}

	void ShowDistribution() {
		vector<vector<int>> values;
		values.resize(30);
		int maxDepth = Show(values, m_pVirtualRoot->Any(), 0);
		PrintLine("각 층별 꽉찬 정도를 출력");
		for (int i = 0; i < maxDepth; i++) {
			PrintLine("%d층 : %.6f%%%", i + 1, (double)values[i].size() / Math::Pow(2, i) * 100);
		}
	}

	void Clear() {
		Node* pRoot = m_pVirtualRoot->Any();

		if (pRoot == nullptr) {
			return;
		}

		DeleteElementRecursive(pRoot->Left);
		DeleteElementRecursive(pRoot->Right);
		DeleteSafe(pRoot);
	}
private:
	void DeleteElementRecursive(Node* node) {
		if (node == nullptr) {
			return;
		}

		DeleteElementRecursive(node->Left);
		DeleteElementRecursive(node->Right);
		DeleteSafe(node);
	}

	int Show(vector<vector<int>>& values, Node* node, int depth) {
		if (node == nullptr) {
			return depth;
		}
		values[depth].push_back(node->Data);
		return Math::Max(Show(values, node->Left, depth + 1), Show(values, node->Right, depth + 1));
	}

	Node* Add(Node* root, int data) {
		Node* pParent = nullptr;
		Node* pCur = root;
		Node* pNewNode = new Node{ data };
		int iComp = 0;

		while (pCur != nullptr) {
			iComp = Compare(pCur->Data, data);
			pParent = pCur;
			pCur = iComp > 0 ? pCur->Right : pCur->Left;
		}

		if (iComp <= 0) {
			pParent->Left = pNewNode;
			pParent->Left->Parent = pParent;
		} else {
			pParent->Right = pNewNode;
			pParent->Right->Parent = pParent;
		}

		return pNewNode;
	}

	void BalanceStart(Node* node) {
		Node* pCur = node;

		while (pCur != m_pVirtualRoot) {
			pCur->UpdateHeight();
			Balance(pCur);
			pCur = pCur->Parent;
		}
	}

	// 해당 노드 기준으로 밸런싱 진행
	void Balance(Node* node) {
		int iHeightDiff = GetHeightDiff(node);
		if (iHeightDiff >= 2) {

			int iChildHeightDiff = GetHeightDiff(node->Right);
			if (iChildHeightDiff >= 1) {
				RotateRR(node);
			} else {
				RotateRL(node);
			}
		}
		else if (iHeightDiff <= -2) {
			int iChildHeightDiff = GetHeightDiff(node->Left);
			if (iChildHeightDiff <= -1) {
				RotateLL(node);
			} else {
				RotateLR(node);
			}
		}
	}

	int GetHeightDiff(Node* node) {
		return node->RightSubtreeHeight() - node->LeftSubtreeHeight();
	}


	void RotateRR(Node* cur) {
		Node* pParent = cur->Parent;
		Node* pCur = cur;
		Node* pChild = cur->Right;
		Node* pChildLeft = cur->Right->Left;

		if (pParent->Left == pCur) 
			pParent->Left = pChild;
		else
			pParent->Right = pChild;
		pChild->Parent = pParent;


		pCur->Right = pChildLeft;
		if (pChildLeft)
			pChildLeft->Parent = pCur;

		pChild->Left = pCur;
		pCur->Parent = pChild;

		
	#if INSERTION_BALANCE == ON
		// 회전 후 높이를 갱신해준다.
		pChild->UpdateHeight();
		pCur->UpdateHeight();
	#endif
	}

	void RotateLR(Node* cur) {
		RotateRR(cur->Left);
		RotateLL(cur);
	}

	void RotateRL(Node* cur) {
		RotateLL(cur->Right);
		RotateRR(cur);
	}

	void RotateLL(Node* cur) {
		Node* pParent = cur->Parent;
		Node* pCur = cur;
		Node* pChild = cur->Left;
		Node* pChildRight = cur->Left->Right;

		if (pParent->Left == pCur)
			pParent->Left = pChild;
		else
			pParent->Right = pChild;
		pChild->Parent = pParent;

		pCur->Left = pChildRight;
		if (pChildRight)
			pChildRight->Parent = pCur;

		pChild->Right = pCur;
		pCur->Parent = pChild;

		// 회전 후 높이를 갱신해준다.
	#if INSERTION_BALANCE == ON
		pChild->UpdateHeight();
		pCur->UpdateHeight();
	#endif
	}


	int Compare(int src, int dst) {
		if (src < dst) {
			return 1;
		}

		if (src > dst) {
			return -1;
		}

		return 0;
	}

	
private:
	Node* m_pVirtualRoot;
	int m_iSize;
};

} // namespace AVLMemo

TEST(AVLTreeMemoImplTest, AVLTreeMemoImplTest) {
	AVLMemo::AVLTree tree;

	const int MANY_TEST = 10;
	const int MANY_COUNT = 1000;

	const int MIN_TEST = 1000;
	const int MIN_COUNT = 10;

	const int ACCURATE_TEST = 1000;
	const int ACCURATE_COUNT = 1000;

	Random rand;
	std::vector<int> vec;

	//for (int k = 0; k < MIN_COUNT + 300; k++) {
	//	vec.push_back(rand.GenerateInt(0, 20));
	//	tree.Add(vec[k]);
	//}
	////tree.ShowDistribution();
	//for (int k = 0; k < MIN_COUNT + 300; k++) {
	//	if (!tree.Remove(vec[k])) {
	//		goto FAILED;
	//	}
	//}

	//vec.clear();

	// 정확성 테스트
	for (int i = 0; i < ACCURATE_TEST; i++) {
		for (int k = 0; k < ACCURATE_COUNT; k++) {
			vec.push_back(rand.GenerateInt(0, 1000));
			tree.Add(vec[k]);
		}

		for (int k = 0; k < ACCURATE_COUNT; k++) {
			if (!tree.Remove(vec[k])) {
				goto FAILED;
			}
			
		}

		vec.clear();
	}

	//// 많은 수의 원소 테스트
	//for (int i = 0; i < MANY_TEST; i++) {
	//	for (int k = 0; k < MANY_COUNT; k++) {
	//		vec.push_back(rand.GenerateInt(0, 100));
	//		tree.Add(vec[k]);
	//	}

	//	for (int k = 0; k < MANY_COUNT; k++) {
	//		if (!tree.Remove(vec[k])) {
	//			goto FAILED;
	//		}
	//	}
	//	vec.clear();
	//}

	//// 적은 수의 원소 테스트
	//for (int i = 0; i < MIN_TEST; i++) {
	//	for (int k = 0; k < MIN_COUNT; k++) {
	//		vec.push_back(rand.GenerateInt(0, 100));
	//		tree.Add(vec[k]);
	//	}

	//	for (int k = 0; k < MIN_COUNT; k++) {
	//		if (!tree.Remove(vec[k])) {
	//			goto FAILED;
	//		}
	//	}
	//	vec.clear();
	//}
	return;
FAILED:
	EXPECT_TRUE(false);
}

#endif // TEST_AVLTreeMemoImplTest == ON