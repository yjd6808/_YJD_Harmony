/*
 * 작성자: 윤정도
 * 생성일: 8/15/2023 3:22:16 AM
 * =====================
 * TreeTable에서 사용할 노드
 */


#pragma once

#include <JCore/Hasher.h>
#include <JCore/Memory.h>

#include <JCore/Container/Pair.h>

NS_JC_BEGIN

struct TreeNodeColor { enum _ { eBlack, eRed }; };

template <typename TKey>
struct TreeNode
{
	using TTreeNode = TreeNode<TKey>;

	enum Color
	{
		eBlack,
		eRed
	};

	template <typename Ky>
	TreeNode(Ky&& data)
		: Parent(nullptr)
		, Left(nullptr)
		, Right(nullptr)
		, Color(TreeNodeColor::eRed) {
		Data = Forward<Ky>(data);
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

	TKey Data;
	TTreeNode* Parent;
	TTreeNode* Left;
	TTreeNode* Right;
	char Color;
};

NS_JC_END

