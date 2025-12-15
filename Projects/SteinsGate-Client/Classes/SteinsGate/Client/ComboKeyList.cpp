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
		Keys[i] = ControlKey::None;
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
			Keys[i] = *(it + i);
		}
		else
		{
			Keys[i] = ControlKey::None;
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
		Keys[i] = _other.Keys[i];
	}
	return *this;
}

//////////////////////////////////////////////////////////////////////////////////////////
bool ComboKeyList::operator==(const ComboKeyList& _other) const
{
	int comboCount = count();

	for (int i = 0; i < comboCount; ++i)
	{
		if (Keys[i] != _other.Keys[i])
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
	return Keys[_idx];
}

//////////////////////////////////////////////////////////////////////////////////////////
void ComboKeyList::set(int _idx, ControlKey_t _key)
{
	Keys[_idx] = _key;
}

//////////////////////////////////////////////////////////////////////////////////////////
ControlKey_t ComboKeyList::at(int _idx) const
{
	return Keys[_idx];
}

//////////////////////////////////////////////////////////////////////////////////////////
ComboKeyList ComboKeyList::reverse() const
{
	ComboKeyList reversedComboKeyList;
	int comboCount = count();

	for (int i = 0; i < comboCount; ++i)
	{
		if (Keys[i] == ControlKey_t::Left || Keys[i] == ControlKey_t::Right)
		{
			reversedComboKeyList.Keys[i] = ControlKey::ReverseDirection[Keys[i]];
			continue;
		}

		reversedComboKeyList.Keys[i] = Keys[i];
	}

	return reversedComboKeyList;
}

//////////////////////////////////////////////////////////////////////////////////////////
SGString ComboKeyList::string() const
{
	SGString keyString;
	const int comboCount = count();

	for (int i = 0; i < comboCount; i++)
	{
		keyString += ControlKey::Name[Keys[i]];
		keyString += " ";
	}

	return keyString;
}

//////////////////////////////////////////////////////////////////////////////////////////
int ComboKeyList::count() const
{
	int comboCount = 0;
	for (int i = 0; i < Const::Action::ComboSequenceCount; ++i)
	{
		if (Keys[i] == ControlKey::None)
		{
			break;
		}
		++comboCount;
	}

	return comboCount;
}
