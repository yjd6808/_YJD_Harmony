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

		// 1. 데이터를 먼저 넣는다.
		if (m_pRoot == nullptr) {
			pNewNode = m_pRoot = dbg_new TreeNode{ data };
		} else {
			// data가 삽입될 부모 노드를 찾는다.
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

		// 2. 삽입된 노드를 기준으로 레드블랙트리가 위반되는지 확인하여 바로잡는다.
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

	// data가 삽입될 부모를 찾는다.
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

		// (1) 루트 노드는 Black이다.
		if (child == m_pRoot) {
			child->Color = TreeNodeColor::Black;
			return;
		}

		TreeNode* pParent = child->Parent;		// (1)에서 종료되지 않았다면 무조건 부모가 존재함.
		TreeNodeColor eParentColor = pParent->Color;

		/*  (2) Red 노드의 자식은 Black이어야한다.
		 *  만약 자식과 부모가 색상이 모두 빨간색이 아닌 경우 더이상 검사할 필요가 없다.
		 *  조상님이 없는 경우, 즉 pParent가 루트 노드인 경우
		 *  루트 노드는 무조건 Black이고 새로 삽입된 노드는 Red이므로 트리 높이가 2일때는 항상 RB트리의 모든 조건에 만족한다.
		 *   => 따라서 InsertFixup 수행시 아무것도 할게 없다.
		*
		 *     5    root = parent (black)          5         root = parent (black)
		 *   1	 ?	child (red)                  ?   10		 child (red)
		 *  
		 */
		if (eParentColor != TreeNodeColor::Red || child->Color != TreeNodeColor::Red) {
			return;
		}

		// 노드 깊이(트리 높이)가 2인 경우는 모두 위 IF문에서 걸러지므로 이후로 GrandParent가 nullptr일 수 없다.
		TreeNode* pGrandParent = pParent->Parent;		
		TreeNode* pUncle = nullptr;						// 삼촌 노드정보 (부모가 조상님의 왼쪽자식인 경우 조상님의 오른쪽 자식이 삼촌 노드)
		if (pGrandParent != nullptr) {
			if (pGrandParent->Left == pParent)
				pUncle = pGrandParent->Right;
			else
				pUncle = pGrandParent->Left;
		}
		DebugAssertMsg(pGrandParent, "그랜드 부모가 NULL입니다.");
		const TreeNodeColor eUncleColor = pUncle ? pUncle->Color : TreeNodeColor::Black; // 삼촌 노드는 있을 수도 없을 수도 있고. NIL 노드는 Black이다.


		/*
		 * Case 1: 삼촌 노드가 Black일 경우
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
		 *		    Case 1-3 (삼각형 모양) - 5를 RR회전하여 Case 1-1의 모양으로 변환해줘야한다.
		 *			----------------------------------------------
		 *			       10(B)				<- grandparent
		 *			    5(R)	 ?(B)			<- parent, uncle
		 * 				   7(R) 				<- child
		 *				              ↓ 변환 후 
		 *			       10(B)				<- grandparent
		 *			     7(R)	 ?(B)			<- child, uncle	==> 
		 * 			  5(R) ?					<- parent
		 *
		 *		    Case 1-4 (삼각형 모양) - 5를 RR회전하여 Case 1-1의 모양으로 변환해줘야한다.
		 *			----------------------------------------------
		 *			       10(B)				<- grandparent
		 *			    ?(B)	 15(R)			<- parent, uncle
		 * 				      12(R) 			<- child
		 *				              ↓ 변환 후 
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

					// 조상이 루트노드였다면 회전 후 부모가 루트노드로 올라오므로 변경해줘야함
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

					// 조상이 루트노드였다면 회전 후 부모가 루트노드로 올라오므로 변경해줘야함
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
		 * Case 2: 삼촌 노드가 Red일 경우
		 *     이경우 Case1보다 훨씬 단순하다. 부모, 삼촌의 색상과 조상님의 색상을 바꿔줌으로써
		 *	   RB트리 속성 4번이 위배되지 않도록 만든다.
		 *	   그리고 조상님이 Red가 되었기 때문에 조상님의 부모가 마찬가지로 Red일 수가 있으므로
		 *	   조상님을 기준으로 다시 Fixup을 수행해주면 된다.
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
		*		    Case 1-3 (삼각형 모양)
		 *			----------------------------------------------
		 *			       10(B)				<- grandparent
		 *			    5(R)	 15(R)			<- parent, uncle
		 * 				   7(R) 				<- child
		 *
		 *		    Case 1-4 (삼각형 모양)
		 *			----------------------------------------------
		 *			       10(B)				<- grandparent
		 *			    5(R)	 15(R)			<- parent, uncle
		 * 				      12(R) 			<- child
		 *
		 * @참고: Uncle이 Red로 판정되었다는 말은 nullptr이 아니기도하다.
		 */


		
		pUncle->Color = TreeNodeColor::Black;	
		pParent->Color = TreeNodeColor::Black;
		pGrandParent->Color = TreeNodeColor::Red;
		InsertFixup(pGrandParent);
	}

	void RemoveFixup(TreeNode* child) {
		
	}

	// 노드가 왼쪽/왼쪽으로 붙은 경우
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

	// 노드가 오른쪽/오른쪽으로 붙은 경우
	static void RotateRR(TreeNode* node) {
		//  ?   		- ? : pParent
		//    1 		- 1 : pCur
		//      3		- 3 : pChild
		//    ?	  5  	- ? : pChildLeft
		// 
		//         ↓ 변환 후
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
