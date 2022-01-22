/*
	�ۼ��� : ������
	���� Ž�� Ʈ�� �׽�Ʈ ����
*/

#include <JCoreTest/CoreTest.h>
#include <JCore/Core.h>
#include <JCore/String/StringUtil.h>
#include <JCore/String/String.h>
#include <JCore/Tuple.h>
#include <JCore/TypeTraits.h>
#include <JCore/Random.h>

using namespace std;
using namespace JCore;

#if TEST_BinarySearchTreeImplTest == ON

namespace BST {

struct Node
{
	int Data;
	Node* Left;
	Node* Right;
};


// ���� Ž�� Ʈ���� �����غ���.
class BinarySearchTree
{
public:
	BinarySearchTree(int (*comp)(int, int)) {
		m_iSize = 0;
		m_fnComp = comp;
		m_pVirtualRoot = new Node{0};
	}
	~BinarySearchTree() {
		Clear();
	}
private:


public:
	// ���Խ� ����ؾ��� ����
	// 1. ������ �� ����� ū ��
	// 2. ������ �� ����� ���� ��
	void Add(int data) {
		Node* pParent = m_pVirtualRoot;
		Node* pCur = GetChildIfExist(m_pVirtualRoot);
		Node* pNew = new Node{ data };

		int iComp = 0;

		while (pCur != nullptr) {
			iComp = m_fnComp(pCur->Data, pNew->Data);
			pParent = pCur;

			if (iComp > 0) {
				pCur = pCur->Right;
				continue;
			}
			pCur = pCur->Left;
		}

		if (iComp > 0) 
			pParent->Right = pNew;
		else 
			pParent->Left = pNew;
			
		m_iSize++;
	}

	bool Find(int data) {
		// �ڽ��� ���� ���� ������� m_pVirtualRoot�� �ڽ��� nullptr���״ϱ�
		Node* pCur = GetChildIfExist(m_pVirtualRoot);

		while (pCur != nullptr) {
			if (pCur->Data == data) {
				return true;
			}

			if (m_fnComp(pCur->Data, data) > 0) {
				pCur = pCur->Right;
				continue;
			}

			pCur = pCur->Left;
		}

		return false;
	}


	// ���� ������ ����ؾ��� ����
	// ������ ��Ʈ��尡 ������ ����.
	// [������� 1]
	// 1. ������ ��尡 ��Ʈ ����� ���
	// 2. ������ ��尡 ��Ʈ ��尡 �ƴ� ���
	//		-> ������ ��Ʈ���� �ذ�

	// [������� 2]
	// 1. ������ ��尡 �ܸ� ����� ���
	//		-> �ٷ� �����Ѵ�. (�θ𿡼� �ش� �ܸ� ��带 ��� �������ֵ��� �ϸ�ȴ�.)
	// 2. ������ ����� �ڽ��� �ϳ��� �ִ� ���
	//		-> �θ��� �����ʿ� ������ ��尡 �޸� ���
	//			-> �θ��� ������ ��带 ������ �� ������ ����� �ڽ��� �ٿ��ش�.
	//		-> �θ��� ���ʿ� ������ ��尡 �޸� ���
	//			-> �θ��� ���� ��带 ������ �� ������ ����� �ڽ��� �ٿ��ش�.
	// 3. ������ ����� �ڽ��� 2���� �� �ִ� ���
	//		2���� �޸� ��� ������ ����� ��ġ�� ��ü�� �ٸ� �༮�� �;��Ѵ�.
	//		�� �� �����ϴ� ��찡 2������ �ִ�.
	//		1. ������ ����� ���� �ڽĳ����� ����Ʈ������ ���� ���� �༮�� ã�´�. // �̰ɷ� ����.
	//			-> ã�Ƴ� ����� ���� ������ ��忡 �������ش�.
	//			-> ã�Ƴ� ����� �θ𿡼� ã�Ƴ� ��带 �����Ѵ�.
	//			-> ã�Ƴ� ����� �θ�� ã�Ƴ� ����� ���� �ڽ��� �������ش�.
	//				1. ã�Ƴ� ����� �θ��� ������ ã�Ƴ� ��尡 ����� ��� 
	//		2. ������ ����� ���� �ڽĳ����� ����Ʈ������ ���� ū �༮�� ã�´�.
	//			-> ã�Ƴ� ����� ���� ������ ��忡 �������ش�.
	//			-> ã�Ƴ� ����� �θ𿡼� ã�Ƴ� ��带 �����Ѵ�.
	//			-> ã�Ƴ� ����� �θ�� ã�Ƴ� ����� ���� �ڽ��� �������ش�.
	

	bool Remove(int data) {
		if (m_iSize == 0) {
			return false;
		}

		Node* pParent = m_pVirtualRoot;
		Node* pCur = GetChildIfExist(m_pVirtualRoot);

		while (pCur != nullptr) {
			if (pCur->Data == data) {
				break;
			}

			pParent = pCur;

			if (m_fnComp(pCur->Data, data) > 0) {
				pCur = pCur->Right;
				continue;
			}

			pCur = pCur->Left;
		}

		if (pCur == nullptr) {
			return false;
		}

		int iChildCount = GetChildCount(pCur);
		if (iChildCount == 0) {
			SafeDeleteChild(pParent, pCur);
		} else if (iChildCount == 1) {
			Node* pCurChild = GetChildIfExist(pCur);
			if (pParent->Right == pCur) {
				SafeDeleteChild(pParent, pCur);
				pParent->Right = pCurChild;
			} else {
				SafeDeleteChild(pParent, pCur);
				pParent->Left = pCurChild;
			}
		} else {
			Node* pSmallestParent = pCur;
			Node* pSmallest = pCur->Right;

			while (pSmallest->Left != nullptr) {
				pSmallestParent = pSmallest;
				pSmallest = pSmallest->Left;
			}

			pCur->Data = Move(pSmallest->Data);

			if (pSmallestParent->Left == pSmallest) {
				pSmallestParent->Left = pSmallest->Right;
			} else {
				pSmallestParent->Right = pSmallest->Right;
			}
			safe_delete(pSmallest);
		}
		m_iSize--;
		return true;
	}

	void PrintTree() {
		cout << "===================\n";

		if (m_iSize == 0) {
			cout << "�������\n";
			return;
		} 
		PrintTree(GetChildIfExist(m_pVirtualRoot));
	}

	int Count() {
		int c = 0;
		Count(GetChildIfExist(m_pVirtualRoot), c);
		return c;
	}
private:
	void SafeDeleteChild(Node* parent, Node* child) {
		if (parent == nullptr || child == nullptr) {
			throw std::runtime_error("parent == nullptr || child == nullptr");
		}

		if (parent->Left == child) {
			safe_delete(parent->Left);
		} else {
			safe_delete(parent->Right);
		}
	}

	Node* GetChildIfExist(Node* node) {
		if (node->Left) {
			return node->Left;
		}

		if (node->Right) {
			return node->Right;
		}

		return nullptr;
	}

	const int GetChildCount(Node* node) {
		int count = 0;

		if (node->Left)
			count++;
		if (node->Right)
			count++;
		return count;
	}

	void PrintTree(Node* node) {
		if (node == nullptr) {
			return;
		}
		PrintTree(node->Left);
		std::cout << node->Data << "\n";
		PrintTree(node->Right);
	}

	void Count(Node* node, int& cnt) {
		if (node == nullptr) {
			return;
		}
		Count(node->Left, cnt);
		cnt++;
		Count(node->Right, cnt);
	}

	void Clear() {

	}
private:
	Node* m_pVirtualRoot;
	int (*m_fnComp)(int, int);
	int m_iSize;
};

} // namespace BST

int comp(int lhs, int rhs) {
	if (lhs < rhs) {
		return 1;
	} else if (lhs > rhs) {
		return -1;
	}
	return 0;
}

TEST(BinarySearchTreeImplTest, BinarySearchTreeImplTest) {
	BST::BinarySearchTree tree(comp);

	const int MANY_TEST = 10;
	const int MANY_COUNT = 1000;
	

	const int MIN_TEST = 1000;
	const int MIN_COUNT = 10;

	const int ACCURATE_TEST = 1000;
	const int ACCURATE_COUNT = 1000;

	Random rand;
	std::vector<int> vec;


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

#endif // TEST_BinarySearchTreeImplTest == ON