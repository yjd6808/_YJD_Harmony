/*
 * 작성자: 윤정도
 * 생성일: 1/20/2023 11:04:49 AM
 * =====================
 *
 */


#pragma once

#include <sg/_Util/DescBase.h>
#include <sg/_Struct/SteinsGate_CharBase.h>

#include <jc/Primitives/SmartPtr.h>

//////////////////////////////////////////////////////////////////////////////////////////
// 인덱스 기반 타입이므로 무조건 0부터 시작해야한다.
// 변수 초기화는 None으로 수동으로 할 것
JC_SENUM_BEGIN(ControlKey)
Right,
Left,
Up,
Down,
Attack,
Jump,
Max,
None
JC_SENUM_MIDDLE(ControlKey)

static constexpr const char* Name[Max]{
	"→",
	"←",
	"↑",
	"↓",
	"X",
	"C"
};

static constexpr bool IsMoveKey[Max]{
	true, true, true, true, false, false
};

static constexpr ControlKey_t ReverseDirection[Direction::Max]{
	Left,
	Right,
	Down,
	Up
};
JC_SENUM_MIDDLE_END(ControlKey)

 //////////////////////////////////////////////////////////////////////////////////////////
struct ComboKeyList
{
public:
	ComboKeyList();
	ComboKeyList(std::initializer_list<ControlKey_t> _list);
	ComboKeyList(const ComboKeyList& _other);
	ComboKeyList& operator=(const ComboKeyList& _other);

	bool operator==(const ComboKeyList& _other) const;
	bool operator!=(const ComboKeyList& _other) const;

	ControlKey_t& operator[](int _idx);
	void Set(int _idx, ControlKey_t _key);

	ControlKey_t At(int _idx) const;
	ComboKeyList Reverse() const; // 좌, 우 방향키 반전 후 반환
	jc::String ToString() const;
	int Count() const;

	ControlKey_t keys_[Const::Action::ComboSequenceCount];
};
using ComboKeyListPtr = jc::SharedPtr<ComboKeyList>;

struct ActionInfo : SDescBase
{
	~ActionInfo() override = default;

	jc::String ActionName;
	bool ForceCancelable;
	bool Moveable;
	float SpeedX;
	float SpeedY;
	ComboKeyList Command;
};
