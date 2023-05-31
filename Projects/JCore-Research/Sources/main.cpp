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

	TreeNode(int data, TreeNodeColor color = TreeNodeColor::Red)
		: Data(data)
		, Color(color)
		, Parent(nullptr)
		, Left(nullptr)
		, Right(nullptr)
	{}

	// 둘중 할당된 자식 아무거나 반환
	TreeNode* Any() const { return Left ? Left : Right; }

	// 둥중 하나의 자식 아무거나 반환 및 자식이 몇개있는지도 같이 반환
	TreeNode* AnyWithChildrenCount(JCORE_OUT int& count) const {
		if (Left && Right) {
			count = 2;
			return Left;
		}
		if (Left) {
			count = 1;
			return Left;
		}
		if (Right) {
			count = 1;
			return Right;
		}
		count = 0;
		return nullptr;
	}
	bool IsLeft() const { return Parent->Left == this; }
	bool IsRight() const { return Parent->Right == this; }
	int Count() const {
		if (Left && Right) return 2;
		if (Left) return 1;
		if (Right) return 1;
		return 0;
	}

	static void ConnectLeft(TreeNode* parent, TreeNode* child) {
		DebugAssertMsg(parent->Left == nullptr, "부모(%d)의 좌측자식(%d)가 이미할당되어있음. %d 자식 연결불가능", parent->Data, parent->Left->Data, child->Data);
		DebugAssert(child->Parent == nullptr, "자식(%d)의 부모(%d)가 이미할당되어있음. %d 부모 연결불가능", child->Data, child->Parent->Data, parent->Data);
		parent->Left = child;
		child->Parent = parent;
	}
	static void ConnectRight(TreeNode* parent, TreeNode* child) {
		DebugAssertMsg(parent->Right == nullptr, "부모(%d)의 우측자식(%d)가 이미할당되어있음. %d 자식 연결불가능", parent->Data, parent->Right->Data, child->Data);
		DebugAssert(child->Parent == nullptr, "자식(%d)의 부모(%d)가 이미할당되어있음. %d 부모 연결불가능", child->Data, child->Parent->Data, parent->Data);
		parent->Right = child;
		child->Parent = parent;
	}
};

struct TreeNodeFamily
{
	/* Not Null */ TreeNode* Parent;
	/* Not Null */ TreeNode* Sibling;
	/* Nullable */ TreeNode* NephewLine;
	/* Nullable */ TreeNode* NephewTri;

	TreeNodeFamily(TreeNode* child) {
		const bool bRightChild = child->IsRight();
		Parent = child->Parent;									// 부모 노드
		DebugAssertMsg(Parent, "부모노드 없을 수 없습니다.");

		Sibling = bRightChild ? Parent->Left : Parent->Right;	// 형제 노드 (child가 우측이면 부모의 왼쪽 노드가 형제 노드)
		DebugAssertMsg(Sibling, "형제노드가 없을 수 없습니다.");

		const TreeNode* pNephewLine = nullptr;					// 조카 노드 (일렬로 나열)		
		const TreeNode* pNephewTri = nullptr;					// 조카 노드 (꺽여서 나열)

		if (Sibling->IsLeft()) {
			NephewLine = Sibling->Left;
			NephewTri = Sibling->Right;
		} else {
			NephewLine = Sibling->Right;
			NephewTri = Sibling->Left;
		}
	}
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

	void DeleteNode(TreeNode* node) {
		if (node == m_pRoot) {
			JCORE_DELETE_SAFE(m_pRoot);
			return;
		}

		if (node->Parent) {
			if (node->Parent->Left == node)
				node->Parent->Left = nullptr;
			else if (node->Parent->Right == node)	// 부유 상태의 node일 수 있으므로 무조건 체크
				node->Parent->Right = nullptr;
		} 

		delete node;
	}

	void ConnectPredecessorChildToParent(TreeNode* predecessor, TreeNode* predecessorLeftChild) {

		if (predecessor->IsRight()) {
			predecessor->Parent->Right = predecessorLeftChild;
			predecessorLeftChild->Parent = predecessor->Parent;
			return;
		}

		predecessor->Parent->Left = predecessorLeftChild;
		predecessorLeftChild->Parent = predecessor->Parent;
	}

	bool Remove(int data) {
		TreeNode* pDelNode = FindNode(data);

		if (pDelNode == nullptr) {
			return false;
		}

		// 자식이 없는 경우 그냥 바로 제거 진행
		int iCount = 0;
		TreeNode* pChild = pDelNode->AnyWithChildrenCount(iCount);

		if (iCount == 2) {
			// 자식이 둘 다 있는 경우
			TreeNode* pPredecessor = FindBiggestNode(pDelNode->Left);

			// 전임자는 값을 복사해주고 전임자의 자식을 전임자의 부모와 다시 이어줘야한다.
			pDelNode->Data = pPredecessor->Data;

			if (pPredecessor->Left) 
				ConnectPredecessorChildToParent(pPredecessor, pPredecessor->Left);

			// 전임자가 실제로 삭제될 노드이다.
			pDelNode = pPredecessor;
		} else if (iCount == 1) {
			// 자식이 한쪽만 있는 경우
			TreeNode* pParent = pDelNode->Parent;
			pChild->Parent = pParent;

			// 삭제되는 노드의 부모가 있을 경우, 삭제되는 노드의 자식과 부모를 올바른 위치로 연결해준다.
			if (pParent) {
				if (pParent->Left == pDelNode)
					pParent->Left = pChild;
				else
					pParent->Right = pChild;
			} else {
				// pDelNode의 부모가 없다는 말은
				//  => pDelNode = 루트라는 뜻이므로, 자식을 루트로 만들어준다.
				m_pRoot = pChild;
			}
		}

		RemoveFixup(pDelNode);
		DeleteNode(pDelNode);
		return true;
	}

	void Clear() {
		DeleteNodeRecursive(m_pRoot);
		m_pRoot = nullptr;
	}

	int Count() {
		int iCount = 0;
		CountRecursive(m_pRoot, iCount);
		return iCount;
	}

	int GetMaxHeight() {
		int iMaxHeight = 0;
		GetMaxHeightRecursive(m_pRoot, 1, iMaxHeight);
		return iMaxHeight;
	}

	void DbgGenerateTreeWithString(String data) {
		Clear();
		data.Split(" ").ForEach([this](String& s) {
			int a = StringUtil::ToNumber<Int32>(s.Source());
			 Insert(a);
		});
	}

	void DbgRemoveWithString(String data) {
		Console::WriteLine("데이터 갯수: %d", Count());
		DbgPrintHierarchical();
		data.Split(" ").ForEach([this, &data](String& s) {
			int a = StringUtil::ToNumber<Int32>(s.Source());
			int target = 20;
			if (a == target) {
				
			}
			Console::WriteLine("%d 삭제", a);
			DebugAssertMsg(Remove(a), "%d 노드 삭제 실패", a);
			Console::WriteLine("데이터 갯수: %d", Count());
			DbgPrintHierarchical();
		});
	}


	void DbgRoot(TreeNode* root) {
		DeleteNodeRecursive(m_pRoot);
		m_pRoot = root;
	}

	
	void DbgPrintHierarchical() {
		
		HashMap<int, Vector<TreeNode*>> hHierarchy;
		for (int i = 0; i < 200; ++i) {
			hHierarchy.Insert(i, Vector<TreeNode*>{});
		}
		RecordDataOnHierarchy(m_pRoot, 1, hHierarchy);
		static const char* Left = "L";
		static const char* Right = "R";
		static const char* None = "-";
		for (int i = 1; i < 200; ++i) {
			auto& nodes =  hHierarchy[i];
			if (nodes.Size() <= 0) continue;
			Console::Write("[%d] ", i);
			for (int j = 0; j < nodes.Size(); ++j) {
				const char* l = nullptr;
				if (nodes[j]->Parent == nullptr) {
					l = None;
				} else {
					if (nodes[j]->Parent->Left == nodes[j])
						l = Left;
					else
						l = Right;
				}
				Console::Write("%d(%s, %d, %s) ",
					nodes[j]->Data,
					TreeNodeColorName(nodes[j]->Color),
					nodes[j]->Parent ? nodes[j]->Parent->Data : -1,
					l
				);
			}
			Console::WriteLine("");
		}
		Console::WriteLine("==============================");
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

	static TreeNode* FindBiggestNode(TreeNode* cur) {
		while (cur != nullptr) {
			if (cur->Right == nullptr) {
				return cur;
			}

			cur = cur->Right;
		}

		return cur;
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

	// 위반 수정
	void RemoveFixup(TreeNode* child) {

		if (child->Color == TreeNodeColor::Red) {
			return;
		}

		// [1. 삭제될 노드가 자식이 1개 경우]
		TreeNode* pChild = child->Any();
		if (pChild) {
			// 케이스 1. 자식이 한개만 있는경우 (이 자식은 무조건 Red일 것이다.)
			DebugAssertMsg(child->Count() == 1, "1. 삭제될 노드에 자식이 1개만 있어야하는데 2개 있습니다.");
			DebugAssert(child->Color == TreeNodeColor::Black);
			DebugAssert(pChild->Color == TreeNodeColor::Red);
			pChild->Color = TreeNodeColor::Black;
			return;
		}

		if (child == m_pRoot) {
			return;
		}

		// [2. 삭제될 노드가 자식이 없는 경우]
		DebugAssertMsg(child->Count() == 0, "2. 삭제될 노드에 자식이 없어야하는 구간인데.. 있습니다.");
		const bool bRightChild = child->IsRight();
		const TreeNodeFamily family(child);

		// 케이스 2: 부모가 Red인 경우 (부모가 Red라는 말은 부모의 부모는 무조건 Black이다.)
		if (family.Parent->Color == TreeNodeColor::Red) {

			// 케이스 2-1: 형제가 자식이 없는 경우
			if (family.Sibling->Any() == nullptr) {
				family.Parent->Color = TreeNodeColor::Black;
				family.Sibling->Color = TreeNodeColor::Red;
				return;
			}

			DebugAssertMsg(family.Parent->Parent, "[케이스 1-2, 1-3] 부모의 부모가 없습니다.");
			DebugAssertMsg(family.Parent->Parent->Color == TreeNodeColor::Black, "[케이스 1-2, 1-3] 부모의 부모가 없습니다.");

			
			if (family.NephewLine) {
				// 케이스 2-2, 2-4: 형제가 자식이 하나라도 있는 경우
				if (bRightChild) {
					RotateLL(family.Parent);
				} else {
					RotateRR(family.Parent);
				}

				// 케이스 2-4는 형제의 자식이 2명다 있을 경우 색상 변경을 해줘야함.
				if (family.NephewTri == nullptr) {
					return;
				}

				// 회전으로 인해서 형제 노드가 부모자리로 올라감 (색상 변경 수행)
				family.Sibling->Color = TreeNodeColor::Red;

				if (family.Sibling->Left)
					family.Sibling->Left->Color = TreeNodeColor::Black;
				if (family.Sibling->Right)
					family.Sibling->Right->Color = TreeNodeColor::Black;

			} else {
				// 케이스 2-3: 형제가 우측 꺽인 자식이 있는 경우
				DebugAssertMsg(family.Sibling->Color == TreeNodeColor::Black, "[케이스 2-3] 형제가 블랙이 아닙니다.");
				DebugAssertMsg(family.NephewTri->Color == TreeNodeColor::Red, "[케이스 2-3] 꺽인 조카가 레드가 아닙니다.");
				family.Sibling->Color = TreeNodeColor::Red;
				family.NephewTri->Color = TreeNodeColor::Black;

				if (bRightChild) {
					RotateLR(family.Parent);
				} else {
					RotateRL(family.Parent);
				}
			}
			return;
		} 

		const bool bHasNephew = family.NephewLine && family.NephewTri;

		// 케이스 3. 부모가 Black인 경우
		if (!bHasNephew) {

			// 케이스 3-1. 조카가 없는 경우
			DebugAssertMsg(family.Sibling->Color == TreeNodeColor::Black, "ㄷㄷ 형제가 Black이 아니라면.? 조카가 있다는 말인데? 여기선 내가 Black, 부모가 Black이면 형제는 당연히 Black이어야함.. bHasNephew가 False라니.. 심각한 오류다..");
			family.Sibling->Color = TreeNodeColor::Red;
			RemoveFixupExtraBlack(family.Parent);
			return;
		}

		
		if (family.Sibling->Color == TreeNodeColor::Black) {

			if (family.NephewLine) {
				// 케이스 3-2, 3-3: 라인 조카가 있는 경우
				family.NephewLine->Color = TreeNodeColor::Black;

				if (bRightChild)
					RotateLL(family.Parent);
				else
					RotateRR(family.Parent);
				
			} else {
				// 케이스 3-4: 꺽인 조카가 있는 경우
				DebugAssertMsg(family.Sibling->Color == TreeNodeColor::Black, "[케이스 3-4] 형제가 블랙이 아닙니다.");
				DebugAssertMsg(family.NephewTri->Color == TreeNodeColor::Red, "[케이스 3-4] 꺽인 조카가 레드가 아닙니다.");
				family.NephewTri->Color = TreeNodeColor::Black;

				if (bRightChild) {
					RotateLR(family.Parent);
				} else {
					RotateRL(family.Parent);
				}
			}

			return;
		}

		// 케이스 3-5
		family.Parent->Color = TreeNodeColor::Red;
		family.Sibling->Color = TreeNodeColor::Black;
		
		if (bRightChild)
			RotateLL(family.Parent);
		else
			RotateRR(family.Parent);

		RemoveFixup(child);
	}

	// 엑스트라 Black 속성이 부여된 노드를 대상으로 위반 수정
	// 난 엑스트라 Black 속성이 이 함수에 들어온 것 자체로 부여되었다는 걸로 간주하기로 함.
	void RemoveFixupExtraBlack(TreeNode* child) {

		if (m_pRoot == child) {
			// 루트는 엑스트라 Black속성이 부여될 경우 없애기만 하면 됨.
			//	난 엑스트라 Black이라는 추가 정보를 굳이 노드에 담아서 표현할 필요 없다고 생각한다.
			//	삭제중 일시적으로 존재하는 속성이기 떄문이다.
			return;
		}

		const bool bRightChild = child->IsRight();
		const TreeNodeFamily family(child);

		// 엑스트라 Black이 더해진 Black 노드라면 조카들이 2명다 없을 수 없다.
		DebugAssertMsg(family.NephewLine, "라인 조카가 없습니다.");
		DebugAssertMsg(family.NephewTri, "꺽인 조카가 없습니다.");

		if (family.Parent->Color == TreeNodeColor::Red) {

			const TreeNodeColor eNephewLineColor = family.NephewLine->Color;
			const TreeNodeColor eNephewTriColor = family.NephewTri->Color;

			
			if (eNephewLineColor == TreeNodeColor::Black) {
				
				if (eNephewTriColor == TreeNodeColor::Black) {
					// 케이스 3-1-1
					family.Sibling->Color = TreeNodeColor::Red;
					family.Parent->Color = TreeNodeColor::Black;
				} else {
					// 케이스 3-1-2
					family.NephewTri->Color = TreeNodeColor::Black;
					family.Sibling->Color = TreeNodeColor::Red;
					RemoveFixupExtraBlack(child);	// 케이스 3-1-3 수행을 위해
				}
			} else {
				// 케이스 3-1-3
				family.NephewLine->Color = TreeNodeColor::Black;

				if (bRightChild)
					RotateLL(family.Parent);
				else
					RotateRR(family.Parent);
			}
			
			return;
		}

		// 케이스 3-1-4
		if (family.Sibling->Color == TreeNodeColor::Black) {
			family.Sibling->Color = TreeNodeColor::Red;
			RemoveFixupExtraBlack(family.Parent);
			return;
		} 

		// 케이스 3-1-5
		family.Parent->Color = TreeNodeColor::Red;
		family.Sibling->Color = TreeNodeColor::Black;

		if (bRightChild)
			RotateLL(family.Parent);
		else
			RotateRR(family.Parent);

		RemoveFixupExtraBlack(child);
	}

	// 노드가 왼쪽/왼쪽으로 붙은 경우
	void RotateLL(TreeNode* node) {
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

		// 회전으로 인한 루트 변경 업데이트
		if (m_pRoot == pCur) {
			m_pRoot = pChild;
		}
	}

	// 노드가 오른쪽/오른쪽으로 붙은 경우
	void RotateRR(TreeNode* node) {
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

		// 회전으로 인한 루트 변경 업데이트
		if (m_pRoot == pCur) {
			m_pRoot = pChild;
		}
	}

	void RotateLR(TreeNode* cur) {
		RotateRR(cur->Left);
		RotateLL(cur);
	}

	void RotateRL(TreeNode* cur) {
		RotateLL(cur->Right);
		RotateRR(cur);
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

	static void GetMaxHeightRecursive(TreeNode* node, int height, int& maxHeight) {
		if (node == nullptr) {
			maxHeight = Math::Max(maxHeight, height);
			return;
		}

		GetMaxHeightRecursive(node->Left, height + 1, maxHeight);
		GetMaxHeightRecursive(node->Right, height + 1, maxHeight);
	}

	static void CountRecursive(TreeNode* node, int& count) {
		if (node == nullptr) {
			return;
		}
		count++;
		CountRecursive(node->Left, count);
		CountRecursive(node->Right, count);
	}


	TreeNode* m_pRoot;
};

#define ON	1
#define OFF 0

#define CASE_1_1	OFF
#define CASE_1_2	OFF
#define CASE_1_3	OFF
#define CASE_2_1	OFF
#define CASE_2_2	OFF
#define CASE_2_3	OFF
#define CASE_3_1_1	OFF
#define CASE_3_1_2	OFF
#define CASE_3_1_3	OFF
#define CASE_3_1_3	OFF

#define CASE_ALL_RANDOM OFF

int main() {
	Console::SetSize(800, 600);
	



	{
#if CASE_1_1
		// 1-1		https://drive.google.com/file/d/1PZdhLbMnbN_PVSVJ5KnSJ_ROfLKbRFiV/view?usp=sharing
		// 1-1(R)	스크린샷 없음
		TreeSet set;
		Console::WriteLine("케이스 1-1");
		TreeNode* n10 = new TreeNode{ 10, TreeNodeColor::Black };
		TreeNode* n20 = new TreeNode{ 20, TreeNodeColor::Black };
		TreeNode* n40 = new TreeNode{ 40 };
		TreeNode* n30 = new TreeNode{ 30, TreeNodeColor::Black };
		TreeNode* n25 = new TreeNode{ 25 };
		TreeNode* n50 = new TreeNode{ 50, TreeNodeColor::Black };
		TreeNode* n60 = new TreeNode{ 60 };

		TreeNode::ConnectLeft(n20, n10);
		TreeNode::ConnectRight(n20, n40);
		TreeNode::ConnectLeft(n40, n30);
		TreeNode::ConnectRight(n40, n50);
		TreeNode::ConnectLeft(n30, n25);
		TreeNode::ConnectRight(n50, n60);
		set.DbgRoot(n20);
		set.DbgPrintHierarchical();
		set.Remove(50);
		set.DbgPrintHierarchical();
#endif // CASE_1_1
	}



	{
#if CASE_1_2
		// 1-2		https://drive.google.com/file/d/1zxt0Fj-TYkMj4JS41fKppEo8AE0Y6gb0/view?usp=sharing
		// 1-2(R)	https://drive.google.com/file/d/1FBReWnHXm6Xn6jnDZunALd_u5DcIFnvL/view?usp=sharing
		TreeSet set;
		Console::WriteLine("케이스 1-2");
#endif
	}

	{
#if CASE_1_3
		// 1-3		https://drive.google.com/file/d/1CuZ46dpsUh5s2glAsgK5HavnQR4Yj7pX/view?usp=sharing
		// 1-3(R)	https://drive.google.com/file/d/1nH3-3FeNphUi5Z74t7p3cNywVhJ-BT6b/view?usp=sharing
		TreeSet set;
		Console::WriteLine("케이스 1-3");
#endif
	}


	{
#if CASE_2_1
		// 2-1		https://drive.google.com/file/d/1c1PyCOhHg3r4olRmq-nORENAtUFFfnMT/view?usp=sharing
		// 2-1(R)	https://drive.google.com/file/d/1AJJ5wGcZrrcAOKti9sVxdmKssQRLSjvJ/view?usp=sharing

		Console::WriteLine("케이스 2-1");
		TreeSet set;
		TreeNode* n40 = new TreeNode{ 40, TreeNodeColor::Black };

		TreeNode* n20 = new TreeNode{ 20 };
		TreeNode* n10 = new TreeNode{ 10, TreeNodeColor::Black };
		TreeNode* n30 = new TreeNode{ 30, TreeNodeColor::Black };

		TreeNode* n50 = new TreeNode{ 50 };
		TreeNode* n45 = new TreeNode{ 45, TreeNodeColor::Black };
		TreeNode* n60 = new TreeNode{ 60, TreeNodeColor::Black };

		TreeNode::ConnectLeft(n40, n20);
		TreeNode::ConnectRight(n40, n50);

		TreeNode::ConnectLeft(n20, n10);
		TreeNode::ConnectRight(n20, n30);

		TreeNode::ConnectLeft(n50, n45);
		TreeNode::ConnectRight(n50, n60);

		set.DbgRoot(n40);
		set.DbgPrintHierarchical();
		set.Remove(60);		// set.Remove(50);
		set.DbgPrintHierarchical();
#endif
	}

	{
#if CASE_2_2
		// 2-2		https://drive.google.com/file/d/18-lm3yKfAF-P4j6EXrwaQlndpvruT1un/view?usp=sharing
		// 2-2(R)	https://drive.google.com/file/d/1DZNwYp1Yhh0zvx63zlgZOrNaGMHhNbzM/view?usp=sharing

		Console::WriteLine("케이스 2-2");
		TreeSet set;
		TreeNode* n40 = new TreeNode{ 40, TreeNodeColor::Black };
		TreeNode* n20 = new TreeNode{ 20 };
		TreeNode* n10 = new TreeNode{ 10, TreeNodeColor::Black };
		TreeNode* n30 = new TreeNode{ 30, TreeNodeColor::Black };
		TreeNode* n60 = new TreeNode{ 60 };
		TreeNode* n50 = new TreeNode{ 50, TreeNodeColor::Black };
		TreeNode* n45 = new TreeNode{ 45 };
		TreeNode* n70 = new TreeNode{ 70, TreeNodeColor::Black };

		TreeNode::ConnectLeft(n40, n20);

		TreeNode::ConnectLeft(n20, n10);
		TreeNode::ConnectRight(n20, n30);

		TreeNode::ConnectLeft(n60, n50);
		TreeNode::ConnectRight(n60, n70);
		TreeNode::ConnectLeft(n50, n45);

		TreeNode::ConnectRight(n40, n60);

		set.DbgRoot(n40);
		set.DbgPrintHierarchical();
		set.Remove(70);
		set.DbgPrintHierarchical();
#endif
	}

	{
#if CASE_2_3
		// 2-3		https://drive.google.com/file/d/1UtEA2dDg5Gf_k8IkG63jOro5ARi2h4ZX/view?usp=sharing
		// 2-3(R)	https://drive.google.com/file/d/1NudN8C5dopqY4Cf3oOjX6-sIk0cS1GHO/view?usp=sharing

		Console::WriteLine("케이스 2-3");
		TreeSet set;
		TreeNode* n40 = new TreeNode{ 40, TreeNodeColor::Black };
		TreeNode* n20 = new TreeNode{ 20 };
		TreeNode* n10 = new TreeNode{ 10, TreeNodeColor::Black };
		TreeNode* n30 = new TreeNode{ 30, TreeNodeColor::Black };
		TreeNode* n60 = new TreeNode{ 60 };
		TreeNode* n50 = new TreeNode{ 50, TreeNodeColor::Black };
		TreeNode* n45 = new TreeNode{ 45 };
		TreeNode* n55 = new TreeNode{ 55 };
		TreeNode* n70 = new TreeNode{ 70, TreeNodeColor::Black };

		TreeNode::ConnectLeft(n40, n20);

		TreeNode::ConnectLeft(n20, n10);
		TreeNode::ConnectRight(n20, n30);

		TreeNode::ConnectLeft(n60, n50);
		TreeNode::ConnectRight(n60, n70);
		TreeNode::ConnectLeft(n50, n45);
		TreeNode::ConnectRight(n50, n55);

		TreeNode::ConnectRight(n40, n60);

		set.DbgRoot(n40);
		set.DbgPrintHierarchical();
		set.Remove(70);
		set.DbgPrintHierarchical();
#endif
	}

#if CASE_ALL_RANDOM
	Vector<int> nodes;
	Random::EngineInitialize();
	constexpr int itemCount = 16;
	for (int i = 0; i < itemCount; ++i) { nodes.PushBack(i); }
	
	for (int i = 0; i < 100; ++i) {
		Vector<int> pushOrder;
		Vector<int> popOrder;
		bool pushPicked[itemCount]{};
		bool popPicked[itemCount]{};

		for (;;) {
			int idx = Random::GenerateInt(0, itemCount);
			if (!pushPicked[idx]) {
				pushPicked[idx] = true;
				pushOrder.PushBack(idx);
			}

			if (Arrays::AllEqual(pushPicked)) {
				break;
			}
		}

		for (;;) {
			int idx = Random::GenerateInt(0, itemCount);
			if (!popPicked[idx]) {
				popPicked[idx] = true;
				popOrder.PushBack(idx);
			}

			if (Arrays::AllEqual(popPicked)) {
				break;
			}
		}

		Console::Write("삽입 순서: ");
		pushOrder.ForEach([](auto s) { Console::Write("%d ", s); }); Console::WriteLine("");

		Console::Write("삭제 순서: ");
		popOrder.ForEach([](auto s) { Console::Write("%d ", s); }); Console::WriteLine("");

		TreeSet set;
		for (int j = 0; j < itemCount; ++j) {
			set.Insert(nodes[pushOrder[j]]);
		}
		set.DbgPrintHierarchical();
		for (int j = 0; j < itemCount; ++j) {
			int r = nodes[popOrder[j]];
			Console::WriteLine("%d 삭제시도", r);
			DebugAssert(set.Remove(r));
			set.DbgPrintHierarchical();
		}

		Console::WriteLine("[%d]", i);
	}
#endif
	/*5 삭제시도
	[1] 4(Black, -1, -)
	[2] 1(Red, 4, L) 7(Black, 4, R)
	[3] 0(Black, 1, L) 3(Black, 1, R) 6(Red, 7, L)
	[4] 2(Red, 3, L)
	==============================
	4 삭제시도
	[1] 3(Black, -1, -)
	[2] 2(Black, 3, L) 7(Black, 3, R)
	[3] 6(Red, 7, L)*/

	{
		TreeSet set;
		set.DbgGenerateTreeWithString("9 7 0 15 14 12 3 13 1 10 6 2 4 5 8 11");
		set.DbgRemoveWithString("10 15 14 0 1 9 6 4 11 13 5 3 8 2 12 7");

	}

	return 0;
}







