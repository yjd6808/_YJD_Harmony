/*
 *	작성자 : 윤정도
 *	생성일 : 2023/06/01
 *	레드블랙트리 학습결과 보고서: https://blog.naver.com/reversing_joa/223116951373
 */

#pragma once

#include <JCore/Hasher.h>
#include <JCore/Memory.h>
#include <JCore/Comparator.h>

#include <JCore/Container/MapCollection.h>
#include <JCore/Container/LinkedList.h>
#include <JCore/Container/TreeMapIterator.h>

NS_JC_BEGIN

struct TreeNodeColor { enum _ { eBlack, eRed }; };

template <typename TKey, typename TValue>
struct TreeNode
{
	using TKeyValuePair	= Pair<TKey, TValue>;
	using TTreeNode		= TreeNode<TKey, TValue>;

	enum Color
	{
		eBlack,
		eRed
	};

	template <typename Ky, typename Vy>
	TreeNode(Ky&& key, Vy&& value)
		: Parent(nullptr)
		, Left(nullptr)
		, Right(nullptr)
		, Color(TreeNodeColor::eRed) {
		Pair.Key = Forward<Ky>(key);
		Pair.Value = Forward<Vy>(value);
	}
	
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

	TKeyValuePair Pair;
	TTreeNode* Parent;
	TTreeNode* Left;
	TTreeNode* Right;
	char Color;
};


template <typename TKey, typename TValue, typename TKeyComparator = Comparator<TKey>, typename TAllocator = DefaultAllocator>
class TreeMap : public MapCollection<TKey, TValue, TAllocator>
{
	static_assert(IsComparator_v<TKeyComparator>, "... TKeyComparator is not comparator");
	/*
	 * 트리맵은 이터레이션을 어떻게 수행해야할까? (생각의 흐름)
	 *  -> 우선 중위순회(inorder traverse)와 역방향 중위순회(reverse inorder traverse)를 활용해야 할 것이다.
	 *  -> 현재 노드기준에서 다음 노드로 이동할려고 한다고 가정해보자.
	 *	-> 이때 1단계 더 큰 노드가 다음 노드가 될 것이다.
	 *	-> 1단계 더 큰 노드는 어떤 노드일까?
	 *	-> A라는 노드가 있을 때 A노드 기준 우측 서브트리에서 가장 작은 노드(Successor)가 1단계 큰 노드가 될 수 있다.
	 *	    - 1. 만약 A의 우측 서브트리가 존재하지 않고, A의 부모가 있을 때 A가 좌측 자식인 경우 A의 부모가 1단계 큰 노드가 될 수 있다.
	 *      - 2. 만약 A의 우측 서버트리가 존재하지 않고, A의 부모가 있을 때 A가 우측 자식인 경우 연속 우측 분기가 종료되는 지점의 노드의 부모가 1단계 더 큰 노드이다.
	 *		     우측자식인 경우는 이해가 잘 안될 수 있어서 예시 자료를 첨부한다.
	 *           https://drive.google.com/file/d/1rpboZM-cm4NS0VkQFhEF934L2OsFKmes/view?usp=sharing
	 *			 150의 연속 우측 분기가 종료되는 지점은 200
	 *			 275의 연속 우측 분기가 종료되는 지점은 300
	 *			 375의 연속 우측 분기가 종료되는 지점은 400이된다.
	 *			
	 *	-> 이 2가지 조건에 만족하는 노드가 없다면 A는 해당 트리에서 가장 큰 노드라는 것이다.
	 *  -> 이 생각의 흐름을 토대로 함수: FindBiggerNode를 정의해보자.
	 */
	enum class TraverseValueType
	{
		Pair,
		Key,
		Value
	};

	enum class RotateMode
	{
		LL,
		RR
	};
public:
	using THasher					= Hasher<TKey>;
	using TKyComparator				= TKeyComparator;
	using TTreeNode					= TreeNode<TKey, TValue>;
	using TMapCollection			= MapCollection<TKey, TValue, TAllocator>;
	using TKeyValuePair				= Pair<TKey, TValue>;
	using TIterator					= Iterator<TKeyValuePair, TAllocator>;
	using TTreeMap					= TreeMap<TKey, TValue, TKeyComparator, TAllocator>;
	using TTreeMapIterator			= TreeMapIterator<TKey, TValue, TKeyComparator, TAllocator>;
	using TKeyCollection			= typename TMapCollection::KeyCollection;
	using TValueCollection			= typename TMapCollection::ValueCollection;
	using TKeyCollectionIterator	= typename TMapCollection::KeyCollectionIterator;
	using TValueCollectionIterator	= typename TMapCollection::ValueCollectionIterator;
public:
	// 내부 구조체 전방 선언 (inner struct forward declaration)
	struct TreeMapKeyCollection;
	struct TreeMapKeyCollectionIterator;
	struct TreeMapValueCollection;
	struct TreeMapValueCollectionIterator;
public:
	TreeMap()
		: TMapCollection()
		, m_pRoot(nullptr)
	{}

	TreeMap(const TTreeMap& other) : TreeMap() {
		operator=(other);
	}

	TreeMap(TTreeMap&& other) noexcept : TreeMap() {
		operator=(Move(other));
	}

	TreeMap(std::initializer_list<TKeyValuePair> ilist) : TreeMap() {
		operator=(ilist);
	}

	~TreeMap() noexcept override {
		TTreeMap::Clear();
	}
public:

	TTreeMap& operator=(const TTreeMap& other) {
		TTreeMap::Clear();
		InorderTraverseForEach<TraverseValueType::Pair>(other.m_pRoot, [this](auto& pair) { Insert(pair.Key, pair.Value); });
		this->m_iSize = other.m_iSize;
		return *this;
	}

	TTreeMap& operator=(TTreeMap&& other) noexcept {
		TTreeMap::Clear();

		this->m_Owner = Move(other.m_Owner);
		this->m_pRoot = other.m_pRoot;
		this->m_iSize = other.m_iSize;

		other.m_pRoot = nullptr;
		other.m_iSize = 0;

		return *this;
	}

	TTreeMap& operator=(std::initializer_list<TKeyValuePair> ilist) {
		TTreeMap::Clear();

		for (auto it = ilist.begin(); it != ilist.end(); ++it) {
			Insert((*it));
		}

		return *this;
	}

	TValue& operator[](const TKey& key) {
		return Get(key);
	}


	template <typename Ky, typename Vy>
	bool Insert(Ky&& key, Vy&& value) {
		TTreeNode* pNewNode;

		// 1. 데이터를 먼저 넣는다.
		if (m_pRoot == nullptr) {
			pNewNode = m_pRoot = TAllocator::template AllocateInit<TTreeNode>(Forward<Ky>(key), Forward<Vy>(value));
		} else {
			// data가 삽입될 부모 노드를 찾는다.
			TTreeNode* pParent = FindParentDataInserted(key);

			if (pParent == nullptr) {
				return false;
			}

			// 동일한 키값을 가진 데이터가 있으면 안되므로..
			if (ms_KeyComparator(key, pParent->Pair.Key) == 0) {
				return false;
			}

			pNewNode = TAllocator::template AllocateInit<TTreeNode>(Forward<Ky>(key), Forward<Vy>(value));
			pNewNode->Parent = pParent;

			// key, value가 pNewNode를 생성할 때 포워딩되기 때문에 만약 rvalue로 들어올 경우 잘못된 결과를 얻을 수 있다.
			// 따라서 key 대신 pNewNode->Pair.Key를 사용해야함.
			if (ms_KeyComparator(pNewNode->Pair.Key, pParent->Pair.Key) > 0) {
				pParent->Right = pNewNode;
			} else {
				pParent->Left = pNewNode;
			}
		}

		// 2. 삽입된 노드를 기준으로 레드블랙트리가 위반되는지 확인하여 바로잡는다.
		InsertFixup(pNewNode);
		this->m_iSize += 1;
		return true;
	}

	bool Insert(const TKeyValuePair& pair) override {
		return Insert(pair.Key, pair.Value);
	}

	bool Insert(TKeyValuePair&& pair) override {
		return Insert(Move(pair.Key), Move(pair.Value));
	}

	bool Exist(const TKey& key) const override {
		return FindNode(key) != nullptr;
	}

	virtual TValue* Find(const TKey& key) const {
		TTreeNode* pFind = FindNode(key);

		if (pFind == nullptr) {
			return nullptr;
		}

		return &pFind->Pair.Value;
	}

	TValue& Get(const TKey& key) const override {
		TTreeNode* pFind = FindNode(key);

		if (pFind == nullptr) {
			throw InvalidArgumentException("해당 키값에 대응하는 값이 존재하지 않습니다.");
		}

		return pFind->Pair.Value;
	}

	bool Remove(const TKey& key) override {
		TTreeNode* pDelNode = FindNode(key);

		if (pDelNode == nullptr) {
			return false;
		}

		// 자식이 없는 경우 그냥 바로 제거 진행
		int iCount = 0;
		TTreeNode* pChild = pDelNode->AnyWithChildrenCount(iCount);

		if (iCount == 2) {
			// 자식이 둘 다 있는 경우
			TTreeNode* pPredecessor = FindBiggestNode(pDelNode->Left);

			// 전임자는 값을 이동해주고 전임자의 자식을 전임자의 부모와 다시 이어줘야한다.
			pDelNode->Pair = Move(pPredecessor->Pair);

			if (pPredecessor->Left)
				ConnectPredecessorChildToParent(pPredecessor, pPredecessor->Left);

			// 전임자가 실제로 삭제될 노드이다.
			pDelNode = pPredecessor;


		} else if (iCount == 1) {
			// 자식이 한쪽만 있는 경우
			TTreeNode* pParent = pDelNode->Parent;
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
		this->m_iSize -= 1;
		return true;
	}


	void Clear() noexcept override {
		DeleteNodeRecursive(m_pRoot);
		m_pRoot = nullptr;
		this->m_iSize = 0;
	}

	int MaxHeight() const {
		int iMaxHeight = 0;
		MaxHeightRecursive(m_pRoot, 1, iMaxHeight);
		return iMaxHeight;
	}


	// ==========================================
	// 동적할당 안하고 트리맵 순회할 수 있도록 기능 구현
	// ==========================================
	template <typename Consumer>
	void ForEach(Consumer&& consumer) {
		InorderTraverseForEach<TraverseValueType::Pair>(m_pRoot, Forward<Consumer>(consumer));
	}

	template <typename Consumer>
	void ForEachKey(Consumer&& consumer) {
		InorderTraverseForEach<TraverseValueType::Key>(m_pRoot, Forward<Consumer>(consumer));
	}

	template <typename Consumer>
	void ForEachValue(Consumer&& consumer) {
		InorderTraverseForEach<TraverseValueType::Value>(m_pRoot, Forward<Consumer>(consumer));
	}

	SharedPtr<TIterator> Begin() const override { return MakeShared<TTreeMapIterator, TAllocator>(this->GetOwner(), FindSmallestNode(m_pRoot)); }
	SharedPtr<TIterator> End() const override { return MakeShared<TTreeMapIterator, TAllocator>(this->GetOwner(), FindBiggestNode(m_pRoot)); }
	TreeMapKeyCollection Keys() { return TreeMapKeyCollection(this); }
	TreeMapValueCollection Values() { return TreeMapValueCollection(this); }
	ContainerType GetContainerType() override { return ContainerType::TreeMap; }
protected:

	TTreeNode* FindNode(const TKey& key) const {
		TTreeNode* pCur = m_pRoot;

		while (pCur != nullptr) {
			if (ms_KeyComparator(key, pCur->Pair.Key) == 0) {
				return pCur;
			}

			if (ms_KeyComparator(key, pCur->Pair.Key) > 0) {
				pCur = pCur->Right;
			} else {
				pCur = pCur->Left;
			}
		}

		return nullptr;
	}

	// data가 삽입될 부모를 찾는다.
	TTreeNode* FindParentDataInserted(const TKey& key) {
		TTreeNode* pParent = nullptr;
		TTreeNode* pCur = m_pRoot;

		while (pCur != nullptr) {
			pParent = pCur;

			if (ms_KeyComparator(key, pCur->Pair.Key) > 0) {
				pCur = pCur->Right;
			} else {
				pCur = pCur->Left;
			}
		}

		return pParent;
	}

	// 삽입 위반 수정
	void InsertFixup(TTreeNode* child) {

		// (1) 루트 노드는 Black이다.
		if (child == m_pRoot) {
			child->Color = TreeNodeColor::eBlack;
			return;
		}

		TTreeNode* pParent = child->Parent;		// (1)에서 종료되지 않았다면 무조건 부모가 존재함.
		char eParentColor = pParent->Color;

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
		if (eParentColor != TreeNodeColor::eRed || child->Color != TreeNodeColor::eRed) {
			return;
		}

		// 노드 깊이(트리 높이)가 2인 경우는 모두 위 IF문에서 걸러지므로 이후로 GrandParent가 nullptr일 수 없다.
		TTreeNode* pGrandParent = pParent->Parent;
		TTreeNode* pUncle = nullptr;						// 삼촌 노드정보 (부모가 조상님의 왼쪽자식인 경우 조상님의 오른쪽 자식이 삼촌 노드)
		if (pGrandParent != nullptr) {
			if (pGrandParent->Left == pParent)
				pUncle = pGrandParent->Right;
			else
				pUncle = pGrandParent->Left;
		}
		DebugAssertMsg(pGrandParent, "그랜드 부모가 NULL입니다.");
		const char eUncleColor = pUncle ? pUncle->Color : TreeNodeColor::eBlack; // 삼촌 노드는 있을 수도 없을 수도 있고. NIL 노드는 Black이다.


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
		if (eUncleColor == TreeNodeColor::eBlack) {
			if (pParent->IsLeft()) {
				if (child->IsLeft()) {
					// Case 1-1
					pGrandParent->Color = TreeNodeColor::eRed;
					pParent->Color = TreeNodeColor::eBlack;
					RotateLL(pGrandParent);
				} else {
					// Case 1-3
					RotateRR(pParent);
					InsertFixup(pParent);
				}
			} else {
				if (child->IsRight()) {
					// Case 1-2
					pGrandParent->Color = TreeNodeColor::eRed;
					pParent->Color = TreeNodeColor::eBlack;
					RotateRR(pGrandParent);
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



		pUncle->Color = TreeNodeColor::eBlack;
		pParent->Color = TreeNodeColor::eBlack;
		pGrandParent->Color = TreeNodeColor::eRed;
		InsertFixup(pGrandParent);
	}

	void DeleteNode(TTreeNode* node) {
		if (node == m_pRoot) {
			JCORE_ALLOCATOR_STATIC_DEALLOCATE_AND_DESTROY_SAFE(TTreeNode, node);
			return;
		}

		if (node->Parent) {
			if (node->Parent->Left == node)
				node->Parent->Left = nullptr;
			else if (node->Parent->Right == node)	// 부유 상태의 node일 수 있으므로 무조건 체크
				node->Parent->Right = nullptr;
		}

		JCORE_ALLOCATOR_STATIC_DEALLOCATE_AND_DESTROY_SAFE(TTreeNode, node);
	}

	// 삭제 위반 수정
	void RemoveFixup(TTreeNode* child) {

		if (child->Color == TreeNodeColor::eRed) {
			return;
		}

		// [1. 삭제될 노드가 자식이 1개 경우]
		TTreeNode* pChild = child->Any();
		if (pChild) {
			// 케이스 1. 자식이 한개만 있는경우 (이 자식은 무조건 Red일 것이다.)
			DebugAssertMsg(child->Count() == 1, "1. 삭제될 노드에 자식이 1개만 있어야하는데 2개 있습니다.");
			DebugAssert(child->Color == TreeNodeColor::eBlack);
			DebugAssert(pChild->Color == TreeNodeColor::eRed);
			pChild->Color = TreeNodeColor::eBlack;
			return;
		}

		if (child == m_pRoot) {
			return;
		}

		RemoveFixupExtraBlack(child);
	}

	// 엑스트라 Black 속성이 부여된 노드를 대상으로 위반 수정
	// 난 엑스트라 Black 속성이 이 함수에 들어온 것 자체로 부여되었다는 걸로 간주하기로 함.
	void RemoveFixupExtraBlack(TTreeNode* child) {

		if (m_pRoot == child) {
			// 루트는 엑스트라 Black속성이 부여될 경우 없애기만 하면 됨.
			//	난 엑스트라 Black이라는 추가 정보를 굳이 노드에 담아서 표현할 필요 없다고 생각한다.
			//	삭제중 일시적으로 존재하는 속성이기 떄문이다.
			return;
		}

		#pragma region TreeNodeFamily 함수내부 구현
		struct TreeNodeFamily
		{
			/* Not Null */ TTreeNode* Parent;
			/* Not Null */ TTreeNode* Sibling;
			/* Nullable */ TTreeNode* NephewLine;
			/* Nullable */ TTreeNode* NephewTri;

			char ParentColor;
			char SiblingColor;
			char NephewLineColor;
			char NephewTriColor;

			TreeNodeFamily(TTreeNode* child) {
				const bool bRightChild = child->IsRight();
				Parent = child->Parent;									// 부모 노드
				DebugAssertMsg(Parent, "부모노드 없을 수 없습니다.");

				Sibling = bRightChild ? Parent->Left : Parent->Right;	// 형제 노드 (child가 우측이면 부모의 왼쪽 노드가 형제 노드)
				DebugAssertMsg(Sibling, "형제노드가 없을 수 없습니다.");

				const TTreeNode* pNephewLine = nullptr;					// 조카 노드 (일렬로 나열)		
				const TTreeNode* pNephewTri = nullptr;					// 조카 노드 (꺽여서 나열)

				if (Sibling->IsLeft()) {
					NephewLine = Sibling->Left;
					NephewTri = Sibling->Right;
				}
				else {
					NephewLine = Sibling->Right;
					NephewTri = Sibling->Left;
				}

				// 노드가 없는 경우 Black으로 판정토록한다.
				ParentColor = Parent->Color;
				SiblingColor = Sibling->Color;
				NephewTriColor = NephewTri ? NephewTri->Color : TreeNodeColor::eBlack;
				NephewLineColor = NephewLine ? NephewLine->Color : TreeNodeColor::eBlack;
			}

		};
		#pragma endregion

		const bool bRightChild = child->IsRight();
		const TreeNodeFamily family(child);


		// 그룹 케이스 2: 부모의 색이 Black인 경우
		if (family.ParentColor == TreeNodeColor::eBlack) {

			// 케이스 5. (형제가 Red인 경우)
			if (family.SiblingColor == TreeNodeColor::eRed) {
				family.Parent->Color = TreeNodeColor::eRed;
				family.Sibling->Color = TreeNodeColor::eBlack;
				RotateNode(family.Parent, bRightChild ? RotateMode::LL : RotateMode::RR);
				RemoveFixupExtraBlack(child);
				return;
			}

			// 케이스 1 ~ 4 (형제가 Black인 경우)
			if (family.NephewTriColor == TreeNodeColor::eBlack &&
				family.NephewLineColor == TreeNodeColor::eBlack) {
				// 케이스 1. 조카 모두 Black인 경우
				family.Sibling->Color = TreeNodeColor::eRed;
				RemoveFixupExtraBlack(family.Parent);			// Extra Black을 없앨 수 없으므로 부모로 전달
				return;
			}

			if (family.NephewLineColor == TreeNodeColor::eRed) {
				// 케이스 2. 라인조카가 Red인 경우
				family.NephewLine->Color = TreeNodeColor::eBlack;
				RotateNode(family.Parent, bRightChild ? RotateMode::LL : RotateMode::RR);
				return;
			}

			if (family.NephewTriColor == TreeNodeColor::eRed) {
				// 케이스 3. 꺽인조카가 Red인 경우
				family.NephewTri->Color = TreeNodeColor::eBlack;
				family.Sibling->Color = TreeNodeColor::eRed;
				RotateNode(family.Sibling, bRightChild ? RotateMode::RR : RotateMode::LL);
				RemoveFixupExtraBlack(child);	// 케이스 2로 처리하기위해 재호출
				return;
			}

			return;
		}

		DebugAssertMsg(family.SiblingColor == TreeNodeColor::eBlack, "[그룹 케이스 1] 형제노드가 Black이 아닙니다.");
		// 그룹 케이스 1: 부모의 색이 Red인 경우
		if (family.NephewTriColor == TreeNodeColor::eBlack &&
			family.NephewLineColor == TreeNodeColor::eBlack) {
			// 케이스 1. 조카 모두 Black인 경우
			family.Sibling->Color = TreeNodeColor::eRed;
			family.Parent->Color = TreeNodeColor::eBlack;
			return;
		}

		if (family.NephewLineColor == TreeNodeColor::eRed) {
			// 케이스 2. 라인조카가 Red인 경우

			family.NephewLine->Color = TreeNodeColor::eBlack;
			family.Sibling->Color = TreeNodeColor::eRed;
			family.Parent->Color = TreeNodeColor::eBlack;
			RotateNode(family.Parent, bRightChild ? RotateMode::LL : RotateMode::RR);
			return;
		}

		if (family.NephewTriColor == TreeNodeColor::eRed) {
			// 케이스 3. 꺽인조카가 Red인 경우
			family.NephewTri->Color = TreeNodeColor::eBlack;
			family.Sibling->Color = TreeNodeColor::eRed;
			RotateNode(family.Sibling, bRightChild ? RotateMode::RR : RotateMode::LL);
			RemoveFixupExtraBlack(child); // 케이스 2로 처리하기위해 재호출
		}

	}

	void RotateNode(TTreeNode* node, RotateMode mode) {
		switch (mode) {
		case RotateMode::RR: RotateRR(node); return;
		case RotateMode::LL: RotateLL(node); return;
		}
	}
	// 노드가 왼쪽/왼쪽으로 붙은 경우
	void RotateLL(TTreeNode* node) {
		//        ?		- pParent
		//      5		- pCur
		//    3			- pChild
		//  1   ?		- pChildRight

		//      ?		- pParent
		//    3			- pChild
		//  1   5		- pCur
		//    ?			- pChildRight

		TTreeNode* pParent = node->Parent;
		TTreeNode* pCur = node;
		TTreeNode* pChild = node->Left;
		TTreeNode* pChildRight = node->Left->Right;

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
	void RotateRR(TTreeNode* node) {
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

		TTreeNode* pParent = node->Parent;
		TTreeNode* pCur = node;
		TTreeNode* pChild = node->Right;
		TTreeNode* pChildLeft = node->Right->Left;

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

	// node서브트리 기준으로 제일 왼쪽 끝에 위치한 노드
	//   = 중위순회[inorder]시 제일 처음 출력될 노드
	//   = 제일 작은 노드
	static TTreeNode* FindSmallestNode(TTreeNode* node) {
		TTreeNode* pCur = node;
		for (;;) {
			if (pCur->Left == nullptr)
				break;
			pCur = pCur->Left;
		}
		return pCur;
	}

	// node 서브트리 기준으로 제일 오른쪽 끝에 위치한 노드
	//   = 역방향 중위순회시[inorder] 제일 처음 출력될 노드
	//   = 제일 큰 노드
	static TTreeNode* FindBiggestNode(TTreeNode* node) {
		TTreeNode* pCur = node;
		for (;;) {
			if (pCur->Right == nullptr)
				break;
			pCur = pCur->Right;
		}
		return pCur;
	}

	// node의 좌측 서브트리에서 가장 큰 노드
	static TTreeNode* FindPredecessorNode(TTreeNode* node) {
		TTreeNode* pCur = node->Left;

		while (pCur != nullptr) {
			if (pCur->Right == nullptr) {
				return pCur;
			}
			pCur = pCur->Right;
		}

		return pCur;
	}

	// node의 우측 서브트리에서 가장 큰 노드
	static TTreeNode* FindSuccessorNode(TTreeNode* node) {
		TTreeNode* pCur = node->Right;

		while (pCur != nullptr) {
			if (pCur->Left == nullptr) {
				return pCur;
			}
			pCur = pCur->Left;
		}

		return pCur;
	}

	// node보다 1단계 더 큰 노드를 반환한다. 없을 경우 nullptr
	static TTreeNode* FindBiggerNode(TTreeNode* node) {
		TTreeNode* pBigger = TTreeMap::FindSuccessorNode(node);

		if (pBigger == nullptr) {
			TTreeNode* pParent = node->Parent;

			if (pParent && pParent->Left == node)
				pBigger = pParent;
			else {
				// 연속 우측 분기가 종료되는 지점의 부모가 다음 큰 수이다.
				while (pParent) {
					TTreeNode* pNextParent = pParent->Parent;
					if (pNextParent && pNextParent->Left == pParent) {
						pBigger = pNextParent;
						break;
					}
					pParent = pNextParent;
				}
			}

		}

		return pBigger;
	}

	// node보다 1단계 더 작은 노드를 반환한다. 없을 경우 nullptr
	static TTreeNode* FindSmallerNode(TTreeNode* node) {
		TTreeNode* pSmaller = TTreeMap::FindPredecessorNode(node);

		if (pSmaller == nullptr) {
			TTreeNode* pParent = node->Parent;
			if (pParent && pParent->Right == node)
				pSmaller = pParent;
			else {
				// 연속 좌측 분기가 종료되는 지점의 부모가 다음 큰 수이다.
				while (pParent) {
					TTreeNode* pNextParent = pParent->Parent;
					if (pNextParent && pNextParent->Right == pParent) {
						pSmaller = pNextParent;
						break;
					}
					pParent = pNextParent;
				}
			}
		}

		return pSmaller;
	}

	template <TraverseValueType ValueType, typename Consumer>
	static void InorderTraverseForEach(TTreeNode* node, Consumer&& consumer) {
		if (node == nullptr) return;
		InorderTraverseForEach<ValueType>(node->Left, Forward<Consumer>(consumer));
		if constexpr (ValueType == TraverseValueType::Pair)
			consumer(node->Pair);
		else if constexpr (ValueType == TraverseValueType::Key)
			consumer(node->Pair.Key);
		else if constexpr (ValueType == TraverseValueType::Value)
			consumer(node->Pair.Value);
		else
			DebugAssert(false);
		InorderTraverseForEach<ValueType>(node->Right, Forward<Consumer>(consumer));
	}

	template <TraverseValueType ValueType, typename Consumer>
	static void InorderTraverseReverseForEach(TTreeNode* node, Consumer&& consumer) {
		if (node == nullptr) return;
		InorderTraverseForEach<ValueType>(node->Right, Forward<Consumer>(consumer));
		if constexpr (ValueType == TraverseValueType::Pair)
			consumer(node->Pair);
		else if constexpr (ValueType == TraverseValueType::Key)
			consumer(node->Pair.Key);
		else if constexpr (ValueType == TraverseValueType::Value)
			consumer(node->Pair.Value);
		else
			DebugAssert(false);
		InorderTraverseForEach<ValueType>(node->Left, Forward<Consumer>(consumer));
	}

	static void DeleteNodeRecursive(TTreeNode* node) {
		if (node == nullptr) return;
		DeleteNodeRecursive(node->Left);
		DeleteNodeRecursive(node->Right);
		JCORE_ALLOCATOR_STATIC_DEALLOCATE_AND_DESTROY_SAFE(TTreeNode, node);
	}
	static void MaxHeightRecursive(TTreeNode* node, int height, int& maxHeight) {
		if (node == nullptr) {
			maxHeight = Math::Max(maxHeight, height);
			return;
		}

		MaxHeightRecursive(node->Left, height + 1, maxHeight);
		MaxHeightRecursive(node->Right, height + 1, maxHeight);
	}

	static void ConnectPredecessorChildToParent(TTreeNode* predecessor, TTreeNode* predecessorLeftChild) {

		if (predecessor->IsRight()) {
			predecessor->Parent->Right = predecessorLeftChild;
			predecessorLeftChild->Parent = predecessor->Parent;
			return;
		}

		predecessor->Parent->Left = predecessorLeftChild;
		predecessorLeftChild->Parent = predecessor->Parent;
	}

	TTreeNode* m_pRoot;

	inline static TKyComparator ms_KeyComparator;
public:
	struct TreeMapKeyCollection : public TKeyCollection
	{
		using TEnumerator		= SharedPtr<Iterator<TKey, TAllocator>>;
		using TCollection		= Collection<TKey, TAllocator>;

		TreeMapKeyCollection(TTreeMap* hashMap) : TKeyCollection(hashMap) {
			m_pTreeMap = hashMap;
		}

		TreeMapKeyCollection& operator=(const TreeMapKeyCollection& other) {
			this->m_pTreeMap = other.m_pTreeMap;
			this->m_pMap = other.m_pTreeMap;
			return *this;
		}

		virtual ~TreeMapKeyCollection() noexcept override = default;

		int Size() const override {
			return TKeyCollection::Size();
		}

		bool IsEmpty() const override {
			return TKeyCollection::IsEmpty();
		}

		TEnumerator Begin() const override { return MakeShared<TreeMapKeyCollectionIterator, TAllocator>(m_pTreeMap->GetOwner(), TTreeMap::FindSmallestNode(m_pTreeMap->m_pRoot)); }
		TEnumerator End() const override { return MakeShared<TreeMapKeyCollectionIterator, TAllocator>(m_pTreeMap->GetOwner(), TTreeMap::FindBiggestNode(m_pTreeMap->m_pRoot)); }

		ContainerType GetContainerType() override { return ContainerType::TreeMapKeyCollection; }

		TTreeMap* m_pTreeMap;
	};

	struct TreeMapKeyCollectionIterator final : public TKeyCollectionIterator
	{
		TreeMapKeyCollectionIterator(VoidOwner& owner, TTreeNode* iteratorNode)
			: TKeyCollectionIterator(owner, &m_TreeMapIterator)
			, m_TreeMapIterator(owner, iteratorNode)
		{}
		virtual ~TreeMapKeyCollectionIterator() noexcept = default;
		TTreeMapIterator m_TreeMapIterator;
	};

	struct TreeMapValueCollection final : public TValueCollection
	{
		using TEnumerator		= SharedPtr<Iterator<TValue, TAllocator>>;
		using TCollection		= Collection<TValue, TAllocator>;

		TreeMapValueCollection(TTreeMap* treeMap) : TMapCollection::ValueCollection(treeMap) {
			m_pTreeMap = treeMap;
		}

		virtual ~TreeMapValueCollection() noexcept override = default;

		TreeMapValueCollection& operator=(const TreeMapValueCollection& other) {
			this->m_pTreeMap = other.m_pTreeMap;
			this->m_pMap = other.m_pTreeMap;
			return *this;
		}


		TEnumerator Begin() const override { return MakeShared<TreeMapValueCollectionIterator, TAllocator>(m_pTreeMap->GetOwner(), TTreeMap::FindSmallestNode(m_pTreeMap->m_pRoot)); }
		TEnumerator End() const override { return MakeShared<TreeMapValueCollectionIterator, TAllocator>(m_pTreeMap->GetOwner(), TTreeMap::FindBiggestNode(m_pTreeMap->m_pRoot)); }
		ContainerType GetContainerType() override { return ContainerType::TreeMapValueCollection; }
		TTreeMap* m_pTreeMap;
	};

	struct TreeMapValueCollectionIterator final : public TValueCollectionIterator
	{
		TreeMapValueCollectionIterator(VoidOwner& owner, TTreeNode* iteratorNode)
			: TValueCollectionIterator(owner, &m_TreeMapIterator)
			, m_TreeMapIterator(owner, iteratorNode)
		{}
		virtual ~TreeMapValueCollectionIterator() noexcept override = default;
		TTreeMapIterator m_TreeMapIterator;
	};


	friend class TTreeMapIterator;
	friend struct TreeMapKeyCollection;
	friend struct TreeMapKeyCollectionIterator;
	friend struct TreeMapValueCollection;
	friend struct TreeMapValueCollectionIterator;
}; // class TreeMap<TKey, TValue>

NS_JC_END

