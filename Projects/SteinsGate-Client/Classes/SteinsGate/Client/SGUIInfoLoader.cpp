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

USING_NS_JC;
USING_NS_JS;

SGUIInfoLoader::SGUIInfoLoader(DataManagerAbstract* manager)
	: ConfigFileLoaderAbstract(manager)
{}

bool SGUIInfoLoader::load()
{
	Json::Value root;

	if (!loadJson(root))
		return false;
	try {

		// ========================================================================
		//  UI 엘리먼트 로딩
		// ========================================================================
		Value& uiElementListRoot = root["ui_element"];
		for (int i = 0; i < uiElementListRoot.size(); ++i) {
			Value& uiElementRoot = uiElementListRoot[i];

			SGUIElementInfo* pElemInfo = nullptr;
			int eElemType = uiElementRoot["type"].asInt();

			switch (eElemType) {
			case UIElementType::Button: pElemInfo = readElementButton(uiElementRoot); break;
			case UIElementType::Label: pElemInfo = readElementLabel(uiElementRoot); break;
			case UIElementType::Sprite: pElemInfo = readElementSprite(uiElementRoot); break;
			case UIElementType::EditBox: pElemInfo = readElementEditBox(uiElementRoot); break;
			case UIElementType::CheckBox: pElemInfo = readElementCheckBox(uiElementRoot); break;
			default: break;
			}

			DebugAssertMsg(pElemInfo != nullptr, "UI 엘리먼트 로딩에 실패했습니다.");
			addData(pElemInfo);
		}


		// ========================================================================
		//  UI 그룹 로딩
		// ========================================================================
		Value& uiGroupListRoot = root["ui_group"];
		for (int i = 0; i < uiGroupListRoot.size(); ++i) {
			Value& uiGroupRoot = uiGroupListRoot[i];
			Value& groupElemInfoListRoot = uiGroupRoot["elements"];
			SGUIGroupInfo* groupInfo = dbg_new SGUIGroupInfo(groupElemInfoListRoot.size());

			readElementCommon(uiGroupRoot, groupInfo);
			readElementGroup(uiGroupRoot, groupInfo);
			addData(groupInfo);
		}
	}
	catch (std::exception& ex) {
		_LogError_("%s 파싱중 오류가 발생하였습니다. %s", getConfigFileName(), ex.what());
		return false;
	}

	return true;
}



void SGUIInfoLoader::readElementCommon(Value& elementRoot, SGUIElementInfo* elementInfo) {
	elementInfo->Code = elementRoot["code"].asInt();
	elementInfo->Name = SGJsonEx::getString(elementRoot["name"]);
}

void SGUIInfoLoader::readElementGroup(Json::Value& uiGroupRoot, SGUIGroupInfo* groupInfo) {
	float fRect[4];
	SGJsonEx::parseFloatNumberN(uiGroupRoot["rect"], fRect, 4);
	groupInfo->Rect.origin.x = fRect[0];
	groupInfo->Rect.origin.y = fRect[1];
	groupInfo->Rect.size.width = fRect[2];
	groupInfo->Rect.size.height = fRect[3];
	groupInfo->HorizontalAlignment = (HorizontalAlignment_t)uiGroupRoot["horizontal_align"].asInt();
	groupInfo->Type = UIElementType::Group;
	groupInfo->VerticalAlignment = (VerticalAlignment_t)uiGroupRoot["vertical_align"].asInt();

	Value& groupElemInfoListRoot = uiGroupRoot["elements"];
	for (int j = 0; j < groupElemInfoListRoot.size(); ++j) {
		Value& groupElemInfoRoot = groupElemInfoListRoot[j];
		SGUIGroupElemInfo groupElemInfo;
		int groupElemInfoData[4];
		SGJsonEx::parseIntNumberN(groupElemInfoRoot, groupElemInfoData, 4);

		groupElemInfo.Code = groupElemInfoData[0];
		groupElemInfo.ZOrder = groupElemInfoData[1];
		groupElemInfo.X = (float)groupElemInfoData[2];
		groupElemInfo.Y = (float)groupElemInfoData[3];
		groupInfo->InfoList.PushBack(groupElemInfo);
	}

	groupInfo->InfoList.Sort([](SGUIGroupElemInfo& lhs, SGUIGroupElemInfo& rhs) {
		return lhs.ZOrder < rhs.ZOrder;
	});
}


SGUIElementInfo* SGUIInfoLoader::readElementButton(Value& buttonRoot) {
	SGUIButtonInfo* pInfo = dbg_new SGUIButtonInfo();

	readElementCommon(buttonRoot, pInfo);

	SGImagePackManager* pPackManager = SGImagePackManager::get();
	const SGString& sgaName = SGJsonEx::getString(buttonRoot["sga"]);
	const SGString& imgName = SGJsonEx::getString(buttonRoot["img"]);

	SGImagePack* pPack = pPackManager->getPack(sgaName);

	pInfo->Sga = pPack->getPackIndex();
	pInfo->Img = pPack->getImgIndex(imgName);

	pInfo->Type = UIElementType::Button;
	pInfo->Normal = buttonRoot["normal_idx"].asInt();
	pInfo->Over = buttonRoot["over_idx"].asInt();
	pInfo->Pressed = buttonRoot["pressed_idx"].asInt();
	pInfo->Disabled = buttonRoot["disabled_idx"].asInt();

	return pInfo;
}



SGUIElementInfo* SGUIInfoLoader::readElementLabel(Value& labelRoot) {
	SGUILabelInfo* pInfo = dbg_new SGUILabelInfo();
	SGDataManager* pDataManager = SGDataManager::get();

	readElementCommon(labelRoot, pInfo);

	SGString szFontName = SGJsonEx::getString(labelRoot["font"]);

	pInfo->Type = UIElementType::Label;
	pInfo->FontCode =  SGFontPackage::get()->getFontCode(szFontName);
	pInfo->FontSize = labelRoot["font_size"].asInt();
	pInfo->TextWrap = labelRoot["text_wrap"].asBool();
	pInfo->Width = labelRoot["width"].asFloat();
	pInfo->Height = labelRoot["height"].asFloat();
	pInfo->TextHAlignment = (HorizontalAlignment_t)labelRoot["horizontal_align"].asInt();
	pInfo->TextVAlignment = (VerticalAlignment_t)labelRoot["vertical_align"].asInt();
	pInfo->Text = SGJsonEx::getString(labelRoot["text"]);

	return pInfo;
}

SGUIElementInfo* SGUIInfoLoader::readElementSprite(Value& spriteRoot) {
	SGUISpriteInfo* pInfo = dbg_new SGUISpriteInfo();

	readElementCommon(spriteRoot, pInfo);

	SGImagePackManager* pPackManager = SGImagePackManager::get();

	const SGString& sgaName = SGJsonEx::getString(spriteRoot["sga"]);
	const SGString& imgName = SGJsonEx::getString(spriteRoot["img"]);

	SGImagePack* pPack = pPackManager->getPack(sgaName);

	pInfo->Type = UIElementType::Sprite;
	pInfo->Sga = pPack->getPackIndex();
	pInfo->Img = pPack->getImgIndex(imgName);
	pInfo->Sprite = spriteRoot["sprite"].asInt();

	return pInfo;
}

SGUIElementInfo* SGUIInfoLoader::readElementEditBox(Json::Value& editBoxRoot) {
	SGUIEditBoxInfo* pInfo = dbg_new SGUIEditBoxInfo();

	readElementCommon(editBoxRoot, pInfo);

	SGImagePackManager* pPackManager = SGImagePackManager::get();

	pInfo->Type = UIElementType::EditBox;
	pInfo->Size.width = editBoxRoot["width"].asFloat();
	pInfo->Size.height = editBoxRoot["height"].asFloat();
	pInfo->FontSize = editBoxRoot["font_size"].asInt();
	SGJsonEx::parseColor4B(editBoxRoot["font_color"], pInfo->FontColor);

	pInfo->TextHAlignment = HorizontalAlignment_t(editBoxRoot["text_horizontal_align"].asInt());

	pInfo->PlaceholderText = SGJsonEx::getString(editBoxRoot["placeholder_text"]);
	SGJsonEx::parseColor4B(editBoxRoot["placeholder_font_color"], pInfo->PlaceHolderFontColor);
	pInfo->PlaceholderFontSize = editBoxRoot["placeholder_font_size"].asFloat();
	pInfo->MaxLength = editBoxRoot["max_length"].asInt();
	pInfo->InputMode = SGInputMode(editBoxRoot["input_mode"].asInt());

	return pInfo;
}

SGUIElementInfo* SGUIInfoLoader::readElementCheckBox(Json::Value& checkBoxRoot) {
	SGUICheckBoxInfo* pInfo = dbg_new SGUICheckBoxInfo();

	readElementCommon(checkBoxRoot, pInfo);

	SGImagePackManager* pPackManager = SGImagePackManager::get();

	const SGString& sgaName = SGJsonEx::getString(checkBoxRoot["sga"]);
	const SGString& imgName = SGJsonEx::getString(checkBoxRoot["img"]);

	SGImagePack* pPack = pPackManager->getPack(sgaName);

	pInfo->Type = UIElementType::CheckBox;
	pInfo->Sga = pPack->getPackIndex();
	pInfo->Img = pPack->getImgIndex(imgName);
	pInfo->Background = checkBoxRoot["background_idx"].asInt();
	pInfo->BackgroundDisabled= checkBoxRoot["background_disabled_idx"].asInt();
	pInfo->Cross = checkBoxRoot["cross_idx"].asInt();
	pInfo->CrossDisabled = checkBoxRoot["cross_disabled_idx"].asInt();

	if (pInfo->Background < 0)
		pInfo->Background = InvalidValue_v;

	if (pInfo->BackgroundDisabled < 0)
		pInfo->BackgroundDisabled = InvalidValue_v;

	if (pInfo->CrossDisabled < 0)
		pInfo->CrossDisabled = InvalidValue_v;

	DebugAssertMsg(pInfo->Cross >= 0, "체크박스인데 크로스 이미지가 설정되어있지 않습니다.");

	return pInfo;
}
