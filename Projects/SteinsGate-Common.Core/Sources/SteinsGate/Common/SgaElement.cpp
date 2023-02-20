
/*
 * 작성자: 윤정도
 * 생성일: 1/8/2023 4:37:25 AM
 * =====================
 *
 */

#include "Core.h"
#include "SgaElement.h"

SgaPackage* SgaElement::GetParent()
{
	if (m_spParent.Exist())
		return m_spParent.GetPtr();

	return nullptr;
}
