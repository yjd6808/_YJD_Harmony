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
	ControlKey_t& operator[](const int idx);
	void Set(int idx, ControlKey_t key);

	ControlKey_t At(int idx) const;
	SGString String();
	int Count() const;
private:
	ControlKey_t Keys[ComboSequenceCount_v];
};

using ComboKeyListPtr = JCore::SharedPtr<SGComboKeyList>;