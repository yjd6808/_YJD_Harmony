/*
 * 작성자: 윤정도
 * 생성일: 2/15/2023 1:30:20 AM
 * =====================
 *
 */



#include "Tutturu.h"
#include "SGUIInfoLoader.h"

#include <SteinsGate/Client/SGImagePackManager.h>
#include <SteinsGate/Client/SGJson.h>
#include <SteinsGate/Client/SGDataManager.h>

#include <JCore/FileSystem/Path.h>

#include <json.h>
#include <fstream>




USING_NS_JC;
USING_NS_JS;

#define JsonFileName "ui.json"

bool SGUIInfoLoader::LoadUIInfo(SGHashMap<int, SGUIElementInfo*>& elementInfoMap)
{
	SGString path = JCore::Path::Combine(ConfigDirectoryPath_v, JsonFileName);
	std::ifstream reader(path.Source(), std::ifstream::in | std::ifstream::binary);
	DebugAssertMsg(reader.is_open(), "monster.json 파일을 여는데 실패했습니다.");
	Value root;
	try {
		reader >> root;

		// ========================================================================
		//  UI 엘리먼트 로딩
		// ========================================================================
		Value& uiElementListRoot = root["ui_element"];
		for (int i = 0; i < uiElementListRoot.size(); ++i) {
			Value& uiElementRoot = uiElementListRoot[i];

			SGUIElementInfo* pElemInfo = nullptr;
			int eElemType = uiElementRoot["type"].asInt();

			switch (eElemType) {
			case UIElementType::Button: pElemInfo = LoadElementButton(uiElementRoot); break;
			case UIElementType::Label: pElemInfo = LoadElementLabel(uiElementRoot); break;
			case UIElementType::Sprite: pElemInfo = LoadElementSprite(uiElementRoot); break;
			default: break;
			}

			DebugAssertMsg(pElemInfo != nullptr, "UI 엘리먼트 로딩에 실패했습니다.");
			elementInfoMap.Insert(pElemInfo->Code, pElemInfo);
		}


		// ========================================================================
		//  UI 그룹 로딩
		// ========================================================================
		Value& uiGroupListRoot = root["ui_group"];
		for (int i = 0; i < uiGroupListRoot.size(); ++i) {
			Value& uiGroupRoot = uiGroupListRoot[i];
			Value& groupElemInfoListRoot = uiGroupRoot["elements"];
			SGUIGroupInfo* groupInfo = dbg_new SGUIGroupInfo(groupElemInfoListRoot.size());

			LoadElementCommon(uiGroupRoot, groupInfo);
			float fRect[4];
			SGJson::parseFloatNumberN(uiGroupRoot["rect"], fRect, 4);
			groupInfo->Rect.origin.x = fRect[0];
			groupInfo->Rect.origin.y = fRect[1];
			groupInfo->Rect.size.width = fRect[2];
			groupInfo->Rect.size.height = fRect[3];
			groupInfo->HorizontalAlignment = (HorizontalAlignment_t)uiGroupRoot["horizontal_align"].asInt();
			groupInfo->Type = UIElementType::Group;
			groupInfo->VerticalAlignment = (VerticalAlignment_t)uiGroupRoot["vertical_align"].asInt();

			for (int j = 0; j < groupElemInfoListRoot.size(); ++j) {
				Value& groupElemInfoRoot = groupElemInfoListRoot[j];
				SGUIGroupElemInfo groupElemInfo;
				int groupElemInfoData[4];
				SGJson::parseIntNumberN(groupElemInfoRoot, groupElemInfoData, 4);

				groupElemInfo.Code = groupElemInfoData[0];
				groupElemInfo.ZOrder = groupElemInfoData[1];
				groupElemInfo.X = groupElemInfoData[2];
				groupElemInfo.Y = groupElemInfoData[3];
				groupInfo->InfoList.PushBack(groupElemInfo);
			}

			groupInfo->InfoList.Sort([](SGUIGroupElemInfo& lhs, SGUIGroupElemInfo& rhs) {
				return lhs.ZOrder < rhs.ZOrder;
			});

			elementInfoMap.Insert(groupInfo->Code, groupInfo);
		}
	}
	catch (std::exception& ex) {
		Log(SGStringUtil::Format("%s 파싱중 오류가 발생하였습니다. %s\n", JsonFileName, ex.what()).Source());
		return false;
	}

	
	Log("SGUIInfoLoader :: 로딩완료\n");
	return true;
}



void SGUIInfoLoader::LoadElementCommon(Value& value, SGUIElementInfo* info) {
	info->Code = value["code"].asInt();
	info->Name = SGJson::getString(value["name"]);
}


SGUIElementInfo* SGUIInfoLoader::LoadElementButton(Value& value) {
	SGUIButtonInfo* pInfo = dbg_new SGUIButtonInfo();

	LoadElementCommon(value, pInfo);

	SGImagePackManager* pPackManager = SGImagePackManager::get();
	const SGString& npkName = SGJson::getString(value["npk"]);
	const SGString& imgName = SGJson::getString(value["img"]);

	SGImagePack* pPack = pPackManager->getPack(npkName);

	pInfo->Npk = pPack->getPackIndex();
	pInfo->Img = pPack->getImgIndex(imgName);

	pInfo->Type = UIElementType::Button;
	pInfo->Normal = value["normal_idx"].asInt();
	pInfo->Over = value["over_idx"].asInt();
	pInfo->Pressed = value["pressed_idx"].asInt();
	pInfo->Disabled = value["disabled_idx"].asInt();

	return pInfo;
}



SGUIElementInfo* SGUIInfoLoader::LoadElementLabel(Value& value) {
	SGUILabelInfo* pInfo = dbg_new SGUILabelInfo();
	SGDataManager* pDataManager = SGDataManager::get();

	LoadElementCommon(value, pInfo);

	SGString szFontName = SGJson::getString(value["font"]);

	pInfo->Type = UIElementType::Label;
	pInfo->FontCode = pDataManager->getFontCode(szFontName);
	pInfo->FontSize = value["font_size"].asInt();
	pInfo->TextWrap = value["text_wrap"].asBool();
	pInfo->Width = value["width"].asFloat();
	pInfo->Height = value["height"].asFloat();
	pInfo->HorizontalAlignment = (HorizontalAlignment_t)value["horizontal_align"].asInt();
	pInfo->VerticalAlignment = (VerticalAlignment_t)value["vertical_align"].asInt();
	pInfo->Text = SGJson::getString(value["text"]);

	return pInfo;
}

SGUIElementInfo* SGUIInfoLoader::LoadElementSprite(Value& value) {
	SGUISpriteInfo* pInfo = dbg_new SGUISpriteInfo();

	LoadElementCommon(value, pInfo);

	SGImagePackManager* pPackManager = SGImagePackManager::get();

	const SGString& npkName = SGJson::getString(value["npk"]);
	const SGString& imgName = SGJson::getString(value["img"]);

	SGImagePack* pPack = pPackManager->getPack(npkName);

	pInfo->Type = UIElementType::Sprite;
	pInfo->Npk = pPack->getPackIndex();
	pInfo->Img = pPack->getImgIndex(imgName);
	pInfo->Sprite = value["sprite"].asInt();

	return pInfo;
}
