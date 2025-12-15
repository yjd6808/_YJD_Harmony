/*
 * 작성자: 윤정도
 * 생성일: 2/15/2023 1:30:20 AM
 * =====================
 *
 */

#include "Tutturu.h"
#include "UIInfoLoader.h"

#include <SteinsGate/Client/ImagePackManager.h>
#include <SteinsGate/Client/JsonUtilEx.h>
#include <SteinsGate/Client/DataManager.h>
#include <SteinsGate/Client/FontManager.h>

#include "UICheckBox.h"

USING_NS_JC;
USING_NS_JS;

//////////////////////////////////////////////////////////////////////////////////////////
UIInfoLoader::UIInfoLoader(DataManagerAbstract* _pManager)
: ConfigFileLoaderAbstract(_pManager)
{
}

//////////////////////////////////////////////////////////////////////////////////////////
bool UIInfoLoader::Load()
{
	Json::Value root;

	if (!LoadJson(root))
		return false;

	try
	{
		// ========================================================================
		//  UI 엘리먼트 로딩
		// ========================================================================
		Json::Value& elementListRoot = root[JsonElementsKey];

		for (int i = 0; i < elementListRoot.size(); ++i)
		{
			Json::Value& elementRoot = elementListRoot[i];

			UIElementInfo* pElementInfo = nullptr;
			int elementType = elementRoot[JsonElementTypeKey].asInt();

			switch (elementType)
			{
			case UIElementType::Button: pElementInfo = readElementButton(elementRoot);
				break;
			case UIElementType::Label: pElementInfo = readElementLabel(elementRoot);
				break;
			case UIElementType::Sprite: pElementInfo = readElementSprite(elementRoot);
				break;
			case UIElementType::EditBox: pElementInfo = readElementEditBox(elementRoot);
				break;
			case UIElementType::CheckBox: pElementInfo = readElementCheckBox(elementRoot);
				break;
			case UIElementType::ToggleButton: pElementInfo = readElementToggleButton(elementRoot);
				break;
			case UIElementType::ScrollBar: pElementInfo = readElementScrollBar(elementRoot);
				break;
			case UIElementType::ProgressBar: pElementInfo = readElementProgressBar(elementRoot);
				break;
			case UIElementType::Static: pElementInfo = readElementStatic(elementRoot);
				break;
			default:
				break;
			}

			DebugAssertMsg(pElementInfo != nullptr, "UI 엘리먼트 로딩에 실패했습니다.");
			AddData(pElementInfo);
		}

		// ========================================================================
		//  UI 그룹 로딩
		// ========================================================================
		Json::Value& groupListRoot = root[JsonGroupsKey];

		for (int i = 0; i < groupListRoot.size(); ++i)
		{
			Json::Value& groupRoot = groupListRoot[i];
			Json::Value& groupElementInfoListRoot = groupRoot[JsonChildrenKey];

			// 키값이 "groups" = []으로 들어가버린경우
			_LogWarnIf_(groupElementInfoListRoot.empty(), "그룹에 자식이 없습니다.");

			UIGroupInfo* pGroupInfo = dbg_new UIGroupInfo(groupElementInfoListRoot.size());

			readElementCommon(groupRoot, pGroupInfo);
			readElementGroup(groupRoot, pGroupInfo);
			AddData(pGroupInfo);
		}

		// ========================================================================
		//  UI 그룹마스터 로딩
		// ========================================================================
		Json::Value& groupMasterRoot = root[JsonGroupMasterKey];
		Json::Value& groupMasterElementInfoListRoot = groupMasterRoot[JsonChildrenKey];

		UIGroupInfo* pGroupMasterInfo = dbg_new UIGroupInfo(groupMasterElementInfoListRoot.size());

		readElementCommon(groupMasterRoot, pGroupMasterInfo);
		readElementGroup(groupMasterRoot, pGroupMasterInfo);
		AddData(pGroupMasterInfo);
	}
	catch (std::exception& ex)
	{
		_LogError_("%s 파싱중 오류가 발생하였습니다. %s", GetConfigFileName(), ex.what());
		return false;
	}

	return true;
}

//////////////////////////////////////////////////////////////////////////////////////////
void UIInfoLoader::readElementCommon(Json::Value& _elementRoot, UIElementInfo* _pElementInfo)
{
	_pElementInfo->code_ = _elementRoot[JsonCodeKey].asInt();
	_pElementInfo->HAlignment = (HAlignment_t)_elementRoot[JsonHAlignKey].asInt();
	_pElementInfo->VAlignment = (VAlignment_t)_elementRoot[JsonVAlignKey].asInt();
}

//////////////////////////////////////////////////////////////////////////////////////////
void UIInfoLoader::readElementGroup(Json::Value& _groupRoot, UIGroupInfo* _pGroupInfo)
{
	JsonUtilEx::ParseFloatNumber2(_groupRoot[JsonVisualSizeKey], _pGroupInfo->Size.width, _pGroupInfo->Size.height);
	_pGroupInfo->Type = UIElementType::Group;

	Json::Value& groupElementInfoListRoot = _groupRoot[JsonChildrenKey];

	for (int i = 0; i < groupElementInfoListRoot.size(); ++i)
	{
		Json::Value& groupElementInfoRoot = groupElementInfoListRoot[i];
		UIGroupElemInfo groupElementInfo;
		int groupElementInfoData[3];

		JsonUtilEx::ParseIntNumberN(groupElementInfoRoot, groupElementInfoData, 3);

		groupElementInfo.Code = groupElementInfoData[0];
		groupElementInfo.Pos.x = (float)groupElementInfoData[1];
		groupElementInfo.Pos.y = (float)groupElementInfoData[2];

		_pGroupInfo->InfoList.PushBack(groupElementInfo);
	}
}

//////////////////////////////////////////////////////////////////////////////////////////
UIElementInfo* UIInfoLoader::readElementButton(Json::Value& _buttonRoot)
{
	UIButtonInfo* pInfo = dbg_new UIButtonInfo();

	readElementCommon(_buttonRoot, pInfo);

	ImagePackManager* pPackManager = ImagePackManager::Get();
	const SGString& sgaName = JsonUtilEx::GetString(_buttonRoot[JsonSgaKey]);
	const SGString& imageName = JsonUtilEx::GetString(_buttonRoot[JsonImgKey]);

	ImagePack* pPack = pPackManager->getPack(sgaName);

	pInfo->Sga = pPack->getPackIndex();
	pInfo->Img = pPack->getImgIndex(imageName);

	pInfo->Type = UIElementType::Button;
	pInfo->LinearDodge = _buttonRoot[JsonLinearDodgeKey].asBool();
	JsonUtilEx::ParseIntNumberN(_buttonRoot[JsonSpriteKey], pInfo->Sprites, 4);

	return pInfo;
}

//////////////////////////////////////////////////////////////////////////////////////////
UIElementInfo* UIInfoLoader::readElementLabel(Json::Value& _labelRoot)
{
	UILabelInfo* pInfo = dbg_new UILabelInfo();
	DataManager* pDataManager = DataManager::Get();
	(void)pDataManager;

	readElementCommon(_labelRoot, pInfo);

	const SGString fontName = JsonUtilEx::GetString(_labelRoot[JsonFontKey]);

	pInfo->Type = UIElementType::Label;
	pInfo->FontCode = FontManager::Get()->getFontCode(fontName);
	pInfo->FontSize = _labelRoot[JsonFontSizeKey].asInt();
	pInfo->TextWrap = _labelRoot[JsonTextWrapKey].asBool();
	JsonUtilEx::ParseColor4B(_labelRoot[JsonFontColorKey], pInfo->FontColor);
	JsonUtilEx::ParseSize(_labelRoot[JsonVisualSizeKey], pInfo->Size);
	pInfo->TextHAlignment = (HAlignment_t)_labelRoot[JsonTextHAlignKey].asInt();
	pInfo->TextVAlignment = (VAlignment_t)_labelRoot[JsonTextVAlignKey].asInt();
	pInfo->Text = JsonUtilEx::GetString(_labelRoot[JsonTextKey]);

	return pInfo;
}

//////////////////////////////////////////////////////////////////////////////////////////
UIElementInfo* UIInfoLoader::readElementSprite(Json::Value& _spriteRoot)
{
	UISpriteInfo* pInfo = dbg_new UISpriteInfo();

	readElementCommon(_spriteRoot, pInfo);

	ImagePackManager* pPackManager = ImagePackManager::Get();

	const SGString& sgaName = JsonUtilEx::GetString(_spriteRoot[JsonSgaKey]);
	const SGString& imageName = JsonUtilEx::GetString(_spriteRoot[JsonImgKey]);

	ImagePack* pPack = pPackManager->getPack(sgaName);

	pInfo->Type = UIElementType::Sprite;
	pInfo->Sga = pPack->getPackIndex();
	pInfo->Img = pPack->getImgIndex(imageName);
	pInfo->Sprite = _spriteRoot[JsonSpriteKey].asInt();
	pInfo->LinearDodge = _spriteRoot.get(JsonLinearDodgeKey, false).asBool();
	pInfo->Scale9 = _spriteRoot.get(JsonScale9, false).asBool();
	JsonUtilEx::ParseSize(_spriteRoot[JsonVisualSizeKey], pInfo->Size);

	return pInfo;
}

//////////////////////////////////////////////////////////////////////////////////////////
UIElementInfo* UIInfoLoader::readElementEditBox(Json::Value& _editBoxRoot)
{
	UIEditBoxInfo* pInfo = dbg_new UIEditBoxInfo();

	readElementCommon(_editBoxRoot, pInfo);

	ImagePackManager* pPackManager = ImagePackManager::Get();
	(void)pPackManager;

	pInfo->Type = UIElementType::EditBox;
	JsonUtilEx::ParseSize(_editBoxRoot[JsonVisualSizeKey], pInfo->Size);
	pInfo->FontSize = _editBoxRoot[JsonFontSizeKey].asInt();
	pInfo->TextHAlignment = (HAlignment_t)_editBoxRoot[JsonTextHAlignKey].asInt();
	JsonUtilEx::ParseColor4B(_editBoxRoot[JsonFontColorKey], pInfo->FontColor);

	pInfo->PlaceholderText = JsonUtilEx::GetString(_editBoxRoot[JsonPlaceholderTextKey]);
	JsonUtilEx::ParseColor4B(_editBoxRoot[JsonPlaceholderFontColorKey], pInfo->PlaceHolderFontColor);
	pInfo->PlaceholderFontSize = _editBoxRoot[JsonPlaceholderFontSizeKey].asInt();
	pInfo->MaxLength = _editBoxRoot[JsonMaxLengthKey].asInt();
	pInfo->InputMode = SGInputMode(_editBoxRoot[JsonInputModeKey].asInt());

	return pInfo;
}

//////////////////////////////////////////////////////////////////////////////////////////
UIElementInfo* UIInfoLoader::readElementCheckBox(Json::Value& _checkBoxRoot)
{
	UICheckBoxInfo* pInfo = dbg_new UICheckBoxInfo();

	readElementCommon(_checkBoxRoot, pInfo);

	ImagePackManager* pPackManager = ImagePackManager::Get();

	const SGString backgroundSgaName = JsonUtilEx::GetString(_checkBoxRoot[JsonBackgroundSga]);
	const SGString backgroundImageName = JsonUtilEx::GetString(_checkBoxRoot[JsonBackgroundImg]);

	const SGString crossSgaName = JsonUtilEx::GetString(_checkBoxRoot[JsonCrossSga]);
	const SGString crossImageName = JsonUtilEx::GetString(_checkBoxRoot[JsonCrossImg]);

	ImagePack* pBackgroundPack = pPackManager->getPackUnsafe(backgroundSgaName); // 백그라운드는 sga이름이 빈 문자열 일 수 있음
	ImagePack* pCrossPack = pPackManager->getPack(crossSgaName);

	pInfo->Type = UIElementType::CheckBox;
	pInfo->Check = _checkBoxRoot.get(JsonCheck, false).asBool();

	if (pBackgroundPack != nullptr)
	{
		pInfo->BackgroundSga = pBackgroundPack->getPackIndex();
		pInfo->BackgroundImg = pBackgroundPack->getImgIndex(backgroundImageName);
	}

	pInfo->CrossSga = pCrossPack->getPackIndex();
	pInfo->CrossImg = pCrossPack->getImgIndex(crossImageName);
	JsonUtilEx::ParseIntNumberN(_checkBoxRoot[JsonSpriteKey], pInfo->Sprites, 4);

	DebugAssertMsg(pInfo->Sprites[UICheckBox::IndexCross] != InvalidValue_v, "체크박스인데 크로스 이미지가 설정되어있지 않습니다.");

	return pInfo;
}

//////////////////////////////////////////////////////////////////////////////////////////
UIElementInfo* UIInfoLoader::readElementToggleButton(Json::Value& _toggleButtonRoot)
{
	UIToggleButtonInfo* pInfo = dbg_new UIToggleButtonInfo();

	readElementCommon(_toggleButtonRoot, pInfo);

	ImagePackManager* pPackManager = ImagePackManager::Get();

	const SGString sgaName = JsonUtilEx::GetString(_toggleButtonRoot[JsonSgaKey]);
	const SGString imageName = JsonUtilEx::GetString(_toggleButtonRoot[JsonImgKey]);

	ImagePack* pPack = pPackManager->getPack(sgaName);

	pInfo->Type = UIElementType::ToggleButton;
	pInfo->LinearDodge = _toggleButtonRoot[JsonLinearDodgeKey].asBool();
	pInfo->Sga = pPack->getPackIndex();
	pInfo->Img = pPack->getImgIndex(imageName);

	JsonUtilEx::ParseIntNumberN(_toggleButtonRoot[JsonSpriteKey], pInfo->Sprites[0], 4);
	JsonUtilEx::ParseIntNumberN(_toggleButtonRoot[JsonToggleSpriteKey], pInfo->Sprites[1], 4);

	return pInfo;
}

//////////////////////////////////////////////////////////////////////////////////////////
UIElementInfo* UIInfoLoader::readElementScrollBar(Json::Value& _scrollBarRoot)
{
	UIScrollBarInfo* pInfo = dbg_new UIScrollBarInfo();

	readElementCommon(_scrollBarRoot, pInfo);

	ImagePackManager* pPackManager = ImagePackManager::Get();

	const SGString sgaName = JsonUtilEx::GetString(_scrollBarRoot[JsonSgaKey]);
	const SGString imageName = JsonUtilEx::GetString(_scrollBarRoot[JsonImgKey]);

	ImagePack* pPack = pPackManager->getPack(sgaName);

	pInfo->Type = UIElementType::ScrollBar;
	pInfo->Sga = pPack->getPackIndex();
	pInfo->Img = pPack->getImgIndex(imageName);
	JsonUtilEx::ParseSize(_scrollBarRoot[JsonTrackSizeKey], pInfo->TrackSize);
	JsonUtilEx::ParseIntNumberN(_scrollBarRoot[JsonSpriteKey], pInfo->Sprites, 7);

	return pInfo;
}

//////////////////////////////////////////////////////////////////////////////////////////
UIElementInfo* UIInfoLoader::readElementProgressBar(Json::Value& _progressBarRoot)
{
	UIProgressBarInfo* pInfo = dbg_new UIProgressBarInfo();

	readElementCommon(_progressBarRoot, pInfo);

	ImagePackManager* pPackManager = ImagePackManager::Get();

	const SGString sgaName = JsonUtilEx::GetString(_progressBarRoot[JsonSgaKey]);
	const SGString imageName = JsonUtilEx::GetString(_progressBarRoot[JsonImgKey]);

	ImagePack* pPack = pPackManager->getPack(sgaName);

	pInfo->Type = UIElementType::ProgressBar;
	pInfo->Sga = pPack->getPackIndex();
	pInfo->Img = pPack->getImgIndex(imageName);
	pInfo->Sprite = _progressBarRoot[JsonSpriteKey].asInt();
	JsonUtilEx::ParseSize(_progressBarRoot[JsonVisualSizeKey], pInfo->Size);
	pInfo->ProgressIncreaseDirection = (ProgressIncreaseDirection_t)_progressBarRoot[JsonDirectionKey].asInt();

	return pInfo;
}

//////////////////////////////////////////////////////////////////////////////////////////
UIElementInfo* UIInfoLoader::readElementStatic(Json::Value& _staticRoot)
{
	UIStaticInfo* pInfo = dbg_new UIStaticInfo();

	readElementCommon(_staticRoot, pInfo);

	pInfo->Type = UIElementType::Static;
	JsonUtilEx::ParseSize(_staticRoot[JsonVisualSizeKey], pInfo->Size);

	return pInfo;
}
