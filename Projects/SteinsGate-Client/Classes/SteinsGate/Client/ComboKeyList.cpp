/*
 * 작성자: 윤정도
 * 생성일: 1/12/2023 1:14:41 PM
 * =====================
 *
 */

#include "Tutturu.h"
#include "ComboKeyList.h"

//////////////////////////////////////////////////////////////////////////////////////////
ComboKeyList::ComboKeyList()
{
	for (int i = 0; i < Const::Action::ComboSequenceCount; ++i)
	{
		keys_[i] = ControlKey::None;
	}
}

//////////////////////////////////////////////////////////////////////////////////////////
ComboKeyList::ComboKeyList(std::initializer_list<ControlKey_t> _list)
{
	int keyCount = static_cast<int>(_list.size());
	DebugAssertMsg(keyCount <= Const::Action::ComboSequenceCount, "최대 콤보 가능한 컨트롤 키의 수를 벗어났습니다.");

	auto it = _list.begin();

	for (int i = 0; i < Const::Action::ComboSequenceCount; ++i)
	{
		if (i < keyCount)
		{
			keys_[i] = *(it + i);
		}
		else
		{
			keys_[i] = ControlKey::None;
		}
	}
}

//////////////////////////////////////////////////////////////////////////////////////////
ComboKeyList::ComboKeyList(const ComboKeyList& _other)
{
	this->operator=(_other);
}

//////////////////////////////////////////////////////////////////////////////////////////
ComboKeyList& ComboKeyList::operator=(const ComboKeyList& _other)
{
	for (int i = 0; i < Const::Action::ComboSequenceCount; ++i)
	{
		keys_[i] = _other.keys_[i];
	}
	return *this;
}

//////////////////////////////////////////////////////////////////////////////////////////
bool ComboKeyList::operator==(const ComboKeyList& _other) const
{
	int comboCount = Count();

	for (int i = 0; i < comboCount; ++i)
	{
		if (keys_[i] != _other.keys_[i])
		{
			return false;
		}
	}

	return true;
}

//////////////////////////////////////////////////////////////////////////////////////////
bool ComboKeyList::operator!=(const ComboKeyList& _other) const
{
	return !this->operator==(_other);
}

//////////////////////////////////////////////////////////////////////////////////////////
ControlKey_t& ComboKeyList::operator[](const int _idx)
{
	return keys_[_idx];
}

//////////////////////////////////////////////////////////////////////////////////////////
void ComboKeyList::Set(int _idx, ControlKey_t _key)
{
	keys_[_idx] = _key;
}

//////////////////////////////////////////////////////////////////////////////////////////
ControlKey_t ComboKeyList::At(int _idx) const
{
	return keys_[_idx];
}

//////////////////////////////////////////////////////////////////////////////////////////
ComboKeyList ComboKeyList::Reverse() const
{
	ComboKeyList reversedComboKeyList;
	int comboCount = Count();

	for (int i = 0; i < comboCount; ++i)
	{
		if (keys_[i] == ControlKey_t::Left || keys_[i] == ControlKey_t::Right)
		{
			reversedComboKeyList.keys_[i] = ControlKey::ReverseDirection[keys_[i]];
			continue;
		}

		reversedComboKeyList.keys_[i] = keys_[i];
	}

	return reversedComboKeyList;
}

//////////////////////////////////////////////////////////////////////////////////////////
SGString ComboKeyList::ToString() const
{
	SGString keyString;
	const int comboCount = Count();

	for (int i = 0; i < comboCount; i++)
	{
		keyString += ControlKey::Name[keys_[i]];
		keyString += " ";
	}

	return keyString;
}

//////////////////////////////////////////////////////////////////////////////////////////
int ComboKeyList::Count() const
{
	int comboCount = 0;
	for (int i = 0; i < Const::Action::ComboSequenceCount; ++i)
	{
		if (keys_[i] == ControlKey::None)
		{
			break;
		}
		++comboCount;
	}

	return comboCount;
}
