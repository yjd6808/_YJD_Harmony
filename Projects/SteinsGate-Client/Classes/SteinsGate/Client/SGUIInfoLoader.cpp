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
	pInfo->HorizontalAlignment = (HorizontalAlignment_t)labelRoot["horizontal_align"].asInt();
	pInfo->VerticalAlignment = (VerticalAlignment_t)labelRoot["vertical_align"].asInt();
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
