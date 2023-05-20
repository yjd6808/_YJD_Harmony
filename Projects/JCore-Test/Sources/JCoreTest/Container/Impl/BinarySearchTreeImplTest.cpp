/*
	작성자 : 윤정도
	이진 탐색 트리 테스트 구현
*/

#include <JCoreTest/CoreTest.h>
#include <JCore/Primitives/String.h>

using namespace std;


#if TEST_BinarySearchTreeImplTest == ON

namespace BST {

struct Node
{
	int Data;
	Node* Left;
	Node* Right;
};


// 이진 탐색 트리를 구현해보자.
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
	// 삽입시 고려해야할 사항
	// 1. 우측에 비교 결과가 큰 값
	// 2. 좌측에 비교 결과가 작은 값
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

	bool Exist(int data) {
		// 자식이 없는 경우라도 상관없음 m_pVirtualRoot의 자식이 nullptr일테니까
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


	// 삭제 구현시 고려해야할 사항
	// 가상의 루트노드가 있으면 좋다.
	// [고려사항 1]
	// 1. 삭제할 노드가 루트 노드인 경우
	// 2. 삭제할 노드가 루트 노드가 아닌 경우
	//		-> 가상의 루트노드로 해결

	// [고려사항 2]
	// 1. 삭제할 노드가 단말 노드인 경우
	//		-> 바로 삭제한다. (부모에서 해당 단말 노드를 골라서 삭제해주도록 하면된다.)
	// 2. 삭제할 노드의 자식이 하나만 있는 경우
	//		-> 부모의 오른쪽에 삭제할 노드가 달린 경우
	//			-> 부모의 오른쪽 노드를 삭제한 후 삭제할 노드의 자식을 붙여준다.
	//		-> 부모의 왼쪽에 삭제할 노드가 달린 경우
	//			-> 부모의 왼쪽 노드를 삭제한 후 삭제할 노드의 자식을 붙여준다.
	// 3. 삭제할 노드의 자식이 2개가 다 있는 경우
	//		2개다 달린 경우 삭제할 노드의 위치를 대체할 다른 녀석이 와야한다.
	//		이 때 선택하는 경우가 2가지가 있다.
	//		1. 삭제할 노드의 우측 자식노드기준 서브트리에서 가장 작은 녀석을 찾는다. // 이걸로 하자.
	//			-> 찾아낸 노드의 값을 삭제할 노드에 대입해준다.
	//			-> 찾아낸 노드의 부모에서 찾아낸 노드를 삭제한다.
	//			-> 찾아낸 노드의 부모와 찾아낸 노드의 우측 자식을 연결해준다.
	//				1. 찾아낸 노드의 부모의 좌측에 찾아낸 노드가 연결된 경우 
	//		2. 삭제할 노드의 좌측 자식노드기준 서브트리에서 가장 큰 녀석을 찾는다.
	//			-> 찾아낸 노드의 값을 삭제할 노드에 대입해준다.
	//			-> 찾아낸 노드의 부모에서 찾아낸 노드를 삭제한다.
	//			-> 찾아낸 노드의 부모와 찾아낸 노드의 좌측 자식을 연결해준다.
	

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
			DeleteSafeChild(pParent, pCur);
		} else if (iChildCount == 1) {
			Node* pCurChild = GetChildIfExist(pCur);
			if (pParent->Right == pCur) {
				DeleteSafeChild(pParent, pCur);
				pParent->Right = pCurChild;
			} else {
				DeleteSafeChild(pParent, pCur);
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
			JCORE_DELETE_SAFE(pSmallest);
		}
		m_iSize--;
		return true;
	}

	void PrintTree() {
		cout << "===================\n";

		if (m_iSize == 0) {
			cout << "비어있음\n";
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
	void DeleteSafeChild(Node* parent, Node* child) {
		if (parent == nullptr || child == nullptr) {
			throw std::runtime_error("parent == nullptr || child == nullptr");
		}

		if (parent->Left == child) {
			JCORE_DELETE_SAFE(parent->Left);
		} else {
			JCORE_DELETE_SAFE(parent->Right);
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

	const int ACCURATE_TEST = 100;
	const int ACCURATE_COUNT = 100;

	Random rand;
	std::vector<int> vec;


	// 정확성 테스트
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
	
	// 많은 수의 원소 테스트
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

	// 적은 수의 원소 테스트
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