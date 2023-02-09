/*
 * 작성자: 윤정도
 * 생성일: 1/12/2023 1:14:31 PM
 * =====================
 *
 */


#pragma once


#include <SteinsGate/Research/Tutturu.h>
#include <SteinsGate/Research/SGStruct.h>
#include <JCore/Primitives/SmartPtr.h>

struct SGComboKeyList
{
public:
	SGComboKeyList();
	SGComboKeyList(std::initializer_list<ControlKey_t> list);
	SGComboKeyList(const SGComboKeyList& other);
	SGComboKeyList& operator=(const SGComboKeyList& other);

	bool operator==(const SGComboKeyList& other) const;
	bool operator!=(const SGComboKeyList& other) const;

	ControlKey_t& operator[](int idx);
	void set(int idx, ControlKey_t key);

	ControlKey_t at(int idx) const;
	SGComboKeyList reverse() const;		// 좌, 우 방향키 반전 후 반환
	SGString string() const;
	int count() const;

	ControlKey_t Keys[ComboSequenceCount_v];
};

using ComboKeyListPtr = JCore::SharedPtr<SGComboKeyList>;