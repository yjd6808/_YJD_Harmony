/*
 * 작성자: 윤정도
 * 생성일: 2/15/2023 1:30:20 AM
 * =====================
 *
 */

#include "Core.h"
#include "DescMgr_UI.h"

#include <sgcl/ImagePackManager.h>
#include <sgcl/JsonUtilEx.h>
#include <sgcl/DataManager.h>
#include <sgcl/FontManager.h>

#include "UICheckBox.h"

USING_NS_JC;
USING_NS_JS;

//////////////////////////////////////////////////////////////////////////////////////////
UIInfoLoader::UIInfoLoader(DescLoaderMgr* _pManager)
: DescLoaderAbstract(_pManager)
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
		Json::Value& elementListRoot = root[JSON_ELEMENTS_KEY];

		for (int i = 0; i < elementListRoot.size(); ++i)
		{
			Json::Value& elementRoot = elementListRoot[i];

			UIElementInfo* pElementInfo = nullptr;
			int elementType = elementRoot[JSON_ELEMENT_TYPE_KEY].asInt();

			switch (elementType)
			{
			case UIElementType::Button: pElementInfo = ReadElementButton(elementRoot);
				break;
			case UIElementType::Label: pElementInfo = ReadElementLabel(elementRoot);
				break;
			case UIElementType::Sprite: pElementInfo = ReadElementSprite(elementRoot);
				break;
			case UIElementType::EditBox: pElementInfo = ReadElementEditBox(elementRoot);
				break;
			case UIElementType::CheckBox: pElementInfo = ReadElementCheckBox(elementRoot);
				break;
			case UIElementType::ToggleButton: pElementInfo = ReadElementToggleButton(elementRoot);
				break;
			case UIElementType::ScrollBar: pElementInfo = ReadElementScrollBar(elementRoot);
				break;
			case UIElementType::ProgressBar: pElementInfo = ReadElementProgressBar(elementRoot);
				break;
			case UIElementType::Static: pElementInfo = ReadElementStatic(elementRoot);
				break;
			default:
				break;
			}

			jc_assert_msg(pElementInfo != nullptr, "UI 엘리먼트 로딩에 실패했습니다.");
			AddData(pElementInfo);
		}

		// ========================================================================
		//  UI 그룹 로딩
		// ========================================================================
		Json::Value& groupListRoot = root[JSON_GROUPS_KEY];

		for (int i = 0; i < groupListRoot.size(); ++i)
		{
			Json::Value& groupRoot = groupListRoot[i];
			Json::Value& groupElementInfoListRoot = groupRoot[JSON_CHILDREN_KEY];

			// 키값이 "groups" = []으로 들어가버린경우
			_LogWarnIf_(groupElementInfoListRoot.empty(), "그룹에 자식이 없습니다.");

			UIGroupInfo* pGroupInfo = dbg_new UIGroupInfo(groupElementInfoListRoot.size());

			ReadElementCommon(groupRoot, pGroupInfo);
			ReadElementGroup(groupRoot, pGroupInfo);
			AddData(pGroupInfo);
		}

		// ========================================================================
		//  UI 그룹마스터 로딩
		// ========================================================================
		Json::Value& groupMasterRoot = root[JSON_GROUP_MASTER_KEY];
		Json::Value& groupMasterElementInfoListRoot = groupMasterRoot[JSON_CHILDREN_KEY];

		UIGroupInfo* pGroupMasterInfo = dbg_new UIGroupInfo(groupMasterElementInfoListRoot.size());

		ReadElementCommon(groupMasterRoot, pGroupMasterInfo);
		ReadElementGroup(groupMasterRoot, pGroupMasterInfo);
		AddData(pGroupMasterInfo);
	}
	catch (std::exception& ex)
	{
		_LogError_("%s 파싱중 오류가 발생하였습니다. %s", GetConfigFileName(), ex.what());
		return false;
	}

	Loaded();
	return true;
}

//////////////////////////////////////////////////////////////////////////////////////////
void UIInfoLoader::ReadElementCommon(Json::Value& _elementRoot, UIElementInfo* _pElementInfo)
{
	_pElementInfo->code_ = _elementRoot[JSON_CODE_KEY].asInt();
	_pElementInfo->hAlignment_ = (HAlignment_t)_elementRoot[JSON_H_ALIGN_KEY].asInt();
	_pElementInfo->vAlignment_ = (VAlignment_t)_elementRoot[JSON_V_ALIGN_KEY].asInt();
}

//////////////////////////////////////////////////////////////////////////////////////////
void UIInfoLoader::ReadElementGroup(Json::Value& _groupRoot, UIGroupInfo* _pGroupInfo)
{
	JsonUtilEx::ParseFloatNumber2(_groupRoot[JSON_VISUAL_SIZE_KEY], _pGroupInfo->size_.width, _pGroupInfo->size_.height);
	_pGroupInfo->type_ = UIElementType::Group;

	Json::Value& groupElementInfoListRoot = _groupRoot[JSON_CHILDREN_KEY];

	for (int i = 0; i < groupElementInfoListRoot.size(); ++i)
	{
		Json::Value& groupElementInfoRoot = groupElementInfoListRoot[i];
		UIGroupElemInfo groupElementInfo;
		int groupElementInfoData[3];

		JsonUtilEx::ParseIntNumberN(groupElementInfoRoot, groupElementInfoData, 3);

		groupElementInfo.code_ = groupElementInfoData[0];
		groupElementInfo.pos_.x = (float)groupElementInfoData[1];
		groupElementInfo.pos_.y = (float)groupElementInfoData[2];

		_pGroupInfo->infoList_.PushBack(groupElementInfo);
	}
}

//////////////////////////////////////////////////////////////////////////////////////////
UIElementInfo* UIInfoLoader::ReadElementButton(Json::Value& _buttonRoot)
{
	UIButtonInfo* pInfo = dbg_new UIButtonInfo();

	ReadElementCommon(_buttonRoot, pInfo);

	ImagePackManager* pPackManager = ImagePackManager::Get();
	const jc::String& sgaName = JsonUtilEx::GetString(_buttonRoot[JSON_SGA_KEY]);
	const jc::String& imageName = JsonUtilEx::GetString(_buttonRoot[JSON_IMG_KEY]);

	ImagePack* pPack = pPackManager->GetPack(sgaName);

	pInfo->sga_ = pPack->GetPackIndex();
	pInfo->img_ = pPack->GetImgIndex(imageName);

	pInfo->type_ = UIElementType::Button;
	pInfo->linearDodge_ = _buttonRoot[JSON_LINEAR_DODGE_KEY].asBool();
	JsonUtilEx::ParseIntNumberN(_buttonRoot[JSON_SPRITE_KEY], pInfo->sprites_, 4);

	return pInfo;
}

//////////////////////////////////////////////////////////////////////////////////////////
UIElementInfo* UIInfoLoader::ReadElementLabel(Json::Value& _labelRoot)
{
	UILabelInfo* pInfo = dbg_new UILabelInfo();
	DataManager* pDataManager = DataManager::Get();
	ReadElementCommon(_labelRoot, pInfo);

	const jc::String fontName = JsonUtilEx::GetString(_labelRoot[JSON_FONT_KEY]);

	pInfo->type_ = UIElementType::Label;
	pInfo->fontCode_ = FontManager::Get()->GetFontCode(fontName);
	pInfo->fontSize_ = _labelRoot[JSON_FONT_SIZE_KEY].asInt();
	pInfo->textWrap_ = _labelRoot[JSON_TEXT_WRAP_KEY].asBool();
	JsonUtilEx::ParseColor4B(_labelRoot[JSON_FONT_COLOR_KEY], pInfo->fontColor_);
	JsonUtilEx::ParseSize(_labelRoot[JSON_VISUAL_SIZE_KEY], pInfo->size_);
	pInfo->textHAlignment_ = (HAlignment_t)_labelRoot[JSON_TEXT_H_ALIGN_KEY].asInt();
	pInfo->textVAlignment_ = (VAlignment_t)_labelRoot[JSON_TEXT_V_ALIGN_KEY].asInt();
	pInfo->text_ = JsonUtilEx::GetString(_labelRoot[JSON_TEXT_KEY]);

	return pInfo;
}

//////////////////////////////////////////////////////////////////////////////////////////
UIElementInfo* UIInfoLoader::ReadElementSprite(Json::Value& _spriteRoot)
{
	UISpriteInfo* pInfo = dbg_new UISpriteInfo();
	ReadElementCommon(_spriteRoot, pInfo);

	ImagePackManager* pPackManager = ImagePackManager::Get();

	const jc::String& sgaName = JsonUtilEx::GetString(_spriteRoot[JSON_SGA_KEY]);
	const jc::String& imageName = JsonUtilEx::GetString(_spriteRoot[JSON_IMG_KEY]);

	ImagePack* pPack = pPackManager->GetPack(sgaName);

	pInfo->type_ = UIElementType::Sprite;
	pInfo->sga_ = pPack->GetPackIndex();
	pInfo->img_ = pPack->GetImgIndex(imageName);
	pInfo->sprite_ = _spriteRoot[JSON_SPRITE_KEY].asInt();
	pInfo->linearDodge_ = _spriteRoot.get(JSON_LINEAR_DODGE_KEY, false).asBool();
	pInfo->scale9_ = _spriteRoot.get(JSON_SCALE9, false).asBool();
	JsonUtilEx::ParseSize(_spriteRoot[JSON_VISUAL_SIZE_KEY], pInfo->size_);

	return pInfo;
}

//////////////////////////////////////////////////////////////////////////////////////////
UIElementInfo* UIInfoLoader::ReadElementEditBox(Json::Value& _editBoxRoot)
{
	UIEditBoxInfo* pInfo = dbg_new UIEditBoxInfo();

	ReadElementCommon(_editBoxRoot, pInfo);

	ImagePackManager* pPackManager = ImagePackManager::Get();
	(void)pPackManager;

	pInfo->type_ = UIElementType::EditBox;
	JsonUtilEx::ParseSize(_editBoxRoot[JSON_VISUAL_SIZE_KEY], pInfo->Size);
	pInfo->FontSize = _editBoxRoot[JSON_FONT_SIZE_KEY].asInt();
	pInfo->TextHAlignment = (HAlignment_t)_editBoxRoot[JSON_TEXT_H_ALIGN_KEY].asInt();
	JsonUtilEx::ParseColor4B(_editBoxRoot[JSON_FONT_COLOR_KEY], pInfo->FontColor);

	pInfo->PlaceholderText = JsonUtilEx::GetString(_editBoxRoot[JSON_PLACEHOLDER_TEXT_KEY]);
	JsonUtilEx::ParseColor4B(_editBoxRoot[JSON_PLACEHOLDER_FONT_COLOR_KEY], pInfo->PlaceHolderFontColor);
	pInfo->PlaceholderFontSize = _editBoxRoot[JSON_PLACEHOLDER_FONT_SIZE_KEY].asInt();
	pInfo->MaxLength = _editBoxRoot[JSON_MAX_LENGTH_KEY].asInt();
	pInfo->InputMode = c2d_ui::EditBox::InputMode(_editBoxRoot[JSON_INPUT_MODE_KEY].asInt());

	return pInfo;
}

//////////////////////////////////////////////////////////////////////////////////////////
UIElementInfo* UIInfoLoader::ReadElementCheckBox(Json::Value& _checkBoxRoot)
{
	UICheckBoxInfo* pInfo = dbg_new UICheckBoxInfo();

	ReadElementCommon(_checkBoxRoot, pInfo);

	ImagePackManager* pPackManager = ImagePackManager::Get();

	const jc::String backgroundSgaName = JsonUtilEx::GetString(_checkBoxRoot[JSON_BACKGROUND_SGA]);
	const jc::String backgroundImageName = JsonUtilEx::GetString(_checkBoxRoot[JSON_BACKGROUND_IMG]);

	const jc::String crossSgaName = JsonUtilEx::GetString(_checkBoxRoot[JSON_CROSS_SGA]);
	const jc::String crossImageName = JsonUtilEx::GetString(_checkBoxRoot[JSON_CROSS_IMG]);

	ImagePack* pBackgroundPack = pPackManager->GetPackUnsafe(backgroundSgaName); // 백그라운드는 sga이름이 빈 문자열 일 수 있음
	ImagePack* pCrossPack = pPackManager->GetPack(crossSgaName);

	pInfo->type_ = UIElementType::CheckBox;
	pInfo->Check = _checkBoxRoot.get(JSON_CHECK, false).asBool();

	if (pBackgroundPack != nullptr)
	{
		pInfo->BackgroundSga = pBackgroundPack->GetPackIndex();
		pInfo->BackgroundImg = pBackgroundPack->GetImgIndex(backgroundImageName);
	}

	pInfo->CrossSga = pCrossPack->GetPackIndex();
	pInfo->CrossImg = pCrossPack->GetImgIndex(crossImageName);
	JsonUtilEx::ParseIntNumberN(_checkBoxRoot[JSON_SPRITE_KEY], pInfo->Sprites, 4);

	jc_assert_msg(pInfo->Sprites[UICheckBox::INDEX_CROSS] != InvalidValue_v, "체크박스인데 크로스 이미지가 설정되어있지 않습니다.");

	return pInfo;
}

//////////////////////////////////////////////////////////////////////////////////////////
UIElementInfo* UIInfoLoader::ReadElementToggleButton(Json::Value& _toggleButtonRoot)
{
	UIToggleButtonInfo* pInfo = dbg_new UIToggleButtonInfo();

	ReadElementCommon(_toggleButtonRoot, pInfo);

	ImagePackManager* pPackManager = ImagePackManager::Get();

	const jc::String sgaName = JsonUtilEx::GetString(_toggleButtonRoot[JSON_SGA_KEY]);
	const jc::String imageName = JsonUtilEx::GetString(_toggleButtonRoot[JSON_IMG_KEY]);

	ImagePack* pPack = pPackManager->GetPack(sgaName);

	pInfo->type_ = UIElementType::ToggleButton;
	pInfo->LinearDodge = _toggleButtonRoot[JSON_LINEAR_DODGE_KEY].asBool();
	pInfo->Sga = pPack->GetPackIndex();
	pInfo->Img = pPack->GetImgIndex(imageName);

	JsonUtilEx::ParseIntNumberN(_toggleButtonRoot[JSON_SPRITE_KEY], pInfo->Sprites[0], 4);
	JsonUtilEx::ParseIntNumberN(_toggleButtonRoot[JSON_TOGGLE_SPRITE_KEY], pInfo->Sprites[1], 4);

	return pInfo;
}

//////////////////////////////////////////////////////////////////////////////////////////
UIElementInfo* UIInfoLoader::ReadElementScrollBar(Json::Value& _scrollBarRoot)
{
	UIScrollBarInfo* pInfo = dbg_new UIScrollBarInfo();

	ReadElementCommon(_scrollBarRoot, pInfo);

	ImagePackManager* pPackManager = ImagePackManager::Get();

	const jc::String sgaName = JsonUtilEx::GetString(_scrollBarRoot[JSON_SGA_KEY]);
	const jc::String imageName = JsonUtilEx::GetString(_scrollBarRoot[JSON_IMG_KEY]);

	ImagePack* pPack = pPackManager->GetPack(sgaName);

	pInfo->type_ = UIElementType::ScrollBar;
	pInfo->Sga = pPack->GetPackIndex();
	pInfo->Img = pPack->GetImgIndex(imageName);
	JsonUtilEx::ParseSize(_scrollBarRoot[JSON_TRACK_SIZE_KEY], pInfo->TrackSize);
	JsonUtilEx::ParseIntNumberN(_scrollBarRoot[JSON_SPRITE_KEY], pInfo->Sprites, 7);

	return pInfo;
}

//////////////////////////////////////////////////////////////////////////////////////////
UIElementInfo* UIInfoLoader::ReadElementProgressBar(Json::Value& _progressBarRoot)
{
	UIProgressBarInfo* pInfo = dbg_new UIProgressBarInfo();

	ReadElementCommon(_progressBarRoot, pInfo);

	ImagePackManager* pPackManager = ImagePackManager::Get();

	const jc::String sgaName = JsonUtilEx::GetString(_progressBarRoot[JSON_SGA_KEY]);
	const jc::String imageName = JsonUtilEx::GetString(_progressBarRoot[JSON_IMG_KEY]);

	ImagePack* pPack = pPackManager->GetPack(sgaName);

	pInfo->type_ = UIElementType::ProgressBar;
	pInfo->Sga = pPack->GetPackIndex();
	pInfo->Img = pPack->GetImgIndex(imageName);
	pInfo->Sprite = _progressBarRoot[JSON_SPRITE_KEY].asInt();
	JsonUtilEx::ParseSize(_progressBarRoot[JSON_VISUAL_SIZE_KEY], pInfo->Size);
	pInfo->ProgressIncreaseDirection = (ProgressIncreaseDirection_t)_progressBarRoot[JSON_DIRECTION_KEY].asInt();

	return pInfo;
}

//////////////////////////////////////////////////////////////////////////////////////////
UIElementInfo* UIInfoLoader::ReadElementStatic(Json::Value& _staticRoot)
{
	UIStaticInfo* pInfo = dbg_new UIStaticInfo();

	ReadElementCommon(_staticRoot, pInfo);

	pInfo->type_ = UIElementType::Static;
	JsonUtilEx::ParseSize(_staticRoot[JSON_VISUAL_SIZE_KEY], pInfo->Size);

	return pInfo;
}
