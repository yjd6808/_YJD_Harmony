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
#include <SteinsGate/Client/FontPackage.h>

#include "UICheckBox.h"

USING_NS_JC;
USING_NS_JS;

UIInfoLoader::UIInfoLoader(DataManagerAbstract* manager)
	: ConfigFileLoaderAbstract(manager)
{}

bool UIInfoLoader::load()
{
	Value root;

	if (!loadJson(root))
		return false;
	try {

		// ========================================================================
		//  UI 엘리먼트 로딩
		// ========================================================================
		Value& uiElementListRoot = root[JsonElementsKey];
		for (int i = 0; i < uiElementListRoot.size(); ++i) {
			Value& uiElementRoot = uiElementListRoot[i];

			UIElementInfo* pElemInfo = nullptr;
			int eElemType = uiElementRoot[JsonElementTypeKey].asInt();

			switch (eElemType) {
			case UIElementType::Button: pElemInfo = readElementButton(uiElementRoot); break;
			case UIElementType::Label: pElemInfo = readElementLabel(uiElementRoot); break;
			case UIElementType::Sprite: pElemInfo = readElementSprite(uiElementRoot); break;
			case UIElementType::EditBox: pElemInfo = readElementEditBox(uiElementRoot); break;
			case UIElementType::CheckBox: pElemInfo = readElementCheckBox(uiElementRoot); break;
			case UIElementType::ToggleButton: pElemInfo = readElementToggleButton(uiElementRoot); break;
			case UIElementType::ScrollBar: pElemInfo = readElementScrollBar(uiElementRoot); break;
			case UIElementType::ProgressBar: pElemInfo = readElementProgressBar(uiElementRoot); break;
			case UIElementType::Static: pElemInfo = readElementStatic(uiElementRoot); break;
			default: break;
			}

			DebugAssertMsg(pElemInfo != nullptr, "UI 엘리먼트 로딩에 실패했습니다.");
			addData(pElemInfo);
		}


		// ========================================================================
		//  UI 그룹 로딩
		// ========================================================================
		Value& uiGroupListRoot = root[JsonGroupsKey];
		for (int i = 0; i < uiGroupListRoot.size(); ++i) {
			Value& uiGroupRoot = uiGroupListRoot[i];
			Value& groupElemInfoListRoot = uiGroupRoot[JsonChildrenKey];

			// 키값이 "groups" = []으로 들어가버린경우
			_LogWarnIf_(groupElemInfoListRoot.empty(), "그룹에 자식이 없습니다.");
			UIGroupInfo* groupInfo = dbg_new UIGroupInfo(groupElemInfoListRoot.size());

			readElementCommon(uiGroupRoot, groupInfo);
			readElementGroup(uiGroupRoot, groupInfo);
			addData(groupInfo);
		}

		// ========================================================================
		//  UI 그룹마스터 로딩
		// ========================================================================
		Value& groupMasterRoot = root[JsonGroupMasterKey];
		Value& groupElemInfoListRoot = groupMasterRoot[JsonChildrenKey];
		UIGroupInfo* groupInfo = dbg_new UIGroupInfo(groupElemInfoListRoot.size());
		readElementCommon(groupMasterRoot, groupInfo);
		readElementGroup(groupMasterRoot, groupInfo);
		addData(groupInfo);
	}
	catch (std::exception& ex) {
		_LogError_("%s 파싱중 오류가 발생하였습니다. %s", getConfigFileName(), ex.what());
		return false;
	}

	return true;
}



void UIInfoLoader::readElementCommon(Value& elementRoot, UIElementInfo* elementInfo) {
	elementInfo->Code = elementRoot[JsonCodeKey].asInt();
	elementInfo->HAlignment = (HAlignment_t)elementRoot[JsonHAlignKey].asInt();
	elementInfo->VAlignment = (VAlignment_t)elementRoot[JsonVAlignKey].asInt();
}

void UIInfoLoader::readElementGroup(Value& uiGroupRoot, UIGroupInfo* groupInfo) {
	JsonUtilEx::parseFloatNumber2(uiGroupRoot[JsonVisualSizeKey], groupInfo->Size.width, groupInfo->Size.height);
	groupInfo->Type = UIElementType::Group;

	Value& groupElemInfoListRoot = uiGroupRoot[JsonChildrenKey];
	for (int j = 0; j < groupElemInfoListRoot.size(); ++j) {
		Value& groupElemInfoRoot = groupElemInfoListRoot[j];
		UIGroupElemInfo groupElemInfo;
		int groupElemInfoData[3];
		JsonUtilEx::parseIntNumberN(groupElemInfoRoot, groupElemInfoData, 3);

		groupElemInfo.Code = groupElemInfoData[0];
		groupElemInfo.Pos.x = (float)groupElemInfoData[1];
		groupElemInfo.Pos.y = (float)groupElemInfoData[2];
		groupInfo->InfoList.PushBack(groupElemInfo);
	}
	
}


UIElementInfo* UIInfoLoader::readElementButton(Value& buttonRoot) {
	UIButtonInfo* pInfo = dbg_new UIButtonInfo();

	readElementCommon(buttonRoot, pInfo);

	ImagePackManager* pPackManager = ImagePackManager::Get();
	const SGString& sgaName = JsonUtilEx::getString(buttonRoot[JsonSgaKey]);
	const SGString& imgName = JsonUtilEx::getString(buttonRoot[JsonImgKey]);

	ImagePack* pPack = pPackManager->getPack(sgaName);

	pInfo->Sga = pPack->getPackIndex();
	pInfo->Img = pPack->getImgIndex(imgName);

	pInfo->Type = UIElementType::Button;
	pInfo->LinearDodge = buttonRoot[JsonLinearDodgeKey].asBool();
	JsonUtilEx::parseIntNumberN(buttonRoot[JsonSpriteKey], pInfo->Sprites, 4);
	return pInfo;
}

UIElementInfo* UIInfoLoader::readElementLabel(Value& labelRoot) {
	UILabelInfo* pInfo = dbg_new UILabelInfo();
	DataManager* pDataManager = DataManager::Get();

	readElementCommon(labelRoot, pInfo);

	const SGString szFontName = JsonUtilEx::getString(labelRoot[JsonFontKey]);

	pInfo->Type = UIElementType::Label;
	pInfo->FontCode =  FontPackage::Get()->getFontCode(szFontName);
	pInfo->FontSize = labelRoot[JsonFontSizeKey].asInt();
	pInfo->TextWrap = labelRoot[JsonTextWrapKey].asBool();
	JsonUtilEx::parseColor4B(labelRoot[JsonFontColorKey], pInfo->FontColor);
	JsonUtilEx::parseSize(labelRoot[JsonVisualSizeKey], pInfo->Size);
	pInfo->TextHAlignment = (HAlignment_t)labelRoot[JsonTextHAlignKey].asInt();
	pInfo->TextVAlignment = (VAlignment_t)labelRoot[JsonTextVAlignKey].asInt();
	pInfo->Text = JsonUtilEx::getString(labelRoot[JsonTextKey]);

	return pInfo;
}

UIElementInfo* UIInfoLoader::readElementSprite(Value& spriteRoot) {
	UISpriteInfo* pInfo = dbg_new UISpriteInfo();

	readElementCommon(spriteRoot, pInfo);

	ImagePackManager* pPackManager = ImagePackManager::Get();

	const SGString& sgaName = JsonUtilEx::getString(spriteRoot[JsonSgaKey]);
	const SGString& imgName = JsonUtilEx::getString(spriteRoot[JsonImgKey]);

	ImagePack* pPack = pPackManager->getPack(sgaName);

	pInfo->Type = UIElementType::Sprite;
	pInfo->Sga = pPack->getPackIndex();
	pInfo->Img = pPack->getImgIndex(imgName);
	pInfo->Sprite = spriteRoot[JsonSpriteKey].asInt();
	pInfo->LinearDodge = spriteRoot.get(JsonLinearDodgeKey, false).asBool();
	pInfo->Scale9 = spriteRoot.get(JsonScale9, false).asBool();
	JsonUtilEx::parseSize(spriteRoot[JsonVisualSizeKey], pInfo->Size);

	return pInfo;
}

UIElementInfo* UIInfoLoader::readElementEditBox(Value& editBoxRoot) {
	UIEditBoxInfo* pInfo = dbg_new UIEditBoxInfo();

	readElementCommon(editBoxRoot, pInfo);

	ImagePackManager* pPackManager = ImagePackManager::Get();

	pInfo->Type = UIElementType::EditBox;
	JsonUtilEx::parseSize(editBoxRoot[JsonVisualSizeKey], pInfo->Size);
	pInfo->FontSize = editBoxRoot[JsonFontSizeKey].asInt();
	pInfo->TextHAlignment = HAlignment_t(editBoxRoot[JsonTextHAlignKey].asInt());
	JsonUtilEx::parseColor4B(editBoxRoot[JsonFontColorKey], pInfo->FontColor);

	pInfo->PlaceholderText = JsonUtilEx::getString(editBoxRoot[JsonPlaceholderTextKey]);
	JsonUtilEx::parseColor4B(editBoxRoot[JsonPlaceholderFontColorKey], pInfo->PlaceHolderFontColor);
	pInfo->PlaceholderFontSize = editBoxRoot[JsonPlaceholderFontSizeKey].asInt();
	pInfo->MaxLength = editBoxRoot[JsonMaxLengthKey].asInt();
	pInfo->InputMode = SGInputMode(editBoxRoot[JsonInputModeKey].asInt());

	return pInfo;
}

UIElementInfo* UIInfoLoader::readElementCheckBox(Value& checkBoxRoot) {
	UICheckBoxInfo* pInfo = dbg_new UICheckBoxInfo();
	readElementCommon(checkBoxRoot, pInfo);

	ImagePackManager* pPackManager = ImagePackManager::Get();

	const SGString sgaNameBackground = JsonUtilEx::getString(checkBoxRoot[JsonBackgroundSga]);
	const SGString imgNameBackground = JsonUtilEx::getString(checkBoxRoot[JsonBackgroundImg]);

	const SGString sgaNameCross = JsonUtilEx::getString(checkBoxRoot[JsonCrossSga]);
	const SGString imgNameCross = JsonUtilEx::getString(checkBoxRoot[JsonCrossImg]);

	ImagePack* pPackBackground = pPackManager->getPackUnsafe(sgaNameBackground);	// 백그라운드는 sga이름이 빈 문자열 일 수 있음
	ImagePack* pPackCross = pPackManager->getPack(sgaNameCross);

	pInfo->Type = UIElementType::CheckBox;
	pInfo->Check = checkBoxRoot.get(JsonCheck, false).asBool();

	if (pPackBackground != nullptr) {
		pInfo->BackgroundSga = pPackBackground->getPackIndex();
		pInfo->BackgroundImg = pPackBackground->getImgIndex(imgNameBackground);
	}

	pInfo->CrossSga = pPackCross->getPackIndex();
	pInfo->CrossImg = pPackCross->getImgIndex(imgNameCross);
	JsonUtilEx::parseIntNumberN(checkBoxRoot[JsonSpriteKey], pInfo->Sprites, 4);
	
	DebugAssertMsg(pInfo->Sprites[UICheckBox::IndexCross] != InvalidValue_v, "체크박스인데 크로스 이미지가 설정되어있지 않습니다.");

	return pInfo;
}

UIElementInfo* UIInfoLoader::readElementToggleButton(Value& toggleButtonRoot) {
	UIToggleButtonInfo* pInfo = dbg_new UIToggleButtonInfo();
	readElementCommon(toggleButtonRoot, pInfo);

	ImagePackManager* pPackManager = ImagePackManager::Get();

	const SGString sgaName = JsonUtilEx::getString(toggleButtonRoot[JsonSgaKey]);
	const SGString imgName = JsonUtilEx::getString(toggleButtonRoot[JsonImgKey]);

	ImagePack* pPack = pPackManager->getPack(sgaName);

	pInfo->Type = UIElementType::ToggleButton;
	pInfo->LinearDodge = toggleButtonRoot[JsonLinearDodgeKey].asBool();
	pInfo->Sga = pPack->getPackIndex();
	pInfo->Img = pPack->getImgIndex(imgName);

	JsonUtilEx::parseIntNumberN(toggleButtonRoot[JsonSpriteKey], pInfo->Sprites[0], 4);
	JsonUtilEx::parseIntNumberN(toggleButtonRoot[JsonToggleSpriteKey], pInfo->Sprites[1], 4);
	return pInfo;
}

UIElementInfo* UIInfoLoader::readElementScrollBar(Value& scrollBarRoot) {
	UIScrollBarInfo* pInfo = dbg_new UIScrollBarInfo();
	readElementCommon(scrollBarRoot, pInfo);

	ImagePackManager* pPackManager = ImagePackManager::Get();

	const SGString sgaName = JsonUtilEx::getString(scrollBarRoot[JsonSgaKey]);
	const SGString imgName = JsonUtilEx::getString(scrollBarRoot[JsonImgKey]);

	ImagePack* pPack = pPackManager->getPack(sgaName);

	pInfo->Type = UIElementType::ScrollBar;
	pInfo->Sga = pPack->getPackIndex();
	pInfo->Img = pPack->getImgIndex(imgName);
	JsonUtilEx::parseSize(scrollBarRoot[JsonTrackSizeKey], pInfo->TrackSize);
	JsonUtilEx::parseIntNumberN(scrollBarRoot[JsonSpriteKey], pInfo->Sprites, 7);
	return pInfo;
}

UIElementInfo* UIInfoLoader::readElementProgressBar(Value& progressBarRoot) {
	UIProgressBarInfo* pInfo = dbg_new UIProgressBarInfo();
	readElementCommon(progressBarRoot, pInfo);

	ImagePackManager* pPackManager = ImagePackManager::Get();

	const SGString sgaName = JsonUtilEx::getString(progressBarRoot[JsonSgaKey]);
	const SGString imgName = JsonUtilEx::getString(progressBarRoot[JsonImgKey]);

	ImagePack* pPack = pPackManager->getPack(sgaName);

	pInfo->Type = UIElementType::ProgressBar;
	pInfo->Sga = pPack->getPackIndex();
	pInfo->Img = pPack->getImgIndex(imgName);
	pInfo->Sprite = progressBarRoot[JsonSpriteKey].asInt();
	JsonUtilEx::parseSize(progressBarRoot[JsonVisualSizeKey], pInfo->Size);
	pInfo->ProgressIncreaseDirection = (ProgressIncreaseDirection_t)progressBarRoot[JsonDirectionKey].asInt();
	return pInfo;
}

UIElementInfo* UIInfoLoader::readElementStatic(Value& staticRoot) {
	UIStaticInfo* pInfo = dbg_new UIStaticInfo();
	readElementCommon(staticRoot, pInfo);

	pInfo->Type = UIElementType::Static;
	JsonUtilEx::parseSize(staticRoot[JsonVisualSizeKey], pInfo->Size);
	return pInfo;
}
