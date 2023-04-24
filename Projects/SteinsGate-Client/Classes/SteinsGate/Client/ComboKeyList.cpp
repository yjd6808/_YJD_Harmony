/*
 * 작성자: 윤정도
 * 생성일: 1/12/2023 1:14:41 PM
 * =====================
 *
 */

#include "Tutturu.h"
#include "ComboKeyList.h"

ComboKeyList::ComboKeyList() {
	for (int i = 0; i < ComboSequenceCount_v; ++i) {
		Keys[i] = ControlKey::None;
	}
}

ComboKeyList::ComboKeyList(std::initializer_list<ControlKey_t> list) {
	int iKeyCount = list.size();
	DebugAssertMsg(iKeyCount <= ComboSequenceCount_v, "최대 콤보 가능한 컨트롤 키의 수를 벗어났습니다.");

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

bool ComboKeyList::operator==(const ComboKeyList& other) const {
	int iCount = count();

	for (int i = 0; i < iCount; ++i) {
		if (Keys[i] != other.Keys[i])
			return false;
	}

	return true;
}

bool ComboKeyList::operator!=(const ComboKeyList& other) const {
	return !this->operator==(other);
}

ControlKey_t& ComboKeyList::operator[](const int idx) {
	return Keys[idx];
}

void ComboKeyList::set(int idx, ControlKey_t key) {
	Keys[idx] = key;
}

ControlKey_t ComboKeyList::at(int idx) const {
	return Keys[idx];
}

ComboKeyList ComboKeyList::reverse() const {
	ComboKeyList reverse;
	int iCount = count();

	for (int i = 0; i < iCount; ++i) {
		if (Keys[i] == ControlKey_t::Left || Keys[i] == ControlKey_t::Right) {
			reverse.Keys[i] = ControlKey::ReverseDirection[Keys[i]];
			continue;
		}

		reverse.Keys[i] = Keys[i];
	}

	return reverse;
}

SGString ComboKeyList::string() const {
	SGString szKey;
	const int iCount = count();

	for (int i = 0; i < iCount; i++) {
		szKey += ControlKey::Name[Keys[i]];
		szKey += " ";
	}

	return szKey;
}

int ComboKeyList::count() const {
	int iCount = 0;
	for (int i = 0; i < ComboSequenceCount_v; ++i) {
		if (Keys[i] == ControlKey::None) {
			break;
		}
		++iCount;
	}

	return iCount;
}
