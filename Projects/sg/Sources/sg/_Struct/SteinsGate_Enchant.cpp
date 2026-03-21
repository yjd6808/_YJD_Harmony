/*
 * 작성자: 윤정도
 * 생성일: 1/24/2023 9:27:54 AM
 * =====================
 *
 */


#include "SteinsGate_Enchant.h"

//////////////////////////////////////////////////////////////////////////////////////////
EnchantBonusOptInfo::EnchantBonusOptInfo(const EnchantBonusOptInfo& _other)
{
	this->operator=(_other);
}

//////////////////////////////////////////////////////////////////////////////////////////
EnchantBonusOptInfo& EnchantBonusOptInfo::operator=(const EnchantBonusOptInfo& _other)
{
	jc_assert_msg(this != &_other, "셀프 어사인 하지마!");
	opt_ = _other.opt_;
	jc::Memory::CopyUnsafe(value_, _other.value_, sizeof(int) * Const::Item::MaxEnchantLevel);
	return *this;
}
