/*
 * 작성자: 윤정도
 * 생성일: 1/8/2023 5:46:35 AM
 * =====================
 *
 */


#pragma once

#include <SteinsGate/Common/SgaElement.h>
#include <JCore/Container/Vector.h>

class SgaSound : public SgaElement
{
	using SgaSoundPtr = JCore::SharedPtr<SgaSound>;
public:
	SgaSound(const Header& header) : SgaElement(SgaElement::eSound, header) {}
	~SgaSound() override;
public:
	SgaDataPtr GetData() { return m_spData; }
	bool Load(bool indexOnly) override;
	bool Unload();

	bool DataLoaded() override { return m_spData.Exist() ; }
public:
	static SgaSoundPtr Create(const Header& header);
protected:
	SgaDataPtr m_spData;

	template <Int32>
	friend class SgaElementInitializerImpl;
	friend class SgaLoader;
};

using SgaSoundPtr = JCore::SharedPtr<SgaSound>;
using SgaSoundDataPtr = JCore::SharedPtr<Byte[]>;
