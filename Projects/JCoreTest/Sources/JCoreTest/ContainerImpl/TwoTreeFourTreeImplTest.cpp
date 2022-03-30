/*
	�ۼ��� : ������
	���� Ž�� Ʈ�� �׽�Ʈ ����
*/

#include <JCoreTest/CoreTest.h>
#include <JCore/String.h>

using namespace std;
using namespace JCore;

#if TEST_TwoThreeFourTreeImplTest == ON

constexpr int g_kiMaxDataCount = 3;
constexpr int g_kiMaxChildCount = 4;

struct Node
{
	bool IsEmpty[g_kiMaxDataCount] = { true, true, true};
	int Datas[g_kiMaxDataCount] = { 0, 0, 0};
	Node* Children[g_kiMaxChildCount] = { nullptr, nullptr, nullptr, nullptr};

	Node() {}
	Node(int d1) {
		Fill(0, d1);
	}

	Node* Any() {
		for (int i = 0; i < g_kiMaxChildCount; i++) {
			if (Children[i] != nullptr) {
				return Children[i];
			}
		}
		return nullptr;
	}

	void Fill(const int slot, int data) {
		if (!IsEmpty[slot]) {
			assert("slot is not empty");
		}

		Datas[slot] = data;
		IsEmpty[slot] = false;
	}

	void Insert(int data) {
		for (int i = 0; i < g_kiMaxDataCount; i++) {
			if (IsEmpty[i]) {
				Fill(i, data);
				Sort();
				return;
			}
		}
	}

	int DataCount() {
		int iCount = 0;
		for (int i = 0; i < g_kiMaxDataCount; i++) {
			if (!IsEmpty[i]) {
				iCount++;
			}
		}

		return iCount;
	}

	void Sort() {
		int iDataCount = DataCount();

		for (int i = 0; i < iDataCount; i++) {
			for (int j = i + 1; j < iDataCount; j++) {
				if (Datas[i] > Datas[j]) {
					int iTemp = Datas[i];
					Datas[i] = Datas[j];
					Datas[j] = iTemp;
				}
			}
		}
	}

	
	bool IsFull() {
		for (int i = 0; i < g_kiMaxDataCount; i++) {
			if (IsEmpty[i]) {
				return false;
			}
		}
		return true;
	}

	bool IsTerminal() {
		for (int i = 0; i < g_kiMaxChildCount; i++) {
			if (Children[i] != nullptr) {
				return false;
			}
		}

		return true;
	}


	// data�� � �ڽ� ���� ã�ư������� Ű���� �������� �Ǵ��ؼ� �ش� ���� �̵���
	// ���� �ش� ��尡 ���� ��� �����ؼ� ��ȯ����.
	Node* NextChild(int data) {
		int iNextChildNodeIdx = -1;
		int iDataCount = DataCount();

		if (iDataCount == 0) {
			assert("�����Ͱ� ���� ��� ���� ��带 ��������");
		}

		for (int i = 0; i < iDataCount; i++) {
			if (data <= Datas[i] && !IsEmpty[i]) {
				iNextChildNodeIdx = i;
			}
		}

		// 0, 1, 2�� �ڽ��߿� ���� ��ã�� ��쿡�� �� ������ ���� ������ ��
		if (iNextChildNodeIdx == -1) {
			iNextChildNodeIdx = iDataCount - 1;
		}
		
		if (Children[iDataCount - 1] == nullptr) {
			Children[iDataCount - 1] = new Node;
		}

		return Children[iDataCount - 1];
	}
};

class TwoThreeFourTree
{
public:
	TwoThreeFourTree() {
		m_pRoot = nullptr;
		m_iSize = 0;
	}
	~TwoThreeFourTree() {

	}
public:
	void Add(int data) {
		if (m_pRoot == nullptr) {
			m_pRoot = new Node(data);
			m_iSize++;
			return;
		}

		Add(nullptr, m_pRoot, data);
		m_iSize++;
	}

	vector<vector<int>> Nodes() {
		vector<vector<int>> nodes;
		nodes.resize(10);

		Nodes(nodes, m_pRoot, 0);
		return nodes;
	}
private:
	void Add(Node* parent, Node* cur, int data) {
		if (cur == nullptr) {
			assert("�ڽ��� nullptr�� �� �����ϴ�.");
			return;
		}

		// �ܸ� ����̸� �����͸� �־���
		if (cur->IsTerminal()) {

			// ���� ��带 �°����� Promote �ϸ鼭 �����͸� �־�����.
			if (cur->IsFull()) {
				PromoteNode(parent, cur, data);
			} else {
				cur->Insert(data);
			}
			return;
		}

		// �ܸ� ��尡 �ƴ� ��� cur ��忡�� ��� ������ ������� Ȯ���ؼ� �ش� ��ġ�� ��带 �����Ѵ�.
		Add(cur, cur->NextChild(data), data);
	}

	void PromoteNode(Node* parent, Node* cur, int data) {
		int arrData[g_kiMaxChildCount];
		FillPromoteWaitignArray(cur, arrData, data);

		// ���� ����� �𸣰ڴ� ����;
	}

	void FillPromoteWaitignArray(Node* cur, int(&arr)[g_kiMaxChildCount], int data) {
		int iPosition = g_kiMaxDataCount;

		for (int i = 0; i < g_kiMaxDataCount; i++) {
			if (data <= cur->Datas[i]) {
				iPosition = i;
				break;
			}
		}

		arr[iPosition] = data;

		for (int i = 0, j = 0; i < g_kiMaxChildCount; i++) {
			if (i == iPosition) {
				continue;
			}
			arr[i] = cur->Datas[j];
			j++;
		}
	}

	void Nodes(vector<vector<int>>& nodes, Node* cur, int depth) {
		if (cur == nullptr) {
			return;
		}

		for (int i = 0; i < g_kiMaxDataCount; i++) {
			if (!cur->IsEmpty[i])
				nodes[depth].push_back(cur->Datas[i]);
		}

		for (int i = 0; i < g_kiMaxChildCount; i++)
			Nodes(nodes, cur->Children[i], depth + 1);
	}

private:

	Node* m_pRoot;
	int m_iSize;
};


TEST(TwoThreeFourTreeImplTest, TwoThreeFourTreeImplTest) {
	TwoThreeFourTree tree;
	tree.Add('C');
	tree.Add('F');
	tree.Add('G');
	tree.Add('D');
	auto z = tree.Nodes(); 

}

#endif // TEST_TwoThreeFourTreeImplTest == ON