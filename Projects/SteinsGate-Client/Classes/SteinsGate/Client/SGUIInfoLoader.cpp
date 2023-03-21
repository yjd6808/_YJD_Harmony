/*
 * 작성자: 윤정도
 * 생성일: 2/15/2023 1:30:20 AM
 * =====================
 *
 */



#include "Tutturu.h"
#include "SGUIInfoLoader.h"

#include <SteinsGate/Client/SGImagePackManager.h>
#include <SteinsGate/Client/SGJsonEx.h>
#include <SteinsGate/Client/SGDataManager.h>
#include <SteinsGate/Client/SGFontPackage.h>

#include "SGUICheckBox.h"

USING_NS_JC;
USING_NS_JS;

SGUIInfoLoader::SGUIInfoLoader(DataManagerAbstract* manager)
	: ConfigFileLoaderAbstract(manager)
{}

bool SGUIInfoLoader::load()
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

			SGUIElementInfo* pElemInfo = nullptr;
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

			DebugAssertMsg(groupElemInfoListRoot.size() != 0, "그룹에 자식이 없습니다.");
			SGUIGroupInfo* groupInfo = dbg_new SGUIGroupInfo(groupElemInfoListRoot.size());

			readElementCommon(uiGroupRoot, groupInfo);
			readElementGroup(uiGroupRoot, groupInfo);
			addData(groupInfo);
		}

		// ========================================================================
		//  UI 그룹마스터 로딩
		// ========================================================================
		Value& groupMasterRoot = root[JsonGroupMasterKey];
		Value& groupElemInfoListRoot = groupMasterRoot[JsonChildrenKey];
		SGUIGroupInfo* groupInfo = dbg_new SGUIGroupInfo(groupElemInfoListRoot.size());
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



void SGUIInfoLoader::readElementCommon(Value& elementRoot, SGUIElementInfo* elementInfo) {
	elementInfo->Code = elementRoot[JsonCodeKey].asInt();
	elementInfo->HAlignment = (HAlignment_t)elementRoot[JsonHAlignKey].asInt();
	elementInfo->VAlignment = (VAlignment_t)elementRoot[JsonVAlignKey].asInt();
}

void SGUIInfoLoader::readElementGroup(Value& uiGroupRoot, SGUIGroupInfo* groupInfo) {
	SGJsonEx::parseFloatNumber2(uiGroupRoot[JsonVisualSizeKey], groupInfo->Size.width, groupInfo->Size.height);
	groupInfo->Type = UIElementType::Group;

	Value& groupElemInfoListRoot = uiGroupRoot[JsonChildrenKey];
	for (int j = 0; j < groupElemInfoListRoot.size(); ++j) {
		Value& groupElemInfoRoot = groupElemInfoListRoot[j];
		SGUIGroupElemInfo groupElemInfo;
		int groupElemInfoData[3];
		SGJsonEx::parseIntNumberN(groupElemInfoRoot, groupElemInfoData, 3);

		groupElemInfo.Code = groupElemInfoData[0];
		groupElemInfo.Pos.x = (float)groupElemInfoData[1];
		groupElemInfo.Pos.y = (float)groupElemInfoData[2];
		groupInfo->InfoList.PushBack(groupElemInfo);
	}
	
}


SGUIElementInfo* SGUIInfoLoader::readElementButton(Value& buttonRoot) {
	SGUIButtonInfo* pInfo = dbg_new SGUIButtonInfo();

	readElementCommon(buttonRoot, pInfo);

	SGImagePackManager* pPackManager = SGImagePackManager::get();
	const SGString& sgaName = SGJsonEx::getString(buttonRoot[JsonSgaKey]);
	const SGString& imgName = SGJsonEx::getString(buttonRoot[JsonImgKey]);

	SGImagePack* pPack = pPackManager->getPack(sgaName);

	pInfo->Sga = pPack->getPackIndex();
	pInfo->Img = pPack->getImgIndex(imgName);

	pInfo->Type = UIElementType::Button;
	pInfo->LinearDodge = buttonRoot[JsonLinearDodgeKey].asBool();
	SGJsonEx::parseIntNumberN(buttonRoot[JsonSpriteKey], pInfo->Sprites, 4);
	return pInfo;
}

SGUIElementInfo* SGUIInfoLoader::readElementLabel(Value& labelRoot) {
	SGUILabelInfo* pInfo = dbg_new SGUILabelInfo();
	SGDataManager* pDataManager = SGDataManager::get();

	readElementCommon(labelRoot, pInfo);

	const SGString szFontName = SGJsonEx::getString(labelRoot[JsonFontKey]);

	pInfo->Type = UIElementType::Label;
	pInfo->FontCode =  SGFontPackage::get()->getFontCode(szFontName);
	pInfo->FontSize = labelRoot[JsonFontSizeKey].asInt();
	pInfo->TextWrap = labelRoot[JsonTextWrapKey].asBool();
	SGJsonEx::parseColor4B(labelRoot[JsonFontColorKey], pInfo->FontColor);
	SGJsonEx::parseSize(labelRoot[JsonVisualSizeKey], pInfo->Size);
	pInfo->TextHAlignment = (HAlignment_t)labelRoot[JsonTextHAlignKey].asInt();
	pInfo->TextVAlignment = (VAlignment_t)labelRoot[JsonTextVAlignKey].asInt();
	pInfo->Text = SGJsonEx::getString(labelRoot[JsonTextKey]);

	return pInfo;
}

SGUIElementInfo* SGUIInfoLoader::readElementSprite(Value& spriteRoot) {
	SGUISpriteInfo* pInfo = dbg_new SGUISpriteInfo();

	readElementCommon(spriteRoot, pInfo);

	SGImagePackManager* pPackManager = SGImagePackManager::get();

	const SGString& sgaName = SGJsonEx::getString(spriteRoot[JsonSgaKey]);
	const SGString& imgName = SGJsonEx::getString(spriteRoot[JsonImgKey]);

	SGImagePack* pPack = pPackManager->getPack(sgaName);

	pInfo->Type = UIElementType::Sprite;
	pInfo->Sga = pPack->getPackIndex();
	pInfo->Img = pPack->getImgIndex(imgName);
	pInfo->Sprite = spriteRoot[JsonSpriteKey].asInt();
	pInfo->LinearDodge = spriteRoot[JsonLinearDodgeKey].asBool();
	SGJsonEx::parseSize(spriteRoot[JsonVisualSizeKey], pInfo->Size);

	return pInfo;
}

SGUIElementInfo* SGUIInfoLoader::readElementEditBox(Value& editBoxRoot) {
	SGUIEditBoxInfo* pInfo = dbg_new SGUIEditBoxInfo();

	readElementCommon(editBoxRoot, pInfo);

	SGImagePackManager* pPackManager = SGImagePackManager::get();

	pInfo->Type = UIElementType::EditBox;
	SGJsonEx::parseSize(editBoxRoot[JsonVisualSizeKey], pInfo->Size);
	pInfo->FontSize = editBoxRoot[JsonFontSizeKey].asInt();
	pInfo->TextHAlignment = HAlignment_t(editBoxRoot[JsonTextHAlignKey].asInt());
	SGJsonEx::parseColor4B(editBoxRoot[JsonFontColorKey], pInfo->FontColor);

	pInfo->PlaceholderText = SGJsonEx::getString(editBoxRoot[JsonPlaceholderTextKey]);
	SGJsonEx::parseColor4B(editBoxRoot[JsonPlaceholderFontColorKey], pInfo->PlaceHolderFontColor);
	pInfo->PlaceholderFontSize = editBoxRoot[JsonPlaceholderFontSizeKey].asInt();
	pInfo->MaxLength = editBoxRoot[JsonMaxLengthKey].asInt();
	pInfo->InputMode = SGInputMode(editBoxRoot[JsonInputModeKey].asInt());

	return pInfo;
}

SGUIElementInfo* SGUIInfoLoader::readElementCheckBox(Value& checkBoxRoot) {
	SGUICheckBoxInfo* pInfo = dbg_new SGUICheckBoxInfo();
	readElementCommon(checkBoxRoot, pInfo);

	SGImagePackManager* pPackManager = SGImagePackManager::get();

	const SGString sgaNameBackground = SGJsonEx::getString(checkBoxRoot[JsonBackgroundSga]);
	const SGString imgNameBackground = SGJsonEx::getString(checkBoxRoot[JsonBackgroundImg]);

	const SGString sgaNameCross = SGJsonEx::getString(checkBoxRoot[JsonCrossSga]);
	const SGString imgNameCross = SGJsonEx::getString(checkBoxRoot[JsonCrossImg]);

	SGImagePack* pPackBackground = pPackManager->getPackUnsafe(sgaNameBackground);	// 백그라운드는 sga이름이 빈 문자열 일 수 있음
	SGImagePack* pPackCross = pPackManager->getPack(sgaNameCross);

	pInfo->Type = UIElementType::CheckBox;

	if (pPackBackground != nullptr) {
		pInfo->BackgroundSga = pPackBackground->getPackIndex();
		pInfo->BackgroundImg = pPackBackground->getImgIndex(imgNameBackground);
	}

	pInfo->CrossSga = pPackCross->getPackIndex();
	pInfo->CrossImg = pPackCross->getImgIndex(imgNameCross);
	SGJsonEx::parseIntNumberN(checkBoxRoot[JsonSpriteKey], pInfo->Sprites, 4);
	
	DebugAssertMsg(pInfo->Sprites[SGUICheckBox::IndexCross] != InvalidValue_v, "체크박스인데 크로스 이미지가 설정되어있지 않습니다.");

	return pInfo;
}

SGUIElementInfo* SGUIInfoLoader::readElementToggleButton(Value& toggleButtonRoot) {
	SGUIToggleButtonInfo* pInfo = dbg_new SGUIToggleButtonInfo();
	readElementCommon(toggleButtonRoot, pInfo);

	SGImagePackManager* pPackManager = SGImagePackManager::get();

	const SGString sgaName = SGJsonEx::getString(toggleButtonRoot[JsonSgaKey]);
	const SGString imgName = SGJsonEx::getString(toggleButtonRoot[JsonImgKey]);

	SGImagePack* pPack = pPackManager->getPack(sgaName);

	pInfo->Type = UIElementType::ToggleButton;
	pInfo->LinearDodge = toggleButtonRoot[JsonLinearDodgeKey].asBool();
	pInfo->Sga = pPack->getPackIndex();
	pInfo->Img = pPack->getImgIndex(imgName);

	SGJsonEx::parseIntNumberN(toggleButtonRoot[JsonSpriteKey], pInfo->Sprites[0], 4);
	SGJsonEx::parseIntNumberN(toggleButtonRoot[JsonToggleSpriteKey], pInfo->Sprites[1], 4);
	return pInfo;
}

SGUIElementInfo* SGUIInfoLoader::readElementScrollBar(Value& scrollBarRoot) {
	SGUIScrollBarInfo* pInfo = dbg_new SGUIScrollBarInfo();
	readElementCommon(scrollBarRoot, pInfo);

	SGImagePackManager* pPackManager = SGImagePackManager::get();

	const SGString sgaName = SGJsonEx::getString(scrollBarRoot[JsonSgaKey]);
	const SGString imgName = SGJsonEx::getString(scrollBarRoot[JsonImgKey]);

	SGImagePack* pPack = pPackManager->getPack(sgaName);

	pInfo->Type = UIElementType::ScrollBar;
	pInfo->Sga = pPack->getPackIndex();
	pInfo->Img = pPack->getImgIndex(imgName);
	SGJsonEx::parseSize(scrollBarRoot[JsonTrackSizeKey], pInfo->TrackSize);
	SGJsonEx::parseIntNumberN(scrollBarRoot[JsonSpriteKey], pInfo->Sprites, 7);
	return pInfo;
}

SGUIElementInfo* SGUIInfoLoader::readElementProgressBar(Value& progressBarRoot) {
	SGUIProgressBarInfo* pInfo = dbg_new SGUIProgressBarInfo();
	readElementCommon(progressBarRoot, pInfo);

	SGImagePackManager* pPackManager = SGImagePackManager::get();

	const SGString sgaName = SGJsonEx::getString(progressBarRoot[JsonSgaKey]);
	const SGString imgName = SGJsonEx::getString(progressBarRoot[JsonImgKey]);

	SGImagePack* pPack = pPackManager->getPack(sgaName);

	pInfo->Type = UIElementType::ProgressBar;
	pInfo->Sga = pPack->getPackIndex();
	pInfo->Img = pPack->getImgIndex(imgName);
	pInfo->Sprite = progressBarRoot[JsonSpriteKey].asInt();
	SGJsonEx::parseSize(progressBarRoot[JsonVisualSizeKey], pInfo->Size);
	pInfo->ProgressIncreaseDirection = (ProgressIncreaseDirection_t)progressBarRoot[JsonDirectionKey].asInt();
	return pInfo;
}

SGUIElementInfo* SGUIInfoLoader::readElementStatic(Value& staticRoot) {
	SGUIStaticInfo* pInfo = dbg_new SGUIStaticInfo();
	readElementCommon(staticRoot, pInfo);

	pInfo->Type = UIElementType::Static;
	SGJsonEx::parseSize(staticRoot[JsonVisualSizeKey], pInfo->Size);
	return pInfo;
}
