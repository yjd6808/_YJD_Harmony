/*
 * 작성자: 윤정도
 * 생성일: 1/12/2023 1:14:31 PM
 * =====================
 *
 */


#pragma once


#include <sgcl/Core.h>

#include <jc/Primitives/SmartPtr.h>

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
