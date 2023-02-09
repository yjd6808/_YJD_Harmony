/*
 * 작성자: 윤정도
 * 생성일: 1/12/2023 1:14:31 PM
 * =====================
 *
 */


#pragma once


#include <SteinsGate/Research/Config.h>
#include <JCore/Primitives/SmartPtr.h>

struct ComboKeyList
{
public:
	ComboKeyList();
	ComboKeyList(std::initializer_list<ControlKey_t> list);
	ComboKeyList(const ComboKeyList& other);
	ComboKeyList& operator=(const ComboKeyList& other);
	ControlKey_t& operator[](const int idx);

	ControlKey_t At(int idx) const;
	int Count() const;
private:
	ControlKey_t Keys[ComboSequenceCount_v];
};

using ComboKeyListPtr = JCore::SharedPtr<ComboKeyList>;