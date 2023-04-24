/*
 * 작성자: 윤정도
 * 생성일: 1/12/2023 1:14:31 PM
 * =====================
 *
 */


#pragma once


#include "Tutturu.h"

#include <JCore/Primitives/SmartPtr.h>

struct ComboKeyList
{
public:
	ComboKeyList();
	ComboKeyList(std::initializer_list<ControlKey_t> list);
	ComboKeyList(const ComboKeyList& other);
	ComboKeyList& operator=(const ComboKeyList& other);

	bool operator==(const ComboKeyList& other) const;
	bool operator!=(const ComboKeyList& other) const;

	ControlKey_t& operator[](int idx);
	void set(int idx, ControlKey_t key);

	ControlKey_t at(int idx) const;
	ComboKeyList reverse() const;		// 좌, 우 방향키 반전 후 반환
	SGString string() const;
	int count() const;

	ControlKey_t Keys[ComboSequenceCount_v];
};

using ComboKeyListPtr = JCore::SharedPtr<ComboKeyList>;