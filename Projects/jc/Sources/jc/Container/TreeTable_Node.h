/*
 * 작성자: 윤정도
 * 생성일: 8/15/2023 3:22:16 AM
 * =====================
 * TreeTable에서 사용할 노드
 */


#pragma once

#include "jc/Hasher.h"
#include "jc/Memory.h"

#include "jc/Container/Pair.h"

NS_JC_BEGIN
struct TreeNodeColor
{
	enum _
	{
		eBlack,
		eRed
	};
};

template <typename TKey>
class TreeNode
{
public:
	using TTreeNode = TreeNode<TKey>;

	enum color
	{
		eBlack,
		eRed
	};

	template <typename Ky>
	TreeNode(Ky&& _data)
	: pParent_(nullptr)
	, pLeft_(nullptr)
	, pRight_(nullptr)
	, color_(TreeNodeColor::eRed)
	{
		new (&data_) TKey(Forward<Ky>(_data));
	}
	~TreeNode()
	{
		data_.~TKey();
	}

	// 둘중 할당된 자식 아무거나 반환
	TreeNode* Any() const
	{
		return pLeft_ ? pLeft_ : pRight_;
	}

	// 둥중 하나의 자식 아무거나 반환 및 자식이 몇개있는지도 같이 반환
	TreeNode* AnyWithChildrenCount(OUT int& _count) const
	{
		if (pLeft_ && pRight_)
		{
			_count = 2;
			return pLeft_;
		}

		if (pLeft_)
		{
			_count = 1;
			return pLeft_;
		}

		if (pRight_)
		{
			_count = 1;
			return pRight_;
		}

		_count = 0;
		return nullptr;
	}

	bool IsLeft() const
	{
		return pParent_->pLeft_ == this;
	}

	bool IsRight() const
	{
		return pParent_->pRight_ == this;
	}

	int Count() const
	{
		if (pLeft_ && pRight_)
			return 2;
		if (pLeft_)
			return 1;
		if (pRight_)
			return 1;
		return 0;
	}

	union
	{
		TKey data_;
	};
	TTreeNode* pParent_;
	TTreeNode* pLeft_;
	TTreeNode* pRight_;
	char color_;
};

NS_END
