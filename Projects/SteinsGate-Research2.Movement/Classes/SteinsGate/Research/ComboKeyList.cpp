/*
 * 작성자: 윤정도
 * 생성일: 1/12/2023 1:14:41 PM
 * =====================
 *
 */


#include "ComboKeyList.h"

ComboKeyList::ComboKeyList() {
	for (int i = 0; i < ComboSequenceCount_v; ++i) {
		Keys[i] = ControlKey::None;
	}
}

ComboKeyList::ComboKeyList(std::initializer_list<ControlKey_t> list) {
	int iKeyCount = list.size();
	DebugAssertMessage(iKeyCount <= ComboSequenceCount_v, "최대 콤보 가능한 컨트롤 키의 수를 벗어났습니다.");

	auto it = list.begin();

	for (int i = 0; i < ComboSequenceCount_v; ++i) {
		if (i < iKeyCount)
			Keys[i] = *(it + i);
		else
			Keys[i] = ControlKey::None;
	}
}

ComboKeyList::ComboKeyList(const ComboKeyList& other) {
	this->operator=(other);
}

ComboKeyList& ComboKeyList::operator=(const ComboKeyList& other) {
	for (int i = 0; i < ComboSequenceCount_v; ++i) {
		Keys[i] = other.Keys[i];
	}
	return *this;
}

ControlKey_t& ComboKeyList::operator[](const int idx) {
	return Keys[idx];
}

ControlKey_t ComboKeyList::At(int idx) const {
	return Keys[idx];
}

int ComboKeyList::Count() const {
	int iCount = 0;
	for (int i = 0; i < ComboSequenceCount_v; ++i) {
		if (Keys[i] == ControlKey::None) {
			break;
		}
		++iCount;
	}

	return iCount;
}
