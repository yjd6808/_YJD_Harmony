/*
 * 작성자: 윤정도
 * 생성일: 1/12/2023 1:14:41 PM
 * =====================
 *
 */

#include "Tutturu.h"
#include "SGComboKeyList.h"

SGComboKeyList::SGComboKeyList() {
	for (int i = 0; i < ComboSequenceCount_v; ++i) {
		Keys[i] = ControlKey::None;
	}
}

SGComboKeyList::SGComboKeyList(std::initializer_list<ControlKey_t> list) {
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

SGComboKeyList::SGComboKeyList(const SGComboKeyList& other) {
	this->operator=(other);
}

SGComboKeyList& SGComboKeyList::operator=(const SGComboKeyList& other) {
	for (int i = 0; i < ComboSequenceCount_v; ++i) {
		Keys[i] = other.Keys[i];
	}
	return *this;
}

bool SGComboKeyList::operator==(const SGComboKeyList& other) const {
	int iCount = count();

	for (int i = 0; i < iCount; ++i) {
		if (Keys[i] != other.Keys[i])
			return false;
	}

	return true;
}

bool SGComboKeyList::operator!=(const SGComboKeyList& other) const {
	return !this->operator==(other);
}

ControlKey_t& SGComboKeyList::operator[](const int idx) {
	return Keys[idx];
}

void SGComboKeyList::set(int idx, ControlKey_t key) {
	Keys[idx] = key;
}

ControlKey_t SGComboKeyList::at(int idx) const {
	return Keys[idx];
}

SGComboKeyList SGComboKeyList::reverse() const {
	SGComboKeyList reverse;
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

SGString SGComboKeyList::string() const {
	SGString szKey;
	const int iCount = count();

	for (int i = 0; i < iCount; i++) {
		szKey += ControlKey::Name[Keys[i]];
		szKey += " ";
	}

	return szKey;
}

int SGComboKeyList::count() const {
	int iCount = 0;
	for (int i = 0; i < ComboSequenceCount_v; ++i) {
		if (Keys[i] == ControlKey::None) {
			break;
		}
		++iCount;
	}

	return iCount;
}
