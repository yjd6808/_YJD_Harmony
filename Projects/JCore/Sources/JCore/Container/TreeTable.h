/*
 * 작성자: 윤정도
 * 생성일: 8/14/2023 4:01:45 PM
 * =====================
 * TreeMap과 TreeSet 구현을 위해
 * 이 둘을 통합하기 위한 TreeTable을 구현한다.
 *
 * 이 헤더파일을 직접적으로 Include해서 사용할 필요는 없다.
 * TreeMap.h 또는 TreeSet.h을 Include해서 사용할 것
 */

#pragma once

#include <JCore/Comparator.h>
#include <JCore/Define.h>
#include <JCore/Math.h>

#include <JCore/Container/TreeTable_Node.h>

NS_JC_BEGIN

enum class ETreeTableImplementation
{
    AVLTree,
    RedBlackTree
};

enum class RotateMode
{
    LL,
    RR
};

// Constant Specialization 수행을 위해 어쩔수없이 인자 갯수를 2개로 고정함.
template <typename Pack, ETreeTableImplementation Implementation>
class TreeTable;

#pragma region TreeTable<TKey> - 레드블랙트리 기반 구현

template <template <typename...> typename Pack, typename TKey, typename TKeyComparator, typename TAllocator>
class TreeTable<Pack<TKey, TKeyComparator, TAllocator>, ETreeTableImplementation::RedBlackTree>
{
    static_assert(IsComparator_v<TKeyComparator>, "... TKeyComparator is not comparator");
    /*
     * 트리맵은 이터레이션을 어떻게 수행해야할까? (생각의 흐름)
     *  -> 우선 중위순회(inorder traverse)와 역방향 중위순회(reverse inorder
     * traverse)를 활용해야 할 것이다.
     *  -> 현재 노드기준에서 다음 노드로 이동할려고 한다고 가정해보자.
     *	-> 이때 1단계 더 큰 노드가 다음 노드가 될 것이다.
     *	-> 1단계 더 큰 노드는 어떤 노드일까?
     *	-> A라는 노드가 있을 때 A노드 기준 우측 서브트리에서 가장 작은
     * 노드(Successor)가 1단계 큰 노드가 될 수 있다.
     *    - 1. 만약 A의 우측 서브트리가 존재하지 않고, A의 부모가 있을 때 A가 좌측
     * 자식인 경우 A의 부모가 1단계 큰 노드가 될 수 있다.
     *      - 2. 만약 A의 우측 서버트리가 존재하지 않고, A의 부모가 있을 때 A가
     * 우측 자식인 경우 연속 우측 분기가 종료되는 지점의 노드의 부모가 1단계 더 큰
     * 노드이다. 우측자식인 경우는 이해가 잘 안될 수 있어서 예시 자료를 첨부한다.
     *           https://drive.google.com/file/d/1rpboZM-cm4NS0VkQFhEF934L2OsFKmes/view?usp=sharing
     *			 150의 연속 우측 분기가 종료되는 지점은 200
     *			 275의 연속 우측 분기가 종료되는 지점은 300
     *			 375의 연속 우측 분기가 종료되는 지점은 400이된다.
     *
     *	-> 이 2가지 조건에 만족하는 노드가 없다면 A는 해당 트리에서 가장 큰
     * 노드라는 것이다.
     *  -> 이 생각의 흐름을 토대로 함수: FindBiggerNode를 정의해보자.
     */

  public:
    using TKyComparator = TKeyComparator;
    using TTreeNode = TreeNode<TKey>;
    using TTreeTable = TreeTable<Pack<TKey, TKeyComparator, TAllocator>, ETreeTableImplementation::RedBlackTree>;

    TreeTable() : pRoot_(nullptr), size_(0)
    {
    }

    TreeTable(const TTreeTable& _other) : TreeTable()
    {
        operator=(_other);
    }

    TreeTable(TTreeTable&& _other) noexcept : TreeTable()
    {
        operator=(Move(_other));
    }

    TreeTable(std::initializer_list<TKey> _ilist) : TreeTable()
    {
        operator=(_ilist);
    }

    ~TreeTable() noexcept
    {
        Clear();
    }

  public:
    TTreeTable& operator=(const TTreeTable& _other)
    {
        Clear();
        InorderTraverseForEach(_other.pRoot_, [this](auto& data) { Insert(data); });
        this->size_ = _other.size_;
        return *this;
    }

    TTreeTable& operator=(TTreeTable&& _other) noexcept
    {
        Clear();

        this->pRoot_ = _other.pRoot_;
        this->size_ = _other.size_;

        _other.pRoot_ = nullptr;
        _other.size_ = 0;

        return *this;
    }

    TTreeTable& operator=(std::initializer_list<TKey> _ilist)
    {
        Clear();

        for(auto it = _ilist.begin(); it != _ilist.end(); ++it)
        {
            Insert((*it));
        }

        return *this;
    }

    template <typename Ky>
    bool Insert(Ky&& _key)
    {
        TTreeNode* pNewNode;

        // 1. 데이터를 먼저 넣는다.
        if(pRoot_ == nullptr)
        {
            pNewNode = pRoot_ = TAllocator::template AllocateInitStatic<TTreeNode>(Forward<Ky>(_key));
        }
        else
        {
            // data가 삽입될 부모 노드를 찾는다.
            TTreeNode* pParent = FindParentDataInserted(_key);

            if(pParent == nullptr)
            {
                return false;
            }

            pNewNode = TAllocator::template AllocateInitStatic<TTreeNode>(Forward<Ky>(_key));
            pNewNode->pParent_ = pParent;

            const int comp = KeyComparator(pNewNode->data_, pParent->data_);
            if(comp > 0)
            {
                pParent->pRight_ = pNewNode;
            }
            else if(comp < 0)
            {
                pParent->pLeft_ = pNewNode;
            }
            else
            {
                DebugAssert(false);
            }
        }

        // 2. 삽입된 노드를 기준으로 레드블랙트리가 위반되는지 확인하여 바로잡는다.
        InsertFixup(pNewNode);
        this->size_ += 1;
        return true;
    }

    bool Exist(const TKey& _key) const
    {
        return FindNode(_key) != nullptr;
    }

    bool Remove(const TKey& _key)
    {
        TTreeNode* pDelNode = FindNode(_key);

        if(pDelNode == nullptr)
        {
            return false;
        }

        RemoveByNode(pDelNode);
        return true;
    }

    void Clear() noexcept
    {
        DeleteNodeRecursive(pRoot_);
        pRoot_ = nullptr;
        this->size_ = 0;
    }

    int MaxHeight() const
    {
        int iMaxHeight = 0;
        MaxHeightRecursive(pRoot_, 1, iMaxHeight);
        return iMaxHeight;
    }

    TKey* LowerBound(const TKey& _key) const
    {
        TTreeNode* pNode = LowerBoundNode(pRoot_, _key);
        if(pNode == nullptr)
            return nullptr;
        return &pNode->data_;
    }

    TKey* UpperBound(const TKey& _key) const
    {
        TTreeNode* pNode = UpperBoundNode(pRoot_, _key);
        if(pNode == nullptr)
            return nullptr;
        return &pNode->data_;
    }

    // ==========================================
    // 동적할당 안하고 트리맵 순회할 수 있도록 기능 구현
    // ==========================================
    template <typename Consumer>
    void ForEach(Consumer&& _consumer)
    {
        InorderTraverseForEach(pRoot_, Forward<Consumer>(_consumer));
    }

    void ForEachDelete()
    {
        if constexpr(!IsPointerType_v<TKey>)
        {
            DebugAssert(false);
        }
        else
        {
            InorderTraverseForEach(pRoot_, [](TKey key) { delete key; });
        }
    }

    bool TryGetFirst(JCORE_OUT TKey& _data) const
    {
        if(pRoot_ == nullptr)
        {
            return false;
        }

        TTreeNode* pFirst = FindSmallestNode(pRoot_);
        if(pFirst == nullptr)
        {
            return false;
        }

        _data = pFirst->data_;
        return true;
    }

    bool TryGetLast(JCORE_OUT TKey& _data) const
    {
        if(pRoot_ == nullptr)
        {
            return false;
        }

        TTreeNode* pLast = FindBiggestNode(pRoot_);
        if(pLast == nullptr)
        {
            return false;
        }

        _data = pLast->data_;
        return true;
    }

    bool IsEmpty() const
    {
        return size_ == 0;
    }
    int Size() const
    {
        return size_;
    }

  protected:
    TTreeNode* FindNode(const TKey& _key) const
    {
        TTreeNode* pCur = pRoot_;

        while(pCur != nullptr)
        {
            if(KeyComparator(_key, pCur->data_) == 0)
            {
                return pCur;
            }

            if(KeyComparator(_key, pCur->data_) > 0)
            {
                pCur = pCur->pRight_;
            }
            else
            {
                pCur = pCur->pLeft_;
            }
        }

        return nullptr;
    }

    // 실제로 삭제되어야하는 노드를 반환
    TTreeNode* RemoveByNode(TTreeNode* _delNode)
    {

        // 자식이 없는 경우 그냥 바로 제거 진행
        int iCount = 0;
        TTreeNode* pChild = _delNode->AnyWithChildrenCount(iCount);

        if(iCount == 2)
        {
            // 자식이 둘 다 있는 경우
            TTreeNode* pPredecessor = FindBiggestNode(_delNode->pLeft_);

            // 전임자는 값을 이동해주고 전임자의 자식을 전임자의 부모와 다시
            // 이어줘야한다.
            _delNode->data_ = Move(pPredecessor->data_);

            if(pPredecessor->pLeft_)
                ConnectPredecessorChildToParent(pPredecessor, pPredecessor->pLeft_);

            // 전임자가 실제로 삭제될 노드이다.
            _delNode = pPredecessor;
        }
        else if(iCount == 1)
        {
            // 자식이 한쪽만 있는 경우
            TTreeNode* pParent = _delNode->pParent_;
            pChild->pParent_ = pParent;

            // 삭제되는 노드의 부모가 있을 경우, 삭제되는 노드의 자식과 부모를 올바른
            // 위치로 연결해준다.
            if(pParent)
            {
                if(pParent->pLeft_ == _delNode)
                    pParent->pLeft_ = pChild;
                else
                    pParent->pRight_ = pChild;
            }
            else
            {
                // delNode의 부모가 없다는 말은
                //  => delNode = 루트라는 뜻이므로, 자식을 루트로 만들어준다.
                pRoot_ = pChild;
            }
        }

        RemoveFixup(_delNode);
        DeleteNode(_delNode);
        this->size_ -= 1;
        return _delNode;
    }

    // data가 삽입될 부모를 찾는다.
    TTreeNode* FindParentDataInserted(const TKey& _key)
    {
        TTreeNode* pParent = nullptr;
        TTreeNode* pCur = pRoot_;

        while(pCur != nullptr)
        {
            pParent = pCur;

            const int comp = KeyComparator(_key, pCur->data_);
            if(comp > 0)
            {
                pCur = pCur->pRight_;
            }
            else if(comp < 0)
            {
                pCur = pCur->pLeft_;
            }
            else
            {
                return nullptr;
            }
        }

        return pParent;
    }

    // 삽입 위반 수정
    void InsertFixup(TTreeNode* _child)
    {

        // (1) 루트 노드는 Black이다.
        if(_child == pRoot_)
        {
            _child->color_ = TreeNodeColor::eBlack;
            return;
        }

        TTreeNode* pParent = _child->pParent_; // (1)에서 종료되지 않았다면 무조건 부모가 존재함.
        char eParentColor = pParent->color_;

        /*  (2) Red 노드의 자식은 Black이어야한다.
         *  만약 자식과 부모가 색상이 모두 빨간색이 아닌 경우 더이상 검사할 필요가
         * 없다. 조상님이 없는 경우, 즉 pParent가 루트 노드인 경우 루트 노드는
         * 무조건 Black이고 새로 삽입된 노드는 Red이므로 트리 높이가 2일때는 항상
         * RB트리의 모든 조건에 만족한다.
         *   => 따라서 InsertFixup 수행시 아무것도 할게 없다.
         *
         *     5    root = parent (black)          5         root = parent (black)
         *   1	 ?	child (red)                  ?   10		 child
         * (red)
         *
         */
        if(eParentColor != TreeNodeColor::eRed || _child->color_ != TreeNodeColor::eRed)
        {
            return;
        }

        // 노드 깊이(트리 높이)가 2인 경우는 모두 위 IF문에서 걸러지므로 이후로
        // GrandParent가 nullptr일 수 없다.
        TTreeNode* pGrandParent = pParent->pParent_;
        TTreeNode* pUncle = nullptr; // 삼촌 노드정보 (부모가 조상님의 왼쪽자식인
                                     // 경우 조상님의 오른쪽 자식이 삼촌 노드)
        if(pGrandParent != nullptr)
        {
            if(pGrandParent->pLeft_ == pParent)
                pUncle = pGrandParent->pRight_;
            else
                pUncle = pGrandParent->pLeft_;
        }
        DebugAssertMsg(pGrandParent, "그랜드 부모가 NULL입니다.");
        const char eUncleColor = pUncle ? pUncle->color_ : TreeNodeColor::eBlack; // 삼촌 노드는 있을 수도 없을 수도
                                                                                  // 있고. NIL 노드는 Black이다.

        // Case 1
        if(eUncleColor == TreeNodeColor::eBlack)
        {
            if(pParent->IsLeft())
            {
                if(_child->IsLeft())
                {
                    // Case 1-1
                    pGrandParent->color_ = TreeNodeColor::eRed;
                    pParent->color_ = TreeNodeColor::eBlack;
                    RotateLL(pGrandParent);
                }
                else
                {
                    // Case 1-3
                    RotateRR(pParent);
                    InsertFixup(pParent);
                }
            }
            else
            {
                if(_child->IsRight())
                {
                    // Case 1-2
                    pGrandParent->color_ = TreeNodeColor::eRed;
                    pParent->color_ = TreeNodeColor::eBlack;
                    RotateRR(pGrandParent);
                }
                else
                {
                    // Case 1-4
                    RotateLL(pParent);
                    InsertFixup(pParent);
                }
            }
            return;
        }

        // Case 2: 삼촌 노드가 Red일 경우
        pUncle->color_ = TreeNodeColor::eBlack;
        pParent->color_ = TreeNodeColor::eBlack;
        pGrandParent->color_ = TreeNodeColor::eRed;
        InsertFixup(pGrandParent);
    }

    void DeleteNode(TTreeNode* _pNode)
    {
        if(_pNode == pRoot_)
        {
            JCORE_ALLOCATOR_STATIC_DEALLOCATE_AND_DESTROY_SAFE(TTreeNode, pRoot_);
            return;
        }

        if(_pNode->pParent_)
        {
            if(_pNode->pParent_->pLeft_ == _pNode)
                _pNode->pParent_->pLeft_ = nullptr;
            else if(_pNode->pParent_->pRight_ == _pNode) // 부유 상태의 node일 수 있으므로 무조건 체크
                _pNode->pParent_->pRight_ = nullptr;
        }

        JCORE_ALLOCATOR_STATIC_DEALLOCATE_AND_DESTROY_SAFE(TTreeNode, _pNode);
    }

    // 삭제 위반 수정
    void RemoveFixup(TTreeNode* _child)
    {

        if(_child->color_ == TreeNodeColor::eRed)
        {
            return;
        }

        // [1. 삭제될 노드가 자식이 1개 경우]
        TTreeNode* pChild = _child->Any();
        if(pChild)
        {
            // 케이스 1. 자식이 한개만 있는경우 (이 자식은 무조건 Red일 것이다.)
            DebugAssertMsg(_child->Count() == 1, "1. 삭제될 노드에 자식이 1개만 있어야하는데 2개 있습니다.");
            DebugAssert(_child->color_ == TreeNodeColor::eBlack);
            DebugAssert(pChild->color_ == TreeNodeColor::eRed);
            pChild->color_ = TreeNodeColor::eBlack;
            return;
        }

        if(_child == pRoot_)
        {
            return;
        }

        RemoveFixupExtraBlack(_child);
    }

    // 엑스트라 Black 속성이 부여된 노드를 대상으로 위반 수정
    // 난 엑스트라 Black 속성이 이 함수에 들어온 것 자체로 부여되었다는 걸로
    // 간주하기로 함.
    void RemoveFixupExtraBlack(TTreeNode* _child)
    {

        if(pRoot_ == _child)
        {
            // 루트는 엑스트라 Black속성이 부여될 경우 없애기만 하면 됨.
            //	난 엑스트라 Black이라는 추가 정보를 굳이 노드에 담아서 표현할
            // 필요 없다고 생각한다. 	삭제중 일시적으로 존재하는 속성이기 떄문이다.
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

            TreeNodeFamily(TTreeNode* _child)
            {
                const bool bRightChild = _child->IsRight();
                Parent = _child->pParent_; // 부모 노드
                DebugAssertMsg(Parent, "부모노드 없을 수 없습니다.");

                Sibling = bRightChild ? Parent->pLeft_ : Parent->pRight_; // 형제 노드 (child가 우측이면 부모의
                                                                          // 왼쪽 노드가 형제 노드)
                DebugAssertMsg(Sibling, "형제노드가 없을 수 없습니다.");

                const TTreeNode* pNephewLine = nullptr; // 조카 노드 (일렬로 나열)
                const TTreeNode* pNephewTri = nullptr;  // 조카 노드 (꺽여서 나열)

                if(Sibling->IsLeft())
                {
                    NephewLine = Sibling->pLeft_;
                    NephewTri = Sibling->pRight_;
                }
                else
                {
                    NephewLine = Sibling->pRight_;
                    NephewTri = Sibling->pLeft_;
                }

                // 노드가 없는 경우 Black으로 판정토록한다.
                ParentColor = Parent->color_;
                SiblingColor = Sibling->color_;
                NephewTriColor = NephewTri ? NephewTri->color_ : TreeNodeColor::eBlack;
                NephewLineColor = NephewLine ? NephewLine->color_ : TreeNodeColor::eBlack;
            }
        };
#pragma endregion

        const bool bRightChild = _child->IsRight();
        const TreeNodeFamily family(_child);

        // 그룹 케이스 2: 부모의 색이 Black인 경우
        if(family.ParentColor == TreeNodeColor::eBlack)
        {

            // 케이스 5. (형제가 Red인 경우)
            if(family.SiblingColor == TreeNodeColor::eRed)
            {
                family.Parent->color_ = TreeNodeColor::eRed;
                family.Sibling->color_ = TreeNodeColor::eBlack;
                RotateNode(family.Parent, bRightChild ? RotateMode::LL : RotateMode::RR);
                RemoveFixupExtraBlack(_child);
                return;
            }

            // 케이스 1 ~ 4 (형제가 Black인 경우)
            if(family.NephewTriColor == TreeNodeColor::eBlack && family.NephewLineColor == TreeNodeColor::eBlack)
            {
                // 케이스 1. 조카 모두 Black인 경우
                family.Sibling->color_ = TreeNodeColor::eRed;
                RemoveFixupExtraBlack(family.Parent); // Extra Black을 없앨 수 없으므로 부모로 전달
                return;
            }

            if(family.NephewLineColor == TreeNodeColor::eRed)
            {
                // 케이스 2. 라인조카가 Red인 경우
                family.NephewLine->color_ = TreeNodeColor::eBlack;
                RotateNode(family.Parent, bRightChild ? RotateMode::LL : RotateMode::RR);
                return;
            }

            if(family.NephewTriColor == TreeNodeColor::eRed)
            {
                // 케이스 3. 꺽인조카가 Red인 경우
                family.NephewTri->color_ = TreeNodeColor::eBlack;
                family.Sibling->color_ = TreeNodeColor::eRed;
                RotateNode(family.Sibling, bRightChild ? RotateMode::RR : RotateMode::LL);
                RemoveFixupExtraBlack(_child); // 케이스 2로 처리하기위해 재호출
                return;
            }

            return;
        }

        DebugAssertMsg(family.SiblingColor == TreeNodeColor::eBlack, "[그룹 케이스 1] 형제노드가 Black이 아닙니다.");
        // 그룹 케이스 1: 부모의 색이 Red인 경우
        if(family.NephewTriColor == TreeNodeColor::eBlack && family.NephewLineColor == TreeNodeColor::eBlack)
        {
            // 케이스 1. 조카 모두 Black인 경우
            family.Sibling->color_ = TreeNodeColor::eRed;
            family.Parent->color_ = TreeNodeColor::eBlack;
            return;
        }

        if(family.NephewLineColor == TreeNodeColor::eRed)
        {
            // 케이스 2. 라인조카가 Red인 경우

            family.NephewLine->color_ = TreeNodeColor::eBlack;
            family.Sibling->color_ = TreeNodeColor::eRed;
            family.Parent->color_ = TreeNodeColor::eBlack;
            RotateNode(family.Parent, bRightChild ? RotateMode::LL : RotateMode::RR);
            return;
        }

        if(family.NephewTriColor == TreeNodeColor::eRed)
        {
            // 케이스 3. 꺽인조카가 Red인 경우
            family.NephewTri->color_ = TreeNodeColor::eBlack;
            family.Sibling->color_ = TreeNodeColor::eRed;
            RotateNode(family.Sibling, bRightChild ? RotateMode::RR : RotateMode::LL);
            RemoveFixupExtraBlack(_child); // 케이스 2로 처리하기위해 재호출
        }
    }

    void RotateNode(TTreeNode* _pNode, RotateMode _mode)
    {
        switch(_mode)
        {
        case RotateMode::RR:
            RotateRR(_pNode);
            return;
        case RotateMode::LL:
            RotateLL(_pNode);
            return;
        }
    }
    // 노드가 왼쪽/왼쪽으로 붙은 경우
    void RotateLL(TTreeNode* _pNode)
    {
        TTreeNode* pParent = _pNode->pParent_;
        TTreeNode* pCur = _pNode;
        TTreeNode* pChild = _pNode->pLeft_;
        TTreeNode* pChildRight = _pNode->pLeft_->pRight_;

        if(pParent)
        {
            if(pParent->pLeft_ == pCur)
                pParent->pLeft_ = pChild;
            else
                pParent->pRight_ = pChild;
        }
        pChild->pParent_ = pParent;

        pCur->pLeft_ = pChildRight;
        if(pChildRight)
            pChildRight->pParent_ = pCur;

        pChild->pRight_ = pCur;
        pCur->pParent_ = pChild;

        // 회전으로 인한 루트 변경 업데이트
        if(pRoot_ == pCur)
        {
            pRoot_ = pChild;
        }
    }
    // 노드가 오른쪽/오른쪽으로 붙은 경우
    void RotateRR(TTreeNode* _pNode)
    {
        TTreeNode* pParent = _pNode->pParent_;
        TTreeNode* pCur = _pNode;
        TTreeNode* pChild = _pNode->pRight_;
        TTreeNode* pChildLeft = _pNode->pRight_->pLeft_;

        if(pParent)
        {
            if(pParent->pLeft_ == pCur)
                pParent->pLeft_ = pChild;
            else
                pParent->pRight_ = pChild;
        }
        pChild->pParent_ = pParent;

        pCur->pRight_ = pChildLeft;
        if(pChildLeft)
            pChildLeft->pParent_ = pCur;

        pChild->pLeft_ = pCur;
        pCur->pParent_ = pChild;

        // 회전으로 인한 루트 변경 업데이트
        if(pRoot_ == pCur)
        {
            pRoot_ = pChild;
        }
    }

    // node서브트리 기준으로 제일 왼쪽 끝에 위치한 노드
    //   = 중위순회[inorder]시 제일 처음 출력될 노드
    //   = 제일 작은 노드
    static TTreeNode* FindSmallestNode(TTreeNode* _pNode)
    {
        if(_pNode == nullptr)
            return nullptr;

        TTreeNode* pCur = _pNode;
        for(;;)
        {
            if(pCur->pLeft_ == nullptr)
                break;
            pCur = pCur->pLeft_;
        }
        return pCur;
    }

    // node 서브트리 기준으로 제일 오른쪽 끝에 위치한 노드
    //   = 역방향 중위순회시[inorder] 제일 처음 출력될 노드
    //   = 제일 큰 노드
    static TTreeNode* FindBiggestNode(TTreeNode* _pNode)
    {
        if(_pNode == nullptr)
            return nullptr;

        TTreeNode* pCur = _pNode;
        for(;;)
        {
            if(pCur->pRight_ == nullptr)
                break;
            pCur = pCur->pRight_;
        }
        return pCur;
    }

    // node의 좌측 서브트리에서 가장 큰 노드
    static TTreeNode* FindPredecessorNode(TTreeNode* _pNode)
    {
        TTreeNode* pCur = _pNode->pLeft_;

        while(pCur != nullptr)
        {
            if(pCur->pRight_ == nullptr)
            {
                return pCur;
            }
            pCur = pCur->pRight_;
        }

        return pCur;
    }

    // node의 우측 서브트리에서 가장 큰 노드
    static TTreeNode* FindSuccessorNode(TTreeNode* _pNode)
    {
        TTreeNode* pCur = _pNode->pRight_;

        while(pCur != nullptr)
        {
            if(pCur->pLeft_ == nullptr)
            {
                return pCur;
            }
            pCur = pCur->pLeft_;
        }

        return pCur;
    }

    // node보다 1단계 더 큰 노드를 반환한다. 없을 경우 nullptr
    static TTreeNode* FindBiggerNode(TTreeNode* _pNode)
    {
        TTreeNode* pBigger = TTreeTable::FindSuccessorNode(_pNode);

        if(pBigger == nullptr)
        {
            TTreeNode* pParent = _pNode->pParent_;

            if(pParent && pParent->pLeft_ == _pNode)
                pBigger = pParent;
            else
            {
                // 연속 우측 분기가 종료되는 지점의 부모가 다음 큰 수이다.
                while(pParent)
                {
                    TTreeNode* pNextParent = pParent->pParent_;
                    if(pNextParent && pNextParent->pLeft_ == pParent)
                    {
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
    static TTreeNode* FindSmallerNode(TTreeNode* _pNode)
    {
        TTreeNode* pSmaller = TTreeTable::FindPredecessorNode(_pNode);

        if(pSmaller == nullptr)
        {
            TTreeNode* pParent = _pNode->pParent_;
            if(pParent && pParent->pRight_ == _pNode)
                pSmaller = pParent;
            else
            {
                // 연속 좌측 분기가 종료되는 지점의 부모가 다음 큰 수이다.
                while(pParent)
                {
                    TTreeNode* pNextParent = pParent->pParent_;
                    if(pNextParent && pNextParent->pRight_ == pParent)
                    {
                        pSmaller = pNextParent;
                        break;
                    }
                    pParent = pNextParent;
                }
            }
        }

        return pSmaller;
    }

    template <typename Consumer>
    static void InorderTraverseForEach(TTreeNode* _pNode, Consumer&& _consumer)
    {
        if(_pNode == nullptr)
            return;
        InorderTraverseForEach(_pNode->pLeft_, Forward<Consumer>(_consumer));
        _consumer(_pNode->data_);
        InorderTraverseForEach(_pNode->pRight_, Forward<Consumer>(_consumer));
    }

    template <typename Consumer>
    static void InorderTraverseReverseForEach(TTreeNode* _pNode, Consumer&& _consumer)
    {
        if(_pNode == nullptr)
            return;
        InorderTraverseForEach(_pNode->pRight_, Forward<Consumer>(_consumer));
        _consumer(_pNode->data_);
        InorderTraverseForEach(_pNode->pLeft_, Forward<Consumer>(_consumer));
    }

    static void DeleteNodeRecursive(TTreeNode* _pNode)
    {
        if(_pNode == nullptr)
            return;
        DeleteNodeRecursive(_pNode->pLeft_);
        DeleteNodeRecursive(_pNode->pRight_);
        JCORE_ALLOCATOR_STATIC_DEALLOCATE_AND_DESTROY_SAFE(TTreeNode, _pNode);
    }
    static void MaxHeightRecursive(TTreeNode* _pNode, int _height, int& _maxHeight)
    {
        if(_pNode == nullptr)
        {
            _maxHeight = Math::Max(_maxHeight, _height);
            return;
        }

        MaxHeightRecursive(_pNode->pLeft_, _height + 1, _maxHeight);
        MaxHeightRecursive(_pNode->pRight_, _height + 1, _maxHeight);
    }

    static void ConnectPredecessorChildToParent(TTreeNode* _predecessor, TTreeNode* _predecessorLeftChild)
    {

        if(_predecessor->IsRight())
        {
            _predecessor->pParent_->pRight_ = _predecessorLeftChild;
            _predecessorLeftChild->pParent_ = _predecessor->pParent_;
            return;
        }

        _predecessor->pParent_->pLeft_ = _predecessorLeftChild;
        _predecessorLeftChild->pParent_ = _predecessor->pParent_;
    }

    // node를 root로 하는 트리에서의 key보다 처음으로 같거나 커지는 노드
    static TTreeNode* LowerBoundNode(TTreeNode* _rootNode, const TKey& _key)
    {
        if(_rootNode == nullptr)
            return nullptr;

        TTreeNode* pHigh = FindBiggestNode(_rootNode);
        TTreeNode* pCur = _rootNode;

        while(pCur)
        {
            const int comp = KeyComparator(_key, pCur->data_);
            if(comp > 0)
            {
                pCur = pCur->pRight_;
            }
            else
            {
                pHigh = pCur;
                pCur = pCur->pLeft_;
            }
        }

        if(KeyComparator(_key, pHigh->data_) > 0)
            return nullptr;

        return pHigh;
    }

    // node를 root로 하는 트리에서의 key보다 처음으로 커지는 노드
    static TTreeNode* UpperBoundNode(TTreeNode* _rootNode, const TKey& _key)
    {
        if(_rootNode == nullptr)
            return nullptr;

        TTreeNode* pHigh = FindBiggestNode(_rootNode);
        TTreeNode* pCur = _rootNode;

        while(pCur)
        {
            const int comp = KeyComparator(_key, pCur->data_);
            if(comp >= 0)
            {
                pCur = pCur->pRight_;
            }
            else
            {
                pHigh = pCur;
                pCur = pCur->pLeft_;
            }
        }

        if(KeyComparator(_key, pHigh->data_) >= 0)
            return nullptr;

        return pHigh;
    }

    TTreeNode* pRoot_;
    int size_;

    inline static TKyComparator KeyComparator;

    template <typename, typename, typename, ETreeTableImplementation> friend class TreeSet;
    template <typename, typename, typename, ETreeTableImplementation> friend class TreeSetIterator;
}; // class CTreeTable<Pack<TKey, TKeyComparator, TAllocator>,
   // ETreeTableImplementation::RedBlackTree>

#pragma endregion

#pragma region TreeTable<TKey, TValue> - 레드블랙트리 기반 구현
template <template <typename...> typename Pack, typename TKey, typename TValue, typename TKeyComparator, typename TAllocator>
class TreeTable<Pack<TKey, TValue, TKeyComparator, TAllocator>, ETreeTableImplementation::RedBlackTree>
{
    static_assert(IsComparator_v<TKeyComparator>, "... TKeyComparator is not comparator");

    enum class TraverseValueType
    {
        Pair,
        Key,
        Value
    };

  public:
    using TKyComparator = TKeyComparator;
    using TKeyValuePair = Pair<TKey, TValue>;
    using TTreeNode = TreeNode<TKeyValuePair>;
    using TTreeTable = TreeTable<Pack<TKey, TValue, TKeyComparator, TAllocator>, ETreeTableImplementation::RedBlackTree>;

    TreeTable() : pRoot_(nullptr), size_(0)
    {
    }

    TreeTable(const TTreeTable& other) : TreeTable()
    {
        operator=(other);
    }

    TreeTable(TTreeTable&& other) noexcept : TreeTable()
    {
        operator=(Move(other));
    }

    TreeTable(std::initializer_list<TKeyValuePair> ilist) : TreeTable()
    {
        operator=(ilist);
    }

    ~TreeTable() noexcept
    {
        Clear();
    }

    TTreeTable& operator=(const TTreeTable& other)
    {
        Clear();
        InorderTraverseForEach<TraverseValueType::Pair>(other.pRoot_, [this](auto& pair) { Insert(pair.key_, pair.value_); });
        this->size_ = other.size_;
        return *this;
    }

    TTreeTable& operator=(TTreeTable&& other) noexcept
    {
        Clear();

        this->pRoot_ = other.pRoot_;
        this->size_ = other.size_;

        other.pRoot_ = nullptr;
        other.size_ = 0;

        return *this;
    }

    TTreeTable& operator=(std::initializer_list<TKeyValuePair> ilist)
    {
        Clear();

        for(auto it = ilist.begin(); it != ilist.end(); ++it)
        {
            Insert(Move((*it)));
        }

        return *this;
    }

    TValue& operator[](const TKey& key)
    {
        return Get(key);
    }

    template <typename Ky, typename Vy>
    bool Insert(Ky&& key, Vy&& value)
    {
        TTreeNode* pNewNode;

        // 1. 데이터를 먼저 넣는다.
        if(pRoot_ == nullptr)
        {
            pNewNode = pRoot_ = TAllocator::template AllocateInitStatic<TTreeNode>(TKeyValuePair{Forward<Ky>(key), Forward<Vy>(value)});
        }
        else
        {
            // data가 삽입될 부모 노드를 찾는다.
            TTreeNode* pParent = FindParentDataInserted(key);

            if(pParent == nullptr)
            {
                return false;
            }

            pNewNode = TAllocator::template AllocateInitStatic<TTreeNode>(TKeyValuePair{Forward<Ky>(key), Forward<Vy>(value)});
            pNewNode->pParent_ = pParent;

            // key, value가 pNewNode를 생성할 때 포워딩되기 때문에 만약 rvalue로
            // 들어올 경우 잘못된 결과를 얻을 수 있다. 따라서 key 대신
            // pNewNode->data_.key_를 사용해야함.
            const int comp = KeyComparator(pNewNode->data_.key_, pParent->data_.key_);
            if(comp > 0)
            {
                pParent->pRight_ = pNewNode;
            }
            else if(comp < 0)
            {
                pParent->pLeft_ = pNewNode;
            }
            else
            {
                DebugAssert(false);
            }
        }

        // 2. 삽입된 노드를 기준으로 레드블랙트리가 위반되는지 확인하여 바로잡는다.
        InsertFixup(pNewNode);
        this->size_ += 1;
        return true;
    }

    bool Insert(const TKeyValuePair& pair)
    {
        return Insert(pair.key_, pair.value_);
    }

    bool Insert(TKeyValuePair&& pair)
    {
        return Insert(Move(pair.key_), Move(pair.value_));
    }

    bool Exist(const TKey& key) const
    {
        return FindNode(key) != nullptr;
    }

    TValue* Find(const TKey& key) const
    {
        TTreeNode* pFind = FindNode(key);

        if(pFind == nullptr)
        {
            return nullptr;
        }

        return &pFind->data_.value_;
    }

    TValue& Get(const TKey& key) const
    {
        TTreeNode* pFind = FindNode(key);

        if(pFind == nullptr)
        {
            throw InvalidArgumentException("해당 키값에 대응하는 값이 존재하지 않습니다.");
        }

        return pFind->data_.value_;
    }

    // 실제 삭제되는 노드가 달라질 수 있어서 이터레이터를 올바로 재설정해줘야한다.

    bool Remove(const TKey& key)
    {
        TTreeNode* pDelNode = FindNode(key);

        if(pDelNode == nullptr)
        {
            return false;
        }

        RemoveByNode(pDelNode);
        return true;
    }

    void Clear() noexcept
    {
        DeleteNodeRecursive(pRoot_);
        pRoot_ = nullptr;
        this->size_ = 0;
    }

    int MaxHeight() const
    {
        int iMaxHeight = 0;
        MaxHeightRecursive(pRoot_, 1, iMaxHeight);
        return iMaxHeight;
    }

    TKeyValuePair* LowerBoundPair(const TKey& key) const
    {
        TTreeNode* pNode = LowerBoundNode(pRoot_, key);
        if(pNode == nullptr)
            return nullptr;
        return &pNode->data_;
    }

    TValue* LowerBoundValue(const TKey& key) const
    {
        TTreeNode* pNode = LowerBoundNode(pRoot_, key);
        if(pNode == nullptr)
            return nullptr;
        return &pNode->data_.Value;
    }

    TKeyValuePair* UpperBoundPair(const TKey& key) const
    {
        TTreeNode* pNode = UpperBoundNode(pRoot_, key);
        if(pNode == nullptr)
            return nullptr;
        return &pNode->data_;
    }

    TValue* UpperBoundValue(const TKey& key) const
    {
        TTreeNode* pNode = UpperBoundNode(pRoot_, key);
        if(pNode == nullptr)
            return nullptr;
        return &pNode->data_.Value;
    }

    TKey* UpperBoundKey(const TKey& key) const
    {
        TTreeNode* pNode = UpperBoundNode(pRoot_, key);
        if(pNode == nullptr)
            return nullptr;
        return &pNode->data_.key_;
    }

    // ==========================================
    // 동적할당 안하고 트리맵 순회할 수 있도록 기능 구현
    // ==========================================
    template <typename Consumer>
    void ForEach(Consumer&& consumer)
    {
        InorderTraverseForEach<TraverseValueType::Pair>(pRoot_, Forward<Consumer>(consumer));
    }

    template <typename Consumer>
    void ForEachKey(Consumer&& consumer)
    {
        InorderTraverseForEach<TraverseValueType::Key>(pRoot_, Forward<Consumer>(consumer));
    }

    template <typename Consumer>
    void ForEachValue(Consumer&& consumer)
    {
        InorderTraverseForEach<TraverseValueType::Value>(pRoot_, Forward<Consumer>(consumer));
    }

    void ForEachValueDelete()
    {
        if constexpr(!IsPointerType_v<TValue>)
        {
            DebugAssert(false);
            return;
        }

        InorderTraverseForEach<TraverseValueType::Value>(pRoot_, [](TValue v) { delete v; });
    }

    bool TryGetFirst(JCORE_OUT TKeyValuePair& pair) const
    {
        if(pRoot_ == nullptr)
        {
            return false;
        }

        TTreeNode* pFirst = FindSmallestNode(pRoot_);
        if(pFirst == nullptr)
        {
            return false;
        }

        pair = pFirst->data_;
        return true;
    }

    bool TryGetLast(JCORE_OUT TKeyValuePair& pair) const
    {
        if(pRoot_ == nullptr)
        {
            return false;
        }

        TTreeNode* pLast = FindBiggestNode(pRoot_);
        if(pLast == nullptr)
        {
            return false;
        }

        pair = pLast->data_;
        return true;
    }

    bool TryGetFirstValue(JCORE_OUT TValue& pair) const
    {
        if(pRoot_ == nullptr)
        {
            return false;
        }

        TTreeNode* pFirst = FindSmallestNode(pRoot_);
        if(pFirst == nullptr)
        {
            return false;
        }

        pair = pFirst->data_.value_;
        return true;
    }

    bool TryGetLastValue(JCORE_OUT TValue& pair) const
    {
        if(pRoot_ == nullptr)
        {
            return false;
        }

        TTreeNode* pLast = FindBiggestNode(pRoot_);
        if(pLast == nullptr)
        {
            return false;
        }

        pair = pLast->data_.value_;
        return true;
    }

    bool TryGetFirstKey(JCORE_OUT TKey& key) const
    {
        if(pRoot_ == nullptr)
        {
            return false;
        }

        TTreeNode* pFirst = FindSmallestNode(pRoot_);
        if(pFirst == nullptr)
        {
            return false;
        }

        key = pFirst->data_.key_;
        return true;
    }

    bool IsEmpty() const
    {
        return size_ == 0;
    }
    int Size() const
    {
        return size_;
    }

  protected:
    TTreeNode* FindNode(const TKey& key) const
    {
        TTreeNode* pCur = pRoot_;

        while(pCur != nullptr)
        {
            if(KeyComparator(key, pCur->data_.key_) == 0)
            {
                return pCur;
            }

            if(KeyComparator(key, pCur->data_.key_) > 0)
            {
                pCur = pCur->pRight_;
            }
            else
            {
                pCur = pCur->pLeft_;
            }
        }

        return nullptr;
    }

    // 실제로 삭제되어야하는 노드를 반환
    TTreeNode* RemoveByNode(TTreeNode* delNode)
    {

        // 자식이 없는 경우 그냥 바로 제거 진행
        int iCount = 0;
        TTreeNode* pChild = delNode->AnyWithChildrenCount(iCount);

        if(iCount == 2)
        {
            // 자식이 둘 다 있는 경우
            TTreeNode* pPredecessor = FindBiggestNode(delNode->pLeft_);

            // 전임자는 값을 이동해주고 전임자의 자식을 전임자의 부모와 다시
            // 이어줘야한다.
            delNode->data_ = Move(pPredecessor->data_);

            if(pPredecessor->pLeft_)
                ConnectPredecessorChildToParent(pPredecessor, pPredecessor->pLeft_);

            // 전임자가 실제로 삭제될 노드이다.
            delNode = pPredecessor;
        }
        else if(iCount == 1)
        {
            // 자식이 한쪽만 있는 경우
            TTreeNode* pParent = delNode->pParent_;
            pChild->pParent_ = pParent;

            // 삭제되는 노드의 부모가 있을 경우, 삭제되는 노드의 자식과 부모를 올바른
            // 위치로 연결해준다.
            if(pParent)
            {
                if(pParent->pLeft_ == delNode)
                    pParent->pLeft_ = pChild;
                else
                    pParent->pRight_ = pChild;
            }
            else
            {
                // delNode의 부모가 없다는 말은
                //  => delNode = 루트라는 뜻이므로, 자식을 루트로 만들어준다.
                pRoot_ = pChild;
            }
        }

        RemoveFixup(delNode);
        DeleteNode(delNode);
        this->size_ -= 1;
        return delNode;
    }

    // data가 삽입될 부모를 찾는다.
    TTreeNode* FindParentDataInserted(const TKey& key)
    {
        TTreeNode* pParent = nullptr;
        TTreeNode* pCur = pRoot_;

        while(pCur != nullptr)
        {
            pParent = pCur;

            const int comp = KeyComparator(key, pCur->data_.key_);
            if(comp > 0)
            {
                pCur = pCur->pRight_;
            }
            else if(comp < 0)
            {
                pCur = pCur->pLeft_;
            }
            else
            {
                return nullptr;
            }
        }

        return pParent;
    }

    // 삽입 위반 수정
    void InsertFixup(TTreeNode* child)
    {

        // (1) 루트 노드는 Black이다.
        if(child == pRoot_)
        {
            child->color_ = TreeNodeColor::eBlack;
            return;
        }

        TTreeNode* pParent = child->pParent_; // (1)에서 종료되지 않았다면 무조건 부모가 존재함.
        char eParentColor = pParent->color_;

        /*  (2) Red 노드의 자식은 Black이어야한다.
         *  만약 자식과 부모가 색상이 모두 빨간색이 아닌 경우 더이상 검사할 필요가
         * 없다. 조상님이 없는 경우, 즉 pParent가 루트 노드인 경우 루트 노드는
         * 무조건 Black이고 새로 삽입된 노드는 Red이므로 트리 높이가 2일때는 항상
         * RB트리의 모든 조건에 만족한다.
         *   => 따라서 InsertFixup 수행시 아무것도 할게 없다.
         *
         *     5    root = parent (black)          5         root = parent (black)
         *   1	 ?	child (red)                  ?   10		 child
         * (red)
         *
         */
        if(eParentColor != TreeNodeColor::eRed || child->color_ != TreeNodeColor::eRed)
        {
            return;
        }

        // 노드 깊이(트리 높이)가 2인 경우는 모두 위 IF문에서 걸러지므로 이후로
        // GrandParent가 nullptr일 수 없다.
        TTreeNode* pGrandParent = pParent->pParent_;
        TTreeNode* pUncle = nullptr; // 삼촌 노드정보 (부모가 조상님의 왼쪽자식인
                                     // 경우 조상님의 오른쪽 자식이 삼촌 노드)
        if(pGrandParent != nullptr)
        {
            if(pGrandParent->pLeft_ == pParent)
                pUncle = pGrandParent->pRight_;
            else
                pUncle = pGrandParent->pLeft_;
        }
        DebugAssertMsg(pGrandParent, "그랜드 부모가 NULL입니다.");
        const char eUncleColor = pUncle ? pUncle->color_ : TreeNodeColor::eBlack; // 삼촌 노드는 있을 수도 없을 수도
                                                                                  // 있고. NIL 노드는 Black이다.

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
         *		    Case 1-3 (삼각형 모양) - 5를 RR회전하여 Case 1-1의 모양으로
         * 변환해줘야한다.
         *			----------------------------------------------
         *			       10(B)				<- grandparent
         *			    5(R)	 ?(B)			<- parent, uncle
         * 				   7(R) 				<- child
         *				              ↓ 변환 후
         *			       10(B)				<- grandparent
         *			     7(R)	 ?(B)			<- child, uncle
         *	==> 5(R) ?					<- parent
         *
         *		    Case 1-4 (삼각형 모양) - 5를 RR회전하여 Case 1-1의 모양으로
         * 변환해줘야한다.
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
        if(eUncleColor == TreeNodeColor::eBlack)
        {
            if(pParent->IsLeft())
            {
                if(child->IsLeft())
                {
                    // Case 1-1
                    pGrandParent->color_ = TreeNodeColor::eRed;
                    pParent->color_ = TreeNodeColor::eBlack;
                    RotateLL(pGrandParent);
                }
                else
                {
                    // Case 1-3
                    RotateRR(pParent);
                    InsertFixup(pParent);
                }
            }
            else
            {
                if(child->IsRight())
                {
                    // Case 1-2
                    pGrandParent->color_ = TreeNodeColor::eRed;
                    pParent->color_ = TreeNodeColor::eBlack;
                    RotateRR(pGrandParent);
                }
                else
                {
                    // Case 1-4
                    RotateLL(pParent);
                    InsertFixup(pParent);
                }
            }
            return;
        }

        /*
         * Case 2: 삼촌 노드가 Red일 경우
         *     이경우 Case1보다 훨씬 단순하다. 부모, 삼촌의 색상과 조상님의 색상을
         * 바꿔줌으로써 RB트리 속성 4번이 위배되지 않도록 만든다. 그리고 조상님이
         * Red가 되었기 때문에 조상님의 부모가 마찬가지로 Red일 수가 있으므로
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

        pUncle->color_ = TreeNodeColor::eBlack;
        pParent->color_ = TreeNodeColor::eBlack;
        pGrandParent->color_ = TreeNodeColor::eRed;
        InsertFixup(pGrandParent);
    }

    void DeleteNode(TTreeNode* _pNode)
    {
        if(_pNode == pRoot_)
        {
            JCORE_ALLOCATOR_STATIC_DEALLOCATE_AND_DESTROY_SAFE(TTreeNode, pRoot_);
            return;
        }

        if(_pNode->pParent_)
        {
            if(_pNode->pParent_->pLeft_ == _pNode)
                _pNode->pParent_->pLeft_ = nullptr;
            else if(_pNode->pParent_->pRight_ == _pNode) // 부유 상태의 node일 수 있으므로 무조건 체크
                _pNode->pParent_->pRight_ = nullptr;
        }

        JCORE_ALLOCATOR_STATIC_DEALLOCATE_AND_DESTROY_SAFE(TTreeNode, _pNode);
    }

    // 삭제 위반 수정
    void RemoveFixup(TTreeNode* _child)
    {

        if(_child->color_ == TreeNodeColor::eRed)
        {
            return;
        }

        // [1. 삭제될 노드가 자식이 1개 경우]
        TTreeNode* pChild = _child->Any();
        if(pChild)
        {
            // 케이스 1. 자식이 한개만 있는경우 (이 자식은 무조건 Red일 것이다.)
            DebugAssertMsg(_child->Count() == 1, "1. 삭제될 노드에 자식이 1개만 있어야하는데 2개 있습니다.");
            DebugAssert(_child->color_ == TreeNodeColor::eBlack);
            DebugAssert(pChild->color_ == TreeNodeColor::eRed);
            pChild->color_ = TreeNodeColor::eBlack;
            return;
        }

        if(_child == pRoot_)
        {
            return;
        }

        RemoveFixupExtraBlack(_child);
    }

    // 엑스트라 Black 속성이 부여된 노드를 대상으로 위반 수정
    // 난 엑스트라 Black 속성이 이 함수에 들어온 것 자체로 부여되었다는 걸로
    // 간주하기로 함.
    void RemoveFixupExtraBlack(TTreeNode* _child)
    {

        if(pRoot_ == _child)
        {
            // 루트는 엑스트라 Black속성이 부여될 경우 없애기만 하면 됨.
            //	난 엑스트라 Black이라는 추가 정보를 굳이 노드에 담아서 표현할
            // 필요 없다고 생각한다. 	삭제중 일시적으로 존재하는 속성이기 떄문이다.
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

            TreeNodeFamily(TTreeNode* _child)
            {
                const bool bRightChild = _child->IsRight();
                Parent = _child->pParent_; // 부모 노드
                DebugAssertMsg(Parent, "부모노드 없을 수 없습니다.");

                Sibling = bRightChild ? Parent->pLeft_ : Parent->pRight_; // 형제 노드 (child가 우측이면 부모의
                                                                          // 왼쪽 노드가 형제 노드)
                DebugAssertMsg(Sibling, "형제노드가 없을 수 없습니다.");

                const TTreeNode* pNephewLine = nullptr; // 조카 노드 (일렬로 나열)
                const TTreeNode* pNephewTri = nullptr;  // 조카 노드 (꺽여서 나열)

                if(Sibling->IsLeft())
                {
                    NephewLine = Sibling->pLeft_;
                    NephewTri = Sibling->pRight_;
                }
                else
                {
                    NephewLine = Sibling->pRight_;
                    NephewTri = Sibling->pLeft_;
                }

                // 노드가 없는 경우 Black으로 판정토록한다.
                ParentColor = Parent->color_;
                SiblingColor = Sibling->color_;
                NephewTriColor = NephewTri ? NephewTri->color_ : TreeNodeColor::eBlack;
                NephewLineColor = NephewLine ? NephewLine->color_ : TreeNodeColor::eBlack;
            }
        };
#pragma endregion

        const bool bRightChild = _child->IsRight();
        const TreeNodeFamily family(_child);

        // 그룹 케이스 2: 부모의 색이 Black인 경우
        if(family.ParentColor == TreeNodeColor::eBlack)
        {

            // 케이스 5. (형제가 Red인 경우)
            if(family.SiblingColor == TreeNodeColor::eRed)
            {
                family.Parent->color_ = TreeNodeColor::eRed;
                family.Sibling->color_ = TreeNodeColor::eBlack;
                RotateNode(family.Parent, bRightChild ? RotateMode::LL : RotateMode::RR);
                RemoveFixupExtraBlack(_child);
                return;
            }

            // 케이스 1 ~ 4 (형제가 Black인 경우)
            if(family.NephewTriColor == TreeNodeColor::eBlack && family.NephewLineColor == TreeNodeColor::eBlack)
            {
                // 케이스 1. 조카 모두 Black인 경우
                family.Sibling->color_ = TreeNodeColor::eRed;
                RemoveFixupExtraBlack(family.Parent); // Extra Black을 없앨 수 없으므로 부모로 전달
                return;
            }

            if(family.NephewLineColor == TreeNodeColor::eRed)
            {
                // 케이스 2. 라인조카가 Red인 경우
                family.NephewLine->color_ = TreeNodeColor::eBlack;
                RotateNode(family.Parent, bRightChild ? RotateMode::LL : RotateMode::RR);
                return;
            }

            if(family.NephewTriColor == TreeNodeColor::eRed)
            {
                // 케이스 3. 꺽인조카가 Red인 경우
                family.NephewTri->color_ = TreeNodeColor::eBlack;
                family.Sibling->color_ = TreeNodeColor::eRed;
                RotateNode(family.Sibling, bRightChild ? RotateMode::RR : RotateMode::LL);
                RemoveFixupExtraBlack(_child); // 케이스 2로 처리하기위해 재호출
                return;
            }

            return;
        }

        DebugAssertMsg(family.SiblingColor == TreeNodeColor::eBlack, "[그룹 케이스 1] 형제노드가 Black이 아닙니다.");
        // 그룹 케이스 1: 부모의 색이 Red인 경우
        if(family.NephewTriColor == TreeNodeColor::eBlack && family.NephewLineColor == TreeNodeColor::eBlack)
        {
            // 케이스 1. 조카 모두 Black인 경우
            family.Sibling->color_ = TreeNodeColor::eRed;
            family.Parent->color_ = TreeNodeColor::eBlack;
            return;
        }

        if(family.NephewLineColor == TreeNodeColor::eRed)
        {
            // 케이스 2. 라인조카가 Red인 경우

            family.NephewLine->color_ = TreeNodeColor::eBlack;
            family.Sibling->color_ = TreeNodeColor::eRed;
            family.Parent->color_ = TreeNodeColor::eBlack;
            RotateNode(family.Parent, bRightChild ? RotateMode::LL : RotateMode::RR);
            return;
        }

        if(family.NephewTriColor == TreeNodeColor::eRed)
        {
            // 케이스 3. 꺽인조카가 Red인 경우
            family.NephewTri->color_ = TreeNodeColor::eBlack;
            family.Sibling->color_ = TreeNodeColor::eRed;
            RotateNode(family.Sibling, bRightChild ? RotateMode::RR : RotateMode::LL);
            RemoveFixupExtraBlack(_child); // 케이스 2로 처리하기위해 재호출
        }
    }

    void RotateNode(TTreeNode* _pNode, RotateMode _mode)
    {
        switch(_mode)
        {
        case RotateMode::RR:
            RotateRR(_pNode);
            return;
        case RotateMode::LL:
            RotateLL(_pNode);
            return;
        }
    }
    // 노드가 왼쪽/왼쪽으로 붙은 경우
    void RotateLL(TTreeNode* _pNode)
    {
        TTreeNode* pParent = _pNode->pParent_;
        TTreeNode* pCur = _pNode;
        TTreeNode* pChild = _pNode->pLeft_;
        TTreeNode* pChildRight = _pNode->pLeft_->pRight_;

        if(pParent)
        {
            if(pParent->pLeft_ == pCur)
                pParent->pLeft_ = pChild;
            else
                pParent->pRight_ = pChild;
        }
        pChild->pParent_ = pParent;

        pCur->pLeft_ = pChildRight;
        if(pChildRight)
            pChildRight->pParent_ = pCur;

        pChild->pRight_ = pCur;
        pCur->pParent_ = pChild;

        // 회전으로 인한 루트 변경 업데이트
        if(pRoot_ == pCur)
        {
            pRoot_ = pChild;
        }
    }
    // 노드가 오른쪽/오른쪽으로 붙은 경우
    void RotateRR(TTreeNode* _pNode)
    {
        TTreeNode* pParent = _pNode->pParent_;
        TTreeNode* pCur = _pNode;
        TTreeNode* pChild = _pNode->pRight_;
        TTreeNode* pChildLeft = _pNode->pRight_->pLeft_;

        if(pParent)
        {
            if(pParent->pLeft_ == pCur)
                pParent->pLeft_ = pChild;
            else
                pParent->pRight_ = pChild;
        }
        pChild->pParent_ = pParent;

        pCur->pRight_ = pChildLeft;
        if(pChildLeft)
            pChildLeft->pParent_ = pCur;

        pChild->pLeft_ = pCur;
        pCur->pParent_ = pChild;

        // 회전으로 인한 루트 변경 업데이트
        if(pRoot_ == pCur)
        {
            pRoot_ = pChild;
        }
    }

    // node서브트리 기준으로 제일 왼쪽 끝에 위치한 노드
    //   = 중위순회[inorder]시 제일 처음 출력될 노드
    //   = 제일 작은 노드
    static TTreeNode* FindSmallestNode(TTreeNode* _pNode)
    {
        if(_pNode == nullptr)
            return nullptr;

        TTreeNode* pCur = _pNode;
        for(;;)
        {
            if(pCur->pLeft_ == nullptr)
                break;
            pCur = pCur->pLeft_;
        }
        return pCur;
    }

    // node 서브트리 기준으로 제일 오른쪽 끝에 위치한 노드
    //   = 역방향 중위순회시[inorder] 제일 처음 출력될 노드
    //   = 제일 큰 노드
    static TTreeNode* FindBiggestNode(TTreeNode* _pNode)
    {
        if(_pNode == nullptr)
            return nullptr;

        TTreeNode* pCur = _pNode;
        for(;;)
        {
            if(pCur->pRight_ == nullptr)
                break;
            pCur = pCur->pRight_;
        }
        return pCur;
    }

    // node의 좌측 서브트리에서 가장 큰 노드
    static TTreeNode* FindPredecessorNode(TTreeNode* _pNode)
    {
        TTreeNode* pCur = _pNode->pLeft_;

        while(pCur != nullptr)
        {
            if(pCur->pRight_ == nullptr)
            {
                return pCur;
            }
            pCur = pCur->pRight_;
        }

        return pCur;
    }

    // node의 우측 서브트리에서 가장 큰 노드
    static TTreeNode* FindSuccessorNode(TTreeNode* _pNode)
    {
        TTreeNode* pCur = _pNode->pRight_;

        while(pCur != nullptr)
        {
            if(pCur->pLeft_ == nullptr)
            {
                return pCur;
            }
            pCur = pCur->pLeft_;
        }

        return pCur;
    }

    // node보다 1단계 더 큰 노드를 반환한다. 없을 경우 nullptr
    static TTreeNode* FindBiggerNode(TTreeNode* _pNode)
    {
        TTreeNode* pBigger = TTreeTable::FindSuccessorNode(_pNode);

        if(pBigger == nullptr)
        {
            TTreeNode* pParent = _pNode->pParent_;

            if(pParent && pParent->pLeft_ == _pNode)
                pBigger = pParent;
            else
            {
                // 연속 우측 분기가 종료되는 지점의 부모가 다음 큰 수이다.
                while(pParent)
                {
                    TTreeNode* pNextParent = pParent->pParent_;
                    if(pNextParent && pNextParent->pLeft_ == pParent)
                    {
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
    static TTreeNode* FindSmallerNode(TTreeNode* _pNode)
    {
        TTreeNode* pSmaller = TTreeTable::FindPredecessorNode(_pNode);

        if(pSmaller == nullptr)
        {
            TTreeNode* pParent = _pNode->pParent_;
            if(pParent && pParent->pRight_ == _pNode)
                pSmaller = pParent;
            else
            {
                // 연속 좌측 분기가 종료되는 지점의 부모가 다음 큰 수이다.
                while(pParent)
                {
                    TTreeNode* pNextParent = pParent->pParent_;
                    if(pNextParent && pNextParent->pRight_ == pParent)
                    {
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
    static void InorderTraverseForEach(TTreeNode* _pNode, Consumer&& _consumer)
    {
        if(_pNode == nullptr)
            return;
        InorderTraverseForEach<ValueType>(_pNode->pLeft_, Forward<Consumer>(_consumer));
        if constexpr(ValueType == TraverseValueType::Pair)
            _consumer(_pNode->data_);
        else if constexpr(ValueType == TraverseValueType::Key)
            _consumer(_pNode->data_.key_);
        else if constexpr(ValueType == TraverseValueType::Value)
            _consumer(_pNode->data_.value_);
        else
            DebugAssert(false);
                InorderTraverseForEach<ValueType>(_pNode->pRight_, Forward<Consumer>(_consumer));
    }

    template <TraverseValueType ValueType, typename Consumer>
    static void InorderTraverseReverseForEach(TTreeNode* _pNode, Consumer&& _consumer)
    {
        if(_pNode == nullptr)
            return;
        InorderTraverseForEach<ValueType>(_pNode->pRight_, Forward<Consumer>(_consumer));
        if constexpr(ValueType == TraverseValueType::Pair)
            _consumer(_pNode->data_);
        else if constexpr(ValueType == TraverseValueType::Key)
            _consumer(_pNode->data_.key_);
        else if constexpr(ValueType == TraverseValueType::Value)
            _consumer(_pNode->data_.Value);
        else
            DebugAssert(false);
        InorderTraverseForEach<ValueType>(_pNode->pLeft_, Forward<Consumer>(_consumer));
    }

    static void DeleteNodeRecursive(TTreeNode* _pNode)
    {
        if(_pNode == nullptr)
            return;
        DeleteNodeRecursive(_pNode->pLeft_);
        DeleteNodeRecursive(_pNode->pRight_);
        JCORE_ALLOCATOR_STATIC_DEALLOCATE_AND_DESTROY_SAFE(TTreeNode, _pNode);
    }
    static void MaxHeightRecursive(TTreeNode* _pNode, int _height, int& _maxHeight)
    {
        if(_pNode == nullptr)
        {
            _maxHeight = Math::Max(_maxHeight, _height);
            return;
        }

        MaxHeightRecursive(_pNode->pLeft_, _height + 1, _maxHeight);
        MaxHeightRecursive(_pNode->pRight_, _height + 1, _maxHeight);
    }

    static void ConnectPredecessorChildToParent(TTreeNode* _predecessor, TTreeNode* _predecessorLeftChild)
    {

        if(_predecessor->IsRight())
        {
            _predecessor->pParent_->pRight_ = _predecessorLeftChild;
            _predecessorLeftChild->pParent_ = _predecessor->pParent_;
            return;
        }

        _predecessor->pParent_->pLeft_ = _predecessorLeftChild;
        _predecessorLeftChild->pParent_ = _predecessor->pParent_;
    }

    // node를 root로 하는 트리에서의 key보다 처음으로 같거나 커지는 노드
    static TTreeNode* LowerBoundNode(TTreeNode* _rootNode, const TKey& _key)
    {
        if(_rootNode == nullptr)
            return nullptr;

        TTreeNode* pHigh = FindBiggestNode(_rootNode);
        TTreeNode* pCur = _rootNode;

        while(pCur)
        {
            const int comp = KeyComparator(_key, pCur->data_.key_);
            if(comp > 0)
            {
                pCur = pCur->pRight_;
            }
            else
            {
                pHigh = pCur;
                pCur = pCur->pLeft_;
            }
        }

        if(KeyComparator(_key, pHigh->data_.key_) > 0)
            return nullptr;

        return pHigh;
    }

    // node를 root로 하는 트리에서의 key보다 처음으로 커지는 노드
    static TTreeNode* UpperBoundNode(TTreeNode* _rootNode, const TKey& _key)
    {
        if(_rootNode == nullptr)
            return nullptr;

        TTreeNode* pHigh = FindBiggestNode(_rootNode);
        TTreeNode* pCur = _rootNode;

        while(pCur)
        {
            const int comp = KeyComparator(_key, pCur->data_.key_);
            if(comp >= 0)
            {
                pCur = pCur->pRight_;
            }
            else
            {
                pHigh = pCur;
                pCur = pCur->pLeft_;
            }
        }

        if(KeyComparator(_key, pHigh->data_.key_) >= 0)
            return nullptr;

        return pHigh;
    }

    TTreeNode* pRoot_;
    int size_;

    inline static TKyComparator KeyComparator;

    template <typename, typename, typename, typename, ETreeTableImplementation> friend class TreeMap;
    template <typename, typename, typename, typename, ETreeTableImplementation> friend class TreeMapIterator;
}; // class CTreeTable<Pack<TKey, TValue, TKeyComparator, TAllocator>,
   // ETreeTableImplementation::RedBlackTree>

#pragma endregion

NS_JC_END
