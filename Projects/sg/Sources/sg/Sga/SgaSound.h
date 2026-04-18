/*
 * 작성자: 윤정도
 * 생성일: 1/8/2023 5:46:35 AM
 * =====================
 *
 */


#pragma once

#include "sg/Sga/SgaElement.h"
#include "jc/Container/Vector.h"

class SgaSound : public SgaElement
{
	using SgaSoundPtr = jc::SharedPtr<SgaSound>;

public:
	SgaSound(const Header& _header)
	: SgaElement(SgaElement::eSound, _header)
	{
	}

	~SgaSound() override;

public:
	SgaDataPtr GetData() { return pData_; }
	bool Load(bool _indexOnly) override;
	bool Unload() override;

	bool DataLoaded() override { return pData_.Exist(); }

public:
	static SgaSoundPtr Create(const Header& _header);

protected:
	SgaDataPtr pData_;

	template <_s32>
	friend class SgaElementInitializerImpl;
	friend class SgaLoader;
};

using SgaSoundPtr = jc::SharedPtr<SgaSound>;
using SgaSoundDataPtr = jc::SharedPtr<_u8[]>;
