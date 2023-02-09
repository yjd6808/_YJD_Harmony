/*
 * 작성자: 윤정도
 * 생성일: 1/8/2023 5:46:35 AM
 * =====================
 *
 */


#pragma once

#include <SteinsGate/Common/Core/Npk/NpkElement.h>
#include <JCore/Container/Vector.h>

class NpkSound : public NpkElement
{
	using NpkSoundPtr = JCore::SharedPtr<NpkSound>;
public:
	NpkSound(const Header& header) : NpkElement(NpkElement::eSound, header) {}
	~NpkSound() override;
public:
	NpkDataPtr GetData() { return m_spData; }
	bool Load(bool indexOnly) override;
	void Unload();

	bool DataLoaded() override { return m_spData.Exist() ; }
public:
	static NpkSoundPtr Create(const Header& header);
protected:
	NpkDataPtr m_spData;

	template <Int32>
	friend class NpkElementInitializerImpl;
	friend class NpkLoader;
};

using NpkSoundPtr = JCore::SharedPtr<NpkSound>;
using NpkSoundDataPtr = JCore::SharedPtr<Byte[]>;
