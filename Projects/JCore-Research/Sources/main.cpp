/*
 *
 * 
 *
 */

#include "header.h"

#include <bitset>
#include <map>
#include <stacktrace>
#include <set>

USING_NS_JC;



enum class TreeNodeColor
{
	Red,
	Black
};

const char* TreeNodeColorName(TreeNodeColor color) {
	return color == TreeNodeColor::Red ? "Red" : "Black";
}

struct TreeNode
{
	int Data;
	TreeNodeColor Color;
	TreeNode* Parent;
	TreeNode* Left;
	TreeNode* Right;

	TreeNode(int data)
		: Data(data)
		, Color(TreeNodeColor::Red)
		, Parent(nullptr)
		, Left(nullptr)
		, Right(nullptr)
	{}

	TreeNode* Any() const { return Left ? Left : Right; }
	bool IsLeft() const { return Parent->Left == this; }
	bool IsRight() const { return Parent->Right == this; }
};

class TreeSet
{
public:
	TreeSet()
		: m_pRoot(nullptr)
	{}
	~TreeSet() { Clear(); }

	bool Search(int data) {
		return FindNode(data) != nullptr;
	}

	bool Insert(int data) {
		
		TreeNode* pNewNode;

		// 1. �����͸� ���� �ִ´�.
		if (m_pRoot == nullptr) {
			pNewNode = m_pRoot = dbg_new TreeNode{ data };
		} else {
			// data�� ���Ե� �θ� ��带 ã�´�.
			TreeNode* pParent = FindParentDataInserted(data);

			if (pParent == nullptr) {
				return false;
			}

			pNewNode = dbg_new TreeNode{ data };
			pNewNode->Parent = pParent;

			if (data > pParent->Data) {
				pParent->Right = pNewNode;
			} else {
				pParent->Left = pNewNode;
			}
		}

		// 2. ���Ե� ��带 �������� �����Ʈ���� ���ݵǴ��� Ȯ���Ͽ� �ٷ���´�.
		InsertFixup(pNewNode);
		return true;
	}

	bool Remove(int data) {
		TreeNode* pDelNode = FindNode(data);

		if (pDelNode == nullptr) {
			return false;
		}

		RemoveFixup()
	}

	void Clear() {
		DeleteNodeRecursive(m_pRoot);
		m_pRoot = nullptr;
	}
	
	void PrintHierarchical() {
		HashMap<int, Vector<TreeNode*>> hHierarchy;
		for (int i = 0; i < 200; ++i) {
			hHierarchy.Insert(i, Vector<TreeNode*>{});
		}
		RecordDataOnHierarchy(m_pRoot, 1, hHierarchy);

		for (int i = 1; i < 200; ++i) {
			auto& nodes =  hHierarchy[i];
			if (nodes.Size() <= 0) continue;
			Console::Write("[%d] ", i);
			for (int j = 0; j < nodes.Size(); ++j) {
				Console::Write("%d(%s) ", nodes[j]->Data, TreeNodeColorName(nodes[j]->Color));
			}
			Console::WriteLine("");
		}
		
	}
private:
	TreeNode* FindNode(int data) {
		TreeNode* pCur = m_pRoot;

		while (pCur != nullptr) {
			if (data == pCur->Data) {
				return pCur;
			}

			if (data > pCur->Data) {
				pCur = pCur->Right;
			} else {
				pCur = pCur->Left;
			}
		}

		return nullptr;
	}

	// data�� ���Ե� �θ� ã�´�.
	TreeNode* FindParentDataInserted(int data) {
		TreeNode* pParent = nullptr;
		TreeNode* pCur = m_pRoot;

		while (pCur != nullptr) {
			pParent = pCur;

			if (data > pCur->Data) {
				pCur = pCur->Right;
			} else {
				pCur = pCur->Left;
			}
		}

		return pParent;
	}

	void InsertFixup(TreeNode* child) {

		// (1) ��Ʈ ���� Black�̴�.
		if (child == m_pRoot) {
			child->Color = TreeNodeColor::Black;
			return;
		}

		TreeNode* pParent = child->Parent;		// (1)���� ������� �ʾҴٸ� ������ �θ� ������.
		TreeNodeColor eParentColor = pParent->Color;

		/*  (2) Red ����� �ڽ��� Black�̾���Ѵ�.
		 *  ���� �ڽİ� �θ� ������ ��� �������� �ƴ� ��� ���̻� �˻��� �ʿ䰡 ����.
		 *  ������� ���� ���, �� pParent�� ��Ʈ ����� ���
		 *  ��Ʈ ���� ������ Black�̰� ���� ���Ե� ���� Red�̹Ƿ� Ʈ�� ���̰� 2�϶��� �׻� RBƮ���� ��� ���ǿ� �����Ѵ�.
		 *   => ���� InsertFixup ����� �ƹ��͵� �Ұ� ����.
		*
		 *     5    root = parent (black)          5         root = parent (black)
		 *   1	 ?	child (red)                  ?   10		 child (red)
		 *  
		 */
		if (eParentColor != TreeNodeColor::Red || child->Color != TreeNodeColor::Red) {
			return;
		}

		// ��� ����(Ʈ�� ����)�� 2�� ���� ��� �� IF������ �ɷ����Ƿ� ���ķ� GrandParent�� nullptr�� �� ����.
		TreeNode* pGrandParent = pParent->Parent;		
		TreeNode* pUncle = nullptr;						// ���� ������� (�θ� ������� �����ڽ��� ��� ������� ������ �ڽ��� ���� ���)
		if (pGrandParent != nullptr) {
			if (pGrandParent->Left == pParent)
				pUncle = pGrandParent->Right;
			else
				pUncle = pGrandParent->Left;
		}
		DebugAssertMsg(pGrandParent, "�׷��� �θ� NULL�Դϴ�.");
		const TreeNodeColor eUncleColor = pUncle ? pUncle->Color : TreeNodeColor::Black; // ���� ���� ���� ���� ���� ���� �ְ�. NIL ���� Black�̴�.


		/*
		 * Case 1: ���� ��尡 Black�� ���
		 *			Case 1-1
		 *			----------------------------------------------
		 *			       10(B)				<- grandparent
		 *			    5(R)	 ?(B)			<- parent, uncle
		 *			  1(R) ?					<- child
		 *
		 *			Case 1-2
		 *			----------------------------------------------
		 *			       10(B)				<- grandparent
		 *		       ?(B)   15(R)				<- uncle, parent 
		 *                       21(R)			<- child
		 *
		 *
		 *		    Case 1-3 (�ﰢ�� ���) - 5�� RRȸ���Ͽ� Case 1-1�� ������� ��ȯ������Ѵ�.
		 *			----------------------------------------------
		 *			       10(B)				<- grandparent
		 *			    5(R)	 ?(B)			<- parent, uncle
		 * 				   7(R) 				<- child
		 *				              �� ��ȯ �� 
		 *			       10(B)				<- grandparent
		 *			     7(R)	 ?(B)			<- child, uncle	==> 
		 * 			  5(R) ?					<- parent
		 *
		 *		    Case 1-4 (�ﰢ�� ���) - 5�� RRȸ���Ͽ� Case 1-1�� ������� ��ȯ������Ѵ�.
		 *			----------------------------------------------
		 *			       10(B)				<- grandparent
		 *			    ?(B)	 15(R)			<- parent, uncle
		 * 				      12(R) 			<- child
		 *				              �� ��ȯ �� 
		 *			       10(B)				<- grandparent
		 *			    ?(B)	12(R)			<- child, uncle
		 * 				            10(R) 		<- parent
		 *
		 *
		 */

		// Case 1 
		if (eUncleColor == TreeNodeColor::Black) {
			if (pParent->IsLeft()) {
				if (child->IsLeft()) {
					// Case 1-1
					pGrandParent->Color = TreeNodeColor::Red;
					pParent->Color = TreeNodeColor::Black;
					RotateLL(pGrandParent);

					// ������ ��Ʈ��忴�ٸ� ȸ�� �� �θ� ��Ʈ���� �ö���Ƿ� �����������
					if (m_pRoot == pGrandParent) {
						m_pRoot = pParent;
					}

				} else {
					// Case 1-3
					RotateRR(pParent);
					InsertFixup(pParent);
				}
			} else {
				if (child->IsRight()) {
					// Case 1-2
					pGrandParent->Color = TreeNodeColor::Red;
					pParent->Color = TreeNodeColor::Black;
					RotateRR(pGrandParent);

					// ������ ��Ʈ��忴�ٸ� ȸ�� �� �θ� ��Ʈ���� �ö���Ƿ� �����������
					if (m_pRoot == pGrandParent) {
						m_pRoot = pParent;
					}
				} else {
					// Case 1-4
					RotateLL(pParent);
					InsertFixup(pParent);
				}
			}
			return;
		} 


		/*
		 * Case 2: ���� ��尡 Red�� ���
		 *     �̰�� Case1���� �ξ� �ܼ��ϴ�. �θ�, ������ ����� ������� ������ �ٲ������ν�
		 *	   RBƮ�� �Ӽ� 4���� ������� �ʵ��� �����.
		 *	   �׸��� ������� Red�� �Ǿ��� ������ ������� �θ� ���������� Red�� ���� �����Ƿ�
		 *	   ������� �������� �ٽ� Fixup�� �������ָ� �ȴ�.
		 *
		 *			Case 1-1
		 *			----------------------------------------------
		 *			       10(B)				<- grandparent
		 *			    5(R)	 15(R)			<- parent, uncle
		 *			 1(R) 						<- child
		 *
		 *			Case 1-2
		 *			----------------------------------------------
		 *			       10(B)				<- grandparent
		 *		        5(R)   15(R)			<- uncle, parent
		 *                        21(R)			<- child
		 *
		*		    Case 1-3 (�ﰢ�� ���)
		 *			----------------------------------------------
		 *			       10(B)				<- grandparent
		 *			    5(R)	 15(R)			<- parent, uncle
		 * 				   7(R) 				<- child
		 *
		 *		    Case 1-4 (�ﰢ�� ���)
		 *			----------------------------------------------
		 *			       10(B)				<- grandparent
		 *			    5(R)	 15(R)			<- parent, uncle
		 * 				      12(R) 			<- child
		 *
		 * @����: Uncle�� Red�� �����Ǿ��ٴ� ���� nullptr�� �ƴϱ⵵�ϴ�.
		 */


		
		pUncle->Color = TreeNodeColor::Black;	
		pParent->Color = TreeNodeColor::Black;
		pGrandParent->Color = TreeNodeColor::Red;
		InsertFixup(pGrandParent);
	}

	void RemoveFixup(TreeNode* child) {
		
	}

	// ��尡 ����/�������� ���� ���
	static void RotateLL(TreeNode* node) {
		//        ?		- pParent
		//      5		- pCur
		//    3			- pChild
		//  1   ?		- pChildRight

		//      ?		- pParent
		//    3			- pChild
		//  1   5		- pCur
		//    ?			- pChildRight

		TreeNode* pParent = node->Parent;
		TreeNode* pCur = node;
		TreeNode* pChild = node->Left;
		TreeNode* pChildRight = node->Left->Right;

		if (pParent) {
			if (pParent->Left == pCur)
				pParent->Left = pChild;
			else
				pParent->Right = pChild;
		}
		pChild->Parent = pParent;

		pCur->Left = pChildRight;
		if (pChildRight)
			pChildRight->Parent = pCur;

		pChild->Right = pCur;
		pCur->Parent = pChild;
	}

	// ��尡 ������/���������� ���� ���
	static void RotateRR(TreeNode* node) {
		//  ?   		- ? : pParent
		//    1 		- 1 : pCur
		//      3		- 3 : pChild
		//    ?	  5  	- ? : pChildLeft
		// 
		//         �� ��ȯ ��
		//  ?			- ? : pParent
		//    3			- 3 : pChild
		//  1   5		- 1 : pCur
		//   ?			- ? : pChildLeft

		TreeNode* pParent = node->Parent;
		TreeNode* pCur = node;
		TreeNode* pChild = node->Right;
		TreeNode* pChildLeft = node->Right->Left;

		if (pParent) {
			if (pParent->Left == pCur)
				pParent->Left = pChild;
			else
				pParent->Right = pChild;
		}
		pChild->Parent = pParent;


		pCur->Right = pChildLeft;
		if (pChildLeft)
			pChildLeft->Parent = pCur;

		pChild->Left = pCur;
		pCur->Parent = pChild;
	}

	static void RecordDataOnHierarchy(TreeNode* node, int depth, HashMap<int, Vector<TreeNode*>>& hierarchy) {
		if (node == nullptr) return;
		hierarchy[depth].PushBack(node);
		RecordDataOnHierarchy(node->Left, depth + 1, hierarchy);
		RecordDataOnHierarchy(node->Right, depth + 1, hierarchy);
	}

	static void DeleteNodeRecursive(TreeNode* node) {
		if (node == nullptr) return;
		DeleteNodeRecursive(node->Left);
		DeleteNodeRecursive(node->Right);
		delete node;
	}

	TreeNode* m_pRoot;
};


int main() {
	Console::SetSize(600, 800);
	TreeSet set;
	Vector<int> nodes;
	Random::EngineInitialize();
	int count = 0;
	for (;;) {
		int a = Random::GenerateInt(0, 100);
		if (!nodes.Exist(a))
			nodes.PushBack(a);

		if (nodes.Size() == 16)
			break;
	}

	nodes.ForEach([](auto s) { Console::Write("%d ", s); });
	Console::WriteLine("==============================\n ");

	for (int i = 0; i < nodes.Size(); ++i) {
		set.Insert(nodes[i]);
		set.PrintHierarchical();
		Console::WriteLine("=============================");
	}
	return 0;
}
