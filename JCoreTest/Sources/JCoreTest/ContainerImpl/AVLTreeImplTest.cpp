/*
	�ۼ��� : ������
	AVL Ʈ�� �н� �� �����غ���
*/

#include <JCoreTest/CoreTest.h>
#include <JCore/Core.h>
#include <JCore/TypeTraits.h>
#include <JCore/Random.h>
#include <JCore/Math.h>

using namespace std;
using namespace JCore;

#if TEST_AVLTreeImplTest == ON

#define INSERTION_BALANCE ON		// ���� �뷱�� �������

#if INSERTION_BALANCE == ON
	#define DELETION_BALANCE ON		// ���� �뷱�� �������
#endif


namespace AVL {

struct Node
{
	int Data = 0;
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
		} else if (Right == node) {
			DeleteSafe(Right);
		} else {
			assert("����?");
		}
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
		Node* pDel = FindNode(data);	// ������ ��带 ã�´�.

		// ������ ��带 ��ã�� ���
		if (pDel == nullptr) {
			return false;
		}

		Node* pBalanceStartingNode = nullptr;	// �뷱���� ������ ��� = ���������� ������ ����� �θ� ���
		int iChildCount = pDel->ChildCount();

		if (iChildCount == 0) {
			pBalanceStartingNode = RemoveDelNoChild(pDel);
		} else if (iChildCount == 1) {
			pBalanceStartingNode = RemoveDelOneChild(pDel);
		} else {
			pBalanceStartingNode = RemoveDelTwoChild(pDel);
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
		PrintFormat("�� ���� ���� ������ ���\n");
		for (int i = 0; i < maxDepth; i++) {
			PrintFormat("%d�� : %.6f%%%\n", i + 1, (double)values[i].size() / Math::Pow(2, i) * 100);
		}
	}

	bool Exist(int data) {
		return FindNode(data) != nullptr;
	}

	void Clear() {
		Node* pRoot = m_pVirtualRoot->Any();

		if (pRoot == nullptr) {
			return;
		}

		DeleteElementRecursive(pRoot->Left);
		DeleteElementRecursive(pRoot->Right);
		DeleteSafe(pRoot);
		m_iSize = 0;
	}
private:
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

	void DeleteElementRecursive(Node* node) {
		if (node == nullptr) {
			return;
		}

		DeleteElementRecursive(node->Left);
		DeleteElementRecursive(node->Right);
		DeleteSafe(node);
	}

	Node* RemoveDelNoChild(Node* del) {
		// 1. ������ ��尡 �ڽ��� ���� ���
		//		�θ𿡰Լ� �ش� ��带 �����ش�.
		Node* pParent = del->Parent;
		pParent->DeleteChild(del);
		return pParent;
	}

	Node* RemoveDelOneChild(Node* del) {
		// 2. ������ ��尡 �ڽ��� �Ѱ��� ���
		//		1. ������ ��尡 �θ���� �����ڽ��� ���
		//			������ ����� �ڽ��� ������ ����� �θ� ������ �ٿ��ش�.
		//		[���� ��]
		//		A - parent
		//			B - del
		//				C
		// 
		//		[���� ��]
		//		A - parent
		//			C
		// 
		//		2. ������ ��尡 �θ���� �����ڽ��� ���
		//			������ ����� �ڽ��� ������ ����� �θ� ������ �ٿ��ش�.

		Node* pParent = del->Parent;
		Node* pChild = del->Any();
		Node* pBalanceStartingNode = nullptr;

		
		if (pParent == m_pVirtualRoot) {
			// ����ó�� ���� �θ� ���߾� ����� ������ ���ʿ� ���̵�������.
			// --> ó�� AVLƮ�� ������ ��� ������ �� ���ʿ� ������ ù ��带 ���̵��� �߱� ����
			pParent->Left = pChild;
		} else {
			if (pParent->Right == del) {
				pParent->Right = pChild;
			} else {
				pParent->Left = pChild;
			}
			pBalanceStartingNode = pParent;
		}

		pChild->Parent = pParent;
		DeleteSafe(del);
		return pBalanceStartingNode;
	}

	Node* RemoveDelTwoChild(Node* del) {
		// 3. ������ ��尡 �ڽ��� 2���� ���
		//		1. ������ ����� ���� �ڽ��� ����Ʈ������ ���� ū ��带 ã�´�. (���1)
		//			1. ���� ū ����� ���� ������ ����� ���� �������ش�.
		//			2. ���� ū ����� ���� �ڽ��� ���� ū ����� �θ��� ���� �ڽ����� ������ش�.
		//			3. ���� ū ��带 �������ش�.
		// 
		//		2. ������ ����� ���� �ڽ��� ����Ʈ������ ���� ���� ��带 ã�´�. (���2)
		//			1. ���� ���� ����� ���� ������ ����� ���� �������ش�.
		//			2. ���� ���� ����� ���� �ڽ��� ���� ���� ����� �θ��� ���� �ڽ����� ������ش�.
		//			3. ���� ���� ��带 �������ش�.

		// --> ���2�� ����

		Node* pSmallestParent = del;		// ������ ����� �θ�
		Node* pSmallest = del->Right;		// ������ ���

		while (pSmallest->Left != nullptr) {
			pSmallest = pSmallest->Left;
		}

		pSmallestParent = pSmallest->Parent;
		del->Data = Move(pSmallest->Data);

		if (pSmallestParent->Right == pSmallest) {
			// ������ ���� ���� ��尡 �޸���� 
			// �� ���� ���� while���� �ѹ��� �ȵ� ����̴�.
			//   = "������ ��庸�� �� ���� ��尡 �ƿ� ���� ���"

			pSmallestParent->Right = pSmallest->Right;
			if (pSmallest->Right)
				pSmallest->Right->Parent = pSmallestParent;
		} else {
			pSmallestParent->Left = pSmallest->Right;
			if (pSmallest->Right)
				pSmallest->Right->Parent = pSmallestParent;
		}
		DeleteSafe(pSmallest);
		return pSmallestParent;
	}

	int Show(vector<vector<int>>& values, Node* node, int depth) {
		if (node == nullptr) {
			return depth;
		}
		values[depth].push_back(node->Data);
		return Math::Max(Show(values, node->Left, depth + 1), Show(values, node->Right, depth + 1));
	}

	

	void BalanceStart(Node* node) {
		Node* pCur = node;

		while (pCur != m_pVirtualRoot) {
			Balance(pCur);
			pCur = pCur->Parent;
		}
	}

	// �ش� ��� �������� �뷱�� ����
	void Balance(Node* node) {
		int iHeightDiff = GetHeightDiff(node);
		// ���� ���̰� ���� ���̺��� 2�̻� ���� ���
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

	Node* FindNode(int data) {
		Node* pCur = m_pVirtualRoot->Any();

		while (pCur != nullptr) {
			if (pCur->Data == data) {
				return pCur;
			}

			if (pCur->Data < data) {
				pCur = pCur->Right;
			} else {
				pCur = pCur->Left;
			}
		}

		return nullptr;
	}

	int GetHeightDiff(Node* node) {
		int iLeftSubtreeHeight = GetHeight(node->Left, 0);
		int iRightSubtreeHeight = GetHeight(node->Right, 0);

		return iRightSubtreeHeight - iLeftSubtreeHeight;
	}

	int GetHeight(Node* cur, int height) {
		if (cur == nullptr) {
			return height;
		}
		int iLeftHeight = GetHeight(cur->Left, height + 1);
		int iRightHeight = GetHeight(cur->Right, height + 1);

		return Math::Max(iLeftHeight, iRightHeight);
	}


	void RotateRR(Node* cur) {
		//  ?   		- ? : pParent
		//    1 		- 1 : pCur
		//      3		- 3 : pChild
		//    ?	  5  	- ? : pChildLeft

		//  ?			- ? : pParent
		//    3			- 3 : pChild
		//  1   5		- 1 : pCur
		//   ?			- ? : pChildLeft

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
		//        ?		- pParent
		//      5		- pCur
		//    3			- pChild
		//  1   ?		- pChildRight
		
		//      ?		- pParent
		//    3			- pChild
		//  1   5		- pCur
		//    ?			- pChildRight
	
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

} // namespace AVL


TEST(AVLTreeImplTest, AVLTreeImplTest) {
	AVL::AVLTree tree;

	const int MANY_TEST = 10;
	const int MANY_COUNT = 1000;

	const int MIN_TEST = 1000;
	const int MIN_COUNT = 10;

	const int ACCURATE_TEST = 100;
	const int ACCURATE_COUNT = 100;

	Random rand;
	std::vector<int> vec;

	for (int k = 0; k < MIN_COUNT + 300; k++) {
		vec.push_back(rand.GenerateInt(0, 20));
		tree.Add(vec[k]);
	}
	//tree.ShowDistribution();
	for (int k = 0; k < MIN_COUNT + 300; k++) {
		if (!tree.Remove(vec[k])) {
			goto FAILED;
		}
	}
	
	vec.clear();

	// ��Ȯ�� �׽�Ʈ
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

	// ���� ���� ���� �׽�Ʈ
	for (int i = 0; i < MANY_TEST; i++) {
		for (int k = 0; k < MANY_COUNT; k++) {
			vec.push_back(rand.GenerateInt(0, 100));
			tree.Add(vec[k]);
		}

		for (int k = 0; k < MANY_COUNT; k++) {
			if (!tree.Remove(vec[k])) {
				goto FAILED;
			}
		}
		vec.clear();
	}

	// ���� ���� ���� �׽�Ʈ
	for (int i = 0; i < MIN_TEST; i++) {
		for (int k = 0; k < MIN_COUNT; k++) {
			vec.push_back(rand.GenerateInt(0, 100));
			tree.Add(vec[k]);
		}

		for (int k = 0; k < MIN_COUNT; k++) {
			if (!tree.Remove(vec[k])) {
				goto FAILED;
			}
		}
		vec.clear();
	}
	return;
FAILED:
	EXPECT_TRUE(false);
}

#endif // TEST_AVLTreeImplTest == ON