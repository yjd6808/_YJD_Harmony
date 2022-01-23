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
		BalanceStart(pNewNode);
		m_iSize++;
	}

	bool Remove(int data) {

		if (m_iSize == 0) {
			return false;
		}

		Node* pParent = m_pVirtualRoot;
		Node* pDel = m_pVirtualRoot->Any();

		// ������ ��带 ã�´�.
		while (pDel != nullptr) {
			if (pDel->Data == data) {
				break;
			}

			if (data > pDel->Data)
				pDel = pDel->Right;
			else
				pDel = pDel->Left;
		}

		// ������ ��带 ��ã�� ���
		if (pDel == nullptr) {
			return false;
		}

		pParent = pDel->Parent;
		int iChildCount = pDel->ChildCount();

		if (iChildCount == 0) {
			//PrintLine("0 �ڽ� ���� �õ�");
			// 1. ������ ��尡 �ڽ��� ���� ���
			//		�θ𿡰Լ� �ش� ��带 �����ش�.

			pParent->DeleteChild(pDel);
		} else if (iChildCount == 1) {
			//PrintLine("1 �ڽ� ���� �õ�");
			// 2. ������ ��尡 �ڽ��� �Ѱ��� ���
			//		1. ������ ��尡 �θ���� �����ڽ��� ���
			//			������ ����� �ڽ��� ������ ����� �θ� ������ �ٿ��ش�.
			//		2. ������ ��尡 �θ���� �����ڽ��� ���
			//			������ ����� �ڽ��� ������ ����� �θ� ������ �ٿ��ش�.

			Node* pChild = pDel->Any();

			if (pParent->Right == pDel) {
				pParent->Right = pChild;
			} else {
				pParent->Left = pChild;
			}
			pChild->Parent = pParent;
			DeleteSafe(pDel);
		} else {
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
			//PrintLine("2 �ڽ� ���� �õ�");
			Node* pSmallestParent = pDel;
			Node* pSmallest = pDel->Right;

			while (pSmallest->Left != nullptr) {
				pSmallest = pSmallest->Left;
			}

			pSmallestParent = pSmallest->Parent;
			pDel->Data = Move(pSmallest->Data);

			
			if (pSmallestParent->Right == pSmallest) {
				// ������ ���� ���� ��尡 �޸���� 
				// �� ���� ���� while���� �ѹ��� �ȵ� ����̴�.
				//   -> ������ ��庸�� �� ���� ��尡 �ƿ� ���� ���

				pSmallestParent->Right = pSmallest->Right;
				if (pSmallest->Right)
					pSmallest->Right->Parent = pSmallestParent;
			} else {
				pSmallestParent->Left = pSmallest->Right;
				if (pSmallest->Right)
					pSmallest->Right->Parent = pSmallestParent;
			}
			DeleteSafe(pSmallest);
		}
		m_iSize--;
		return true;
	}

	void ShowDistribution() {
		vector<vector<int>> values;
		values.resize(30);
		int maxDepth = Show(values, m_pVirtualRoot->Any(), 0);
		PrintLine("�� ���� ���� ������ ���");
		for (int i = 0; i < maxDepth; i++) {
			PrintLine("%d�� : %.6f%%%", i + 1, (double)values[i].size() / Math::Pow(2, i) * 100);
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

		while (pCur->Parent != m_pVirtualRoot) {
			pCur = pCur->Parent;
			Balance(pCur);
		}
	}

	// �ش� ��� �������� �뷱�� ����
	void Balance(Node* node) {
		//PrintLine("���� Ȯ��!");
		int iHeightDiff = GetHeightDiff(node);
		//PrintLine("���� : %d", iHeightDiff);
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
	int cnt = 0;
	int GetHeightDiff(Node* node) {
		int iLeftSubtreeHeight = GetHeight(node->Left, 0);
		cnt = 0;
		int iRightSubtreeHeight = GetHeight(node->Right, 0);
		cnt = 0;
		return iRightSubtreeHeight - iLeftSubtreeHeight;
	}

	int GetHeight(Node* cur, int height) {
		if (cur == nullptr) {
			if (cnt != 0) {
				//PrintLine("\t����(��� ȣ�� %dȸ / %d)!", cnt, height);
			}
				
			return height;
		}
		cnt++;
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
	tree.ShowDistribution();
	for (int k = 0; k < MIN_COUNT + 30; k++) {
		if (!tree.Remove(vec[k])) {
			goto FAILED;
		}
	}
	
	vec.clear();

	// ��Ȯ�� �׽�Ʈ
	for (int i = 0; i < ACCURATE_TEST; i++) {
		for (int k = 0; k < ACCURATE_COUNT; k++) {
			vec.push_back(rand.GenerateInt(0, 2));
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