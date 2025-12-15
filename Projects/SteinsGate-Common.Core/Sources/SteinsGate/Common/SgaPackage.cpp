/*
 * 작성자: 윤정도
 * 생성일: 1/8/2023 4:36:58 AM
 * =====================
 *
 */

#include "Core.h"
#include <SteinsGate/Common/SgaLoader.h>
#include <SteinsGate/Common/SgaPackage.h>

#include <JCore/FileSystem/FileStream.h>
#include <JCore/FileSystem/Path.h>

USING_NS_JC;

//////////////////////////////////////////////////////////////////////////////////////////
SgaPackage::~SgaPackage()
{
}

//////////////////////////////////////////////////////////////////////////////////////////
SgaPackagePtr SgaPackage::Create(const StreamPtr& _pReadOnlyStream, const String& _path, int _capacity)
{
	return MakeShared<SgaPackage>(_pReadOnlyStream, _path, _capacity);
}

//////////////////////////////////////////////////////////////////////////////////////////
void SgaPackage::Add(const int _index, const SgaElementPtr& _pElement)
{
	elementMap_.Insert(_index, _pElement);
}

//////////////////////////////////////////////////////////////////////////////////////////
SgaElementPtr SgaPackage::Get(int _index)
{
	DebugAssertMsg(elementMap_.Exist(_index), "엘리먼트가 존재하지 않습니다.");
	return elementMap_[_index];
}

//////////////////////////////////////////////////////////////////////////////////////////
SgaElementPtr SgaPackage::GetUnsafe(int _index)
{
	if (!elementMap_.Exist(_index))
		return nullptr;

	return elementMap_[_index];
}

//////////////////////////////////////////////////////////////////////////////////////////
void SgaPackage::LoadElement(const int _index, bool _elementOnly)
{
	DebugAssertMsg(_index >= 0 && _index < elementHeaders_.Size(), "올바르지 않은 Element 인덱스입니다.");
	const SgaElementPtr pElement = SgaLoader::ReadElement(stream_.GetRef(), elementHeaders_[_index],
	                                                      elementHeaders_[_index].nextOffset_, _elementOnly);
	DebugAssertMsg(pElement.Exist(), "엘리먼트 파싱에 실패했습니다.");
	pElement->pParent_ = Weak();
	Add(_index, pElement);
}

//////////////////////////////////////////////////////////////////////////////////////////
int SgaPackage::UnloadAllElementData()
{
	int unloadedElementCount = 0;

	elementMap_.ForEachValue([&unloadedElementCount](const SgaElementPtr& _pElement)
	{
		if (_pElement->Unload())
			++unloadedElementCount;
	});

	return unloadedElementCount;
}

//////////////////////////////////////////////////////////////////////////////////////////
bool SgaPackage::IsElementLoaded(const int _index) const
{
	return elementMap_.Exist(_index);
}

//////////////////////////////////////////////////////////////////////////////////////////
int SgaPackage::GetElementIndex(const String& _elementName)
{
	bool exist = elementNameToIndex_.Exist(_elementName);
	DebugAssertMsg(exist, "해당 엘리먼트 이름에 해당하는 인덱스가 없습니다.");
	return elementNameToIndex_[_elementName];
}

//////////////////////////////////////////////////////////////////////////////////////////
int SgaPackage::GetElementIndex(const char* _elementName)
{
	bool exist = elementNameToIndex_.Exist(_elementName);
	DebugAssertMsg(exist, "해당 엘리먼트 이름에 해당하는 인덱스가 없습니다.");
	return elementNameToIndex_[_elementName];
}

//////////////////////////////////////////////////////////////////////////////////////////
bool SgaPackage::HasElementIndex(const String& _elementName) const
{
	return elementNameToIndex_.Exist(_elementName);
}

//////////////////////////////////////////////////////////////////////////////////////////
bool SgaPackage::HasElementIndex(const char* _elementName) const
{
	return elementNameToIndex_.Exist(_elementName);
}

//////////////////////////////////////////////////////////////////////////////////////////
SgaElement& SgaPackage::operator[](const int _index)
{
	DebugAssert(elementMap_.Exist(_index));
	return elementMap_[_index].GetRef();
}

//////////////////////////////////////////////////////////////////////////////////////////
String SgaPackage::ToString() const
{
	return StringUtil::Format("Sga(%s)", path_.Source());
}
