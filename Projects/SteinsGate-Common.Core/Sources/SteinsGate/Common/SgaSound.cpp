/*
 * 작성자: 윤정도
 * 생성일: 1/8/2023 6:25:59 AM
 * =====================
 *
 */

#include "Core.h"
#include <SteinsGate/Common/SgaSound.h>
#include <SteinsGate/Common/SgaPackage.h>

USING_NS_JC;

//////////////////////////////////////////////////////////////////////////////////////////
SgaSound::~SgaSound()
{
}

//////////////////////////////////////////////////////////////////////////////////////////
bool SgaSound::Load(bool _indexOnly)
{
	if (_indexOnly)
		return true;

	if (!pParent_.Exist())
		return false;

	pData_ = MakeShared<Byte[]>(indexLength_);
	pParent_->StreamRef().Seek(indexOffset_);
	pParent_->StreamRef().Read(pData_.GetPtr(), 0, pData_.Length());
	return true;
}

//////////////////////////////////////////////////////////////////////////////////////////
bool SgaSound::Unload()
{
	if (!pData_.Exist())
		return false;

	pData_ = nullptr;
	return true;
}

//////////////////////////////////////////////////////////////////////////////////////////
SgaSoundPtr SgaSound::Create(const Header& _header)
{
	return MakeShared<SgaSound>(_header);
}
