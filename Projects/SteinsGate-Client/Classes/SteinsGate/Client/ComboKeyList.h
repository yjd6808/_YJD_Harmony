/*
 * 작성자: 윤정도
 * 생성일: 1/12/2023 1:14:31 PM
 * =====================
 *
 */


#pragma once


#include <SteinsGate/Client/Tutturu.h>

#include <JCore/Primitives/SmartPtr.h>

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
	void set(int _idx, ControlKey_t _key);

	ControlKey_t at(int _idx) const;
	ComboKeyList reverse() const; // 좌, 우 방향키 반전 후 반환
	SGString string() const;
	int count() const;

	ControlKey_t Keys[Const::Action::ComboSequenceCount];
};

using ComboKeyListPtr = JCore::SharedPtr<ComboKeyList>;
