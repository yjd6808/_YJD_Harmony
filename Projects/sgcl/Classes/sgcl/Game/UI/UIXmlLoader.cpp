#include "sgcl/Game/UI/UIXmlLoader.h"
#include "sgcl/Game/Texture/ImagePackManager.h"
#include "sgcl/Game/Contents/FontManager.h"
#include "sgcl/Game/UI/UICheckBox.h"

USING_NS_JC;

//////////////////////////////////////////////////////////////////////////////////////////
static int ParseSpriteIndices(const char* _str, int* _out, int _count)
{
	if (!_str || !_out || _count <= 0)
		return 0;

	int count = 0;
	const char* cursor = _str;

	while (*cursor && count < _count)
	{
		char* end = nullptr;
		const long value = std::strtol(cursor, &end, 10);
		if (cursor == end)
			break;

		_out[count++] = static_cast<int>(value);
		cursor = end;

		while (*cursor == ',' || *cursor == ' ' || *cursor == '\t')
			++cursor;
	}

	return count;
}

//////////////////////////////////////////////////////////////////////////////////////////
static void ParseColor(const char* _str, cc::Color4B& _color)
{
	if (!_str) return;
	int r, g, b, a;
	sscanf(_str, "%d,%d,%d,%d", &r, &g, &b, &a);
	_color = { (GLubyte)r, (GLubyte)g, (GLubyte)b, (GLubyte)a };
}

//////////////////////////////////////////////////////////////////////////////////////////
static int XmlIntAttr(tinyxml2::XMLElement* _elem, const char* _name, int _default)
{
	const char* val = _elem->Attribute(_name);
	if (!val) return _default;
	return atoi(val);
}

static float XmlFloatAttr(tinyxml2::XMLElement* _elem, const char* _name, float _default)
{
	const char* val = _elem->Attribute(_name);
	if (!val) return _default;
	return (float)atof(val);
}

//////////////////////////////////////////////////////////////////////////////////////////
static bool XmlBoolAttr(tinyxml2::XMLElement* _elem, const char* _name, bool _default)
{
	const char* val = _elem->Attribute(_name);
	if (!val) return _default;
	return strcmp(val, "true") == 0 || strcmp(val, "1") == 0;
}

//////////////////////////////////////////////////////////////////////////////////////////
static void ReadCommon(tinyxml2::XMLElement* _elem, UIElementInfo* _info)
{
	const char* name = _elem->Attribute("name");
	if (name)
		strcpy_s(_info->name_, name);
	_info->hAlignment_ = (HAlignment_t)XmlIntAttr(_elem, "halign", 0);
	_info->vAlignment_ = (VAlignment_t)XmlIntAttr(_elem, "valign", 0);

}

//////////////////////////////////////////////////////////////////////////////////////////
UIGroupInfo* UIXmlLoader::LoadFromFile(const char* _filePath)
{
	tinyxml2::XMLDocument doc;
	if (doc.LoadFile(_filePath) != tinyxml2::XML_SUCCESS)
		return nullptr;

	tinyxml2::XMLElement* root = doc.RootElement();
	if (!root)
		return nullptr;

	return ParseGroup(root);
}

//////////////////////////////////////////////////////////////////////////////////////////
UIGroupInfo* UIXmlLoader::LoadFromString(const char* _xmlContent)
{
	tinyxml2::XMLDocument doc;
	if (doc.Parse(_xmlContent) != tinyxml2::XML_SUCCESS)
		return nullptr;

	tinyxml2::XMLElement* root = doc.RootElement();
	if (!root)
		return nullptr;

	return ParseGroup(root);
}

//////////////////////////////////////////////////////////////////////////////////////////
static int ParseTypeFromTagName(const char* _tag)
{
	if (strcmp(_tag, "Group") == 0) return UIElementType::Group;
	if (strcmp(_tag, "Button") == 0) return UIElementType::Button;
	if (strcmp(_tag, "Label") == 0) return UIElementType::Label;
	if (strcmp(_tag, "Sprite") == 0) return UIElementType::Sprite;
	if (strcmp(_tag, "EditBox") == 0) return UIElementType::EditBox;
	if (strcmp(_tag, "CheckBox") == 0) return UIElementType::CheckBox;
	if (strcmp(_tag, "ToggleButton") == 0) return UIElementType::ToggleButton;
	if (strcmp(_tag, "ScrollBar") == 0) return UIElementType::ScrollBar;
	if (strcmp(_tag, "ProgressBar") == 0) return UIElementType::ProgressBar;
	if (strcmp(_tag, "Static") == 0) return UIElementType::Static;
	return -1;
}

//////////////////////////////////////////////////////////////////////////////////////////
UIElementInfo* UIXmlLoader::ParseElement(tinyxml2::XMLElement* _xmlElement)
{
	if (!_xmlElement) return nullptr;

	const char* typeAttr = _xmlElement->Attribute("type");
	int type;
	if (typeAttr)
	{
		type = atoi(typeAttr);
	}
	else
	{
		type = ParseTypeFromTagName(_xmlElement->Value());
		if (type < 0) return nullptr;
	}

	UIElementInfo* pResult = nullptr;
	switch (type)
	{
	case UIElementType::Button:      pResult = ParseButton(_xmlElement); break;
	case UIElementType::Label:       pResult = ParseLabel(_xmlElement); break;
	case UIElementType::Sprite:      pResult = ParseSprite(_xmlElement); break;
	case UIElementType::EditBox:     pResult = ParseEditBox(_xmlElement); break;
	case UIElementType::CheckBox:    pResult = ParseCheckBox(_xmlElement); break;
	case UIElementType::ToggleButton: pResult = ParseToggleButton(_xmlElement); break;
	case UIElementType::ProgressBar: pResult = ParseProgressBar(_xmlElement); break;
	case UIElementType::ScrollBar:   pResult = ParseScrollBar(_xmlElement); break;
	case UIElementType::Static:      pResult = ParseStatic(_xmlElement); break;
	case UIElementType::Group:       pResult = ParseGroup(_xmlElement); break;
	default: break;
	}

	if (pResult)
		pResult->type_ = (UIElementType_t)type;

	return pResult;
}

//////////////////////////////////////////////////////////////////////////////////////////
UIGroupInfo* UIXmlLoader::ParseGroup(tinyxml2::XMLElement* _xmlElement)
{
	if (!_xmlElement) return nullptr;

	tinyxml2::XMLElement* pDataElem = _xmlElement->FirstChildElement("data");

	int childCount = 0;
	{
		tinyxml2::XMLElement* pChild = _xmlElement->FirstChildElement();
		while (pChild)
		{
			const char* tag = pChild->Value();
			if (strcmp(tag, "data") != 0)
				childCount++;
			pChild = pChild->NextSiblingElement();
		}
	}

	UIGroupInfo* pGroupInfo = dbg_new UIGroupInfo(childCount);
	ReadCommon(_xmlElement, pGroupInfo);

	pGroupInfo->size_.width = XmlFloatAttr(_xmlElement, "width", 0);
	pGroupInfo->size_.height = XmlFloatAttr(_xmlElement, "height", 0);

	if (pDataElem)
	{
		jc::CDataMap<>* pDataMap = dbg_new jc::CDataMap<>();
		tinyxml2::XMLElement* pItem = pDataElem->FirstChildElement("item");
		while (pItem)
		{
			const char* key = pItem->Attribute("key");
			const char* value = pItem->Attribute("value");
			if (key && value)
				pDataMap->SetString(key, value);
			pItem = pItem->NextSiblingElement("item");
		}
		pGroupInfo->pDataMap_ = pDataMap;
	}

	{
		int index = 0;
		tinyxml2::XMLElement* pChild = _xmlElement->FirstChildElement();
		while (pChild && index < childCount)
		{
			const char* tag = pChild->Value();
			if (strcmp(tag, "data") == 0)
			{
				pChild = pChild->NextSiblingElement();
				continue;
			}

			UIGroupElemInfo& elemInfo = pGroupInfo->infoList_[index];
			const char* childName = pChild->Attribute("name");
			if (childName)
				strcpy_s(elemInfo.name_, childName);
			elemInfo.pos_.x = XmlFloatAttr(pChild, "x", 0);
			elemInfo.pos_.y = XmlFloatAttr(pChild, "y", 0);

			UIElementInfo* pElemInfo = ParseElement(pChild);
			if (pElemInfo)
				pGroupInfo->childInfoList_.PushBack(pElemInfo);

			index++;
			pChild = pChild->NextSiblingElement();
		}
	}

	return pGroupInfo;
}

//////////////////////////////////////////////////////////////////////////////////////////
UIElementInfo* UIXmlLoader::ParseButton(tinyxml2::XMLElement* _elem)
{
	UIButtonInfo* pInfo = dbg_new UIButtonInfo();
	ReadCommon(_elem, pInfo);

	ImagePackManager* pPackMgr = ImagePackManager::Get();
	const char* sga = _elem->Attribute("sga");
	const char* img = _elem->Attribute("img");

	if (sga && img)
	{
		ImagePack* pPack = pPackMgr->GetPack(sga);
		pInfo->sga_ = pPack->GetPackIndex();
		pInfo->img_ = pPack->GetImgIndex(img);
	}

	pInfo->linearDodge_ = XmlBoolAttr(_elem, "linear_dodge", false);
	ParseSpriteIndices(_elem->Attribute("sprite"), pInfo->sprites_, 4);

	return pInfo;
}

//////////////////////////////////////////////////////////////////////////////////////////
UIElementInfo* UIXmlLoader::ParseLabel(tinyxml2::XMLElement* _elem)
{
	UILabelInfo* pInfo = dbg_new UILabelInfo();
	ReadCommon(_elem, pInfo);

	const char* fontName = _elem->Attribute("font");
	if (fontName)
		pInfo->fontCode_ = FontManager::Get()->GetFontCode(fontName);

	pInfo->fontSize_ = XmlIntAttr(_elem, "font_size", 16);
	ParseColor(_elem->Attribute("font_color"), pInfo->fontColor_);
	pInfo->textWrap_ = XmlBoolAttr(_elem, "text_wrap", true);
	pInfo->textHAlignment_ = (HAlignment_t)XmlIntAttr(_elem, "text_halign", 0);
	pInfo->textVAlignment_ = (VAlignment_t)XmlIntAttr(_elem, "text_valign", 0);
	const char* text = _elem->Attribute("text");
	if (text) pInfo->text_ = text;
	pInfo->size_.width = XmlFloatAttr(_elem, "width", 0);
	pInfo->size_.height = XmlFloatAttr(_elem, "height", 0);

	return pInfo;
}

//////////////////////////////////////////////////////////////////////////////////////////
UIElementInfo* UIXmlLoader::ParseSprite(tinyxml2::XMLElement* _elem)
{
	UISpriteInfo* pInfo = dbg_new UISpriteInfo();
	ReadCommon(_elem, pInfo);

	ImagePackManager* pPackMgr = ImagePackManager::Get();
	const char* sga = _elem->Attribute("sga");
	const char* img = _elem->Attribute("img");

	if (sga && img)
	{
		ImagePack* pPack = pPackMgr->GetPack(sga);
		pInfo->sga_ = pPack->GetPackIndex();
		pInfo->img_ = pPack->GetImgIndex(img);
	}

	pInfo->sprite_ = XmlIntAttr(_elem, "sprite", InvalidValue_v);
	pInfo->linearDodge_ = XmlBoolAttr(_elem, "linear_dodge", false);
	pInfo->scale9_ = XmlBoolAttr(_elem, "scale9", false);
	pInfo->size_.width = XmlFloatAttr(_elem, "width", 0);
	pInfo->size_.height = XmlFloatAttr(_elem, "height", 0);

	return pInfo;
}

//////////////////////////////////////////////////////////////////////////////////////////
UIElementInfo* UIXmlLoader::ParseEditBox(tinyxml2::XMLElement* _elem)
{
	UIEditBoxInfo* pInfo = dbg_new UIEditBoxInfo();
	ReadCommon(_elem, pInfo);

	pInfo->Size.width = XmlFloatAttr(_elem, "width", 0);
	pInfo->Size.height = XmlFloatAttr(_elem, "height", 0);
	pInfo->FontSize = XmlIntAttr(_elem, "font_size", 16);
	pInfo->TextHAlignment = (HAlignment_t)XmlIntAttr(_elem, "text_halign", 0);
	ParseColor(_elem->Attribute("font_color"), pInfo->FontColor);

	const char* pText = _elem->Attribute("p_text");
	if (pText) pInfo->PlaceholderText = pText;
	ParseColor(_elem->Attribute("p_font_color"), pInfo->PlaceHolderFontColor);
	pInfo->PlaceholderFontSize = XmlIntAttr(_elem, "p_font_size", 16);
	pInfo->MaxLength = XmlIntAttr(_elem, "max_length", 50);
	pInfo->InputMode = (EditBoxInputMode)XmlIntAttr(_elem, "input_mode", 0);

	return pInfo;
}

//////////////////////////////////////////////////////////////////////////////////////////
UIElementInfo* UIXmlLoader::ParseCheckBox(tinyxml2::XMLElement* _elem)
{
	UICheckBoxInfo* pInfo = dbg_new UICheckBoxInfo();
	ReadCommon(_elem, pInfo);

	ImagePackManager* pPackMgr = ImagePackManager::Get();
	const char* bgSga = _elem->Attribute("bg_sga");
	const char* bgImg = _elem->Attribute("bg_img");
	if (bgSga && bgImg)
	{
		ImagePack* pPack = pPackMgr->GetPackUnsafe(bgSga);
		if (pPack) { pInfo->BackgroundSga = pPack->GetPackIndex(); pInfo->BackgroundImg = pPack->GetImgIndex(bgImg); }
	}
	const char* crossSga = _elem->Attribute("cross_sga");
	const char* crossImg = _elem->Attribute("cross_img");
	if (crossSga && crossImg)
	{
		ImagePack* pPack = pPackMgr->GetPack(crossSga);
		pInfo->CrossSga = pPack->GetPackIndex(); pInfo->CrossImg = pPack->GetImgIndex(crossImg);
	}
	pInfo->Check = XmlBoolAttr(_elem, "check", false);
	ParseSpriteIndices(_elem->Attribute("sprite"), pInfo->Sprites, 4);

	return pInfo;
}

//////////////////////////////////////////////////////////////////////////////////////////
UIElementInfo* UIXmlLoader::ParseToggleButton(tinyxml2::XMLElement* _elem)
{
	UIToggleButtonInfo* pInfo = dbg_new UIToggleButtonInfo();
	ReadCommon(_elem, pInfo);

	ImagePackManager* pPackMgr = ImagePackManager::Get();
	const char* sga = _elem->Attribute("sga");
	const char* img = _elem->Attribute("img");
	if (sga && img)
	{
		ImagePack* pPack = pPackMgr->GetPack(sga);
		pInfo->Sga = pPack->GetPackIndex(); pInfo->Img = pPack->GetImgIndex(img);
	}
	pInfo->LinearDodge = XmlBoolAttr(_elem, "linear_dodge", false);
	ParseSpriteIndices(_elem->Attribute("sprite"), pInfo->Sprites[0], 4);
	ParseSpriteIndices(_elem->Attribute("sprite2"), pInfo->Sprites[1], 4);

	return pInfo;
}

//////////////////////////////////////////////////////////////////////////////////////////
UIElementInfo* UIXmlLoader::ParseProgressBar(tinyxml2::XMLElement* _elem)
{
	UIProgressBarInfo* pInfo = dbg_new UIProgressBarInfo();
	ReadCommon(_elem, pInfo);

	ImagePackManager* pPackMgr = ImagePackManager::Get();
	const char* sga = _elem->Attribute("sga");
	const char* img = _elem->Attribute("img");
	if (sga && img)
	{
		ImagePack* pPack = pPackMgr->GetPack(sga);
		pInfo->Sga = pPack->GetPackIndex(); pInfo->Img = pPack->GetImgIndex(img);
	}
	pInfo->Sprite = XmlIntAttr(_elem, "sprite", InvalidValue_v);
	pInfo->Size.width = XmlFloatAttr(_elem, "width", 0);
	pInfo->Size.height = XmlFloatAttr(_elem, "height", 0);
	pInfo->ProgressIncreaseDirection = (ProgressIncreaseDirection_t)XmlIntAttr(_elem, "direction", 0);

	return pInfo;
}

//////////////////////////////////////////////////////////////////////////////////////////
UIElementInfo* UIXmlLoader::ParseScrollBar(tinyxml2::XMLElement* _elem)
{
	UIScrollBarInfo* pInfo = dbg_new UIScrollBarInfo();
	ReadCommon(_elem, pInfo);

	ImagePackManager* pPackMgr = ImagePackManager::Get();
	const char* sga = _elem->Attribute("sga");
	const char* img = _elem->Attribute("img");
	if (sga && img)
	{
		ImagePack* pPack = pPackMgr->GetPack(sga);
		pInfo->Sga = pPack->GetPackIndex(); pInfo->Img = pPack->GetImgIndex(img);
	}
	pInfo->TrackSize.width = XmlFloatAttr(_elem, "track_width", 0);
	pInfo->TrackSize.height = XmlFloatAttr(_elem, "track_height", 0);
	ParseSpriteIndices(_elem->Attribute("sprite"), pInfo->Sprites, 7);

	return pInfo;
}

//////////////////////////////////////////////////////////////////////////////////////////
UIElementInfo* UIXmlLoader::ParseStatic(tinyxml2::XMLElement* _elem)
{
	UIStaticInfo* pInfo = dbg_new UIStaticInfo();
	ReadCommon(_elem, pInfo);
	pInfo->Size.width = XmlFloatAttr(_elem, "width", 0);
	pInfo->Size.height = XmlFloatAttr(_elem, "height", 0);
	return pInfo;
}