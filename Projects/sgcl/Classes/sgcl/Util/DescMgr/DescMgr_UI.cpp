#include "DescMgr_UI.h"

#include "sgcl/Game/Texture/ImagePackManager.h"
#include "sgcl/Game/Contents/FontManager.h"
#include "sgcl/Game/UI/UICheckBox.h"
#include "tinyxml2.h"

USING_NS_JC;
USING_NS_JS;

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
static bool XmlBoolAttr(tinyxml2::XMLElement* _elem, const char* _name, bool _default)
{
    const char* val = _elem->Attribute(_name);
    if (!val) return _default;
    return strcmp(val, "true") == 0 || strcmp(val, "1") == 0;
}

UIInfoLoader::UIInfoLoader()
{
}

bool UIInfoLoader::Load()
{
	tinyxml2::XMLDocument doc;
	if (doc.LoadFile(GetConfigFileName()) != tinyxml2::XML_SUCCESS)
		return false;

	tinyxml2::XMLElement* rootElem = doc.RootElement();
	if (!rootElem || strcmp(rootElem->Name(), XML_ROOT_TAG) != 0)
		return false;

	tinyxml2::XMLElement* pChild = rootElem->FirstChildElement();
	while (pChild)
	{
		UIElementInfo* pElementInfo = nullptr;
		int elementType = XmlIntAttr(pChild, "type", -1);

		switch (elementType)
		{
		case UIElementType::Button: pElementInfo = ReadElementButton(pChild); break;
		case UIElementType::Label: pElementInfo = ReadElementLabel(pChild); break;
		case UIElementType::Sprite: pElementInfo = ReadElementSprite(pChild); break;
		case UIElementType::EditBox: pElementInfo = ReadElementEditBox(pChild); break;
		case UIElementType::CheckBox: pElementInfo = ReadElementCheckBox(pChild); break;
		case UIElementType::ToggleButton: pElementInfo = ReadElementToggleButton(pChild); break;
		case UIElementType::ScrollBar: pElementInfo = ReadElementScrollBar(pChild); break;
		case UIElementType::ProgressBar: pElementInfo = ReadElementProgressBar(pChild); break;
		case UIElementType::Static: pElementInfo = ReadElementStatic(pChild); break;
		default: break;
		}

		if (pElementInfo)
		{
			JC_DELETE_SAFE(pElementInfo);
		}
		pChild = pChild->NextSiblingElement();
	}

	Loaded();
	return true;
}

void UIInfoLoader::ReadElementCommon(tinyxml2::XMLElement* _elementRoot, UIElementInfo* _pElementInfo)
{
	const char* name = _elementRoot->Attribute("name");
	if (name)
	{
		strcpy_s(_pElementInfo->name_, name);
	}
	_pElementInfo->hAlignment_ = (HAlignment_t)XmlIntAttr(_elementRoot, "halign", 0);
	_pElementInfo->vAlignment_ = (VAlignment_t)XmlIntAttr(_elementRoot, "valign", 0);
	_pElementInfo->type_ = (UIElementType_t)XmlIntAttr(_elementRoot, "type", 0);
}

void UIInfoLoader::ReadElementGroup(tinyxml2::XMLElement* _groupRoot, UIGroupInfo* _pGroupInfo)
{
	_pGroupInfo->size_.width = XmlFloatAttr(_groupRoot, "width", 0);
	_pGroupInfo->size_.height = XmlFloatAttr(_groupRoot, "height", 0);

	tinyxml2::XMLElement* pChild = _groupRoot->FirstChildElement();
	while (pChild)
	{
		UIGroupElemInfo elemInfo;
		const char* childName = pChild->Attribute("name");
		if (childName)
		{
			strcpy_s(elemInfo.name_, childName);
		}
		elemInfo.pos_.x = XmlFloatAttr(pChild, "x", 0);
		elemInfo.pos_.y = XmlFloatAttr(pChild, "y", 0);
		_pGroupInfo->infoList_.PushBack(elemInfo);
		pChild = pChild->NextSiblingElement();
	}
}

UIElementInfo* UIInfoLoader::ReadElementButton(tinyxml2::XMLElement* _buttonRoot)
{
	UIButtonInfo* pInfo = dbg_new UIButtonInfo();
	ReadElementCommon(_buttonRoot, pInfo);

	ImagePackManager* pPackManager = ImagePackManager::Get();

	const char* sgaName = _buttonRoot->Attribute("sga");
	const char* imageName = _buttonRoot->Attribute("img");

	if (sgaName && imageName)
	{
		ImagePack* pPack = pPackManager->GetPack(sgaName);
		pInfo->sga_ = pPack->GetPackIndex();
		pInfo->img_ = pPack->GetImgIndex(imageName);
	}

	pInfo->linearDodge_ = XmlBoolAttr(_buttonRoot, "linear_dodge", false);

	const char* sprite = _buttonRoot->Attribute("sprite");
	if (sprite)
	{
		sscanf(sprite, "%d,%d,%d,%d", &pInfo->sprites_[0], &pInfo->sprites_[1], &pInfo->sprites_[2], &pInfo->sprites_[3]);
	}

	return pInfo;
}

UIElementInfo* UIInfoLoader::ReadElementLabel(tinyxml2::XMLElement* _labelRoot)
{
	UILabelInfo* pInfo = dbg_new UILabelInfo();
	ReadElementCommon(_labelRoot, pInfo);

	const char* fontName = _labelRoot->Attribute("font");
	if (fontName)
	{
		pInfo->fontCode_ = FontManager::Get()->GetFontCode(fontName);
	}
	pInfo->fontSize_ = XmlIntAttr(_labelRoot, "font_size", 16);

	const char* colorStr = _labelRoot->Attribute("font_color");
	if (colorStr)
	{
		int r, g, b, a;
		sscanf(colorStr, "%d,%d,%d,%d", &r, &g, &b, &a);
		pInfo->fontColor_ = { (GLubyte)r, (GLubyte)g, (GLubyte)b, (GLubyte)a };
	}

	pInfo->textWrap_ = XmlBoolAttr(_labelRoot, "text_wrap", true);
	pInfo->textHAlignment_ = (HAlignment_t)XmlIntAttr(_labelRoot, "text_halign", 0);
	pInfo->textVAlignment_ = (VAlignment_t)XmlIntAttr(_labelRoot, "text_valign", 0);

	const char* text = _labelRoot->Attribute("text");
	if (text) pInfo->text_ = text;

	pInfo->size_.width = XmlFloatAttr(_labelRoot, "width", 0);
	pInfo->size_.height = XmlFloatAttr(_labelRoot, "height", 0);

	return pInfo;
}

UIElementInfo* UIInfoLoader::ReadElementSprite(tinyxml2::XMLElement* _spriteRoot)
{
	UISpriteInfo* pInfo = dbg_new UISpriteInfo();
	ReadElementCommon(_spriteRoot, pInfo);

	ImagePackManager* pPackManager = ImagePackManager::Get();

	const char* sgaName = _spriteRoot->Attribute("sga");
	const char* imageName = _spriteRoot->Attribute("img");

	if (sgaName && imageName)
	{
		ImagePack* pPack = pPackManager->GetPack(sgaName);
		pInfo->sga_ = pPack->GetPackIndex();
		pInfo->img_ = pPack->GetImgIndex(imageName);
	}

	pInfo->sprite_ = XmlIntAttr(_spriteRoot, "sprite", InvalidValue_v);
	pInfo->linearDodge_ = XmlBoolAttr(_spriteRoot, "linear_dodge", false);
	pInfo->scale9_ = XmlBoolAttr(_spriteRoot, "scale9", false);
	pInfo->size_.width = XmlFloatAttr(_spriteRoot, "width", 0);
	pInfo->size_.height = XmlFloatAttr(_spriteRoot, "height", 0);

	return pInfo;
}

UIElementInfo* UIInfoLoader::ReadElementEditBox(tinyxml2::XMLElement* _editBoxRoot)
{
	UIEditBoxInfo* pInfo = dbg_new UIEditBoxInfo();
	ReadElementCommon(_editBoxRoot, pInfo);

	pInfo->Size.width = XmlFloatAttr(_editBoxRoot, "width", 0);
	pInfo->Size.height = XmlFloatAttr(_editBoxRoot, "height", 0);
	pInfo->FontSize = XmlIntAttr(_editBoxRoot, "font_size", 16);
	pInfo->TextHAlignment = (HAlignment_t)XmlIntAttr(_editBoxRoot, "text_halign", 0);

	const char* fontColor = _editBoxRoot->Attribute("font_color");
	if (fontColor)
	{
		int r, g, b, a;
		sscanf(fontColor, "%d,%d,%d,%d", &r, &g, &b, &a);
		pInfo->FontColor = { (GLubyte)r, (GLubyte)g, (GLubyte)b, (GLubyte)a };
	}

	const char* pText = _editBoxRoot->Attribute("p_text");
	if (pText) pInfo->PlaceholderText = pText;

	const char* pFontColor = _editBoxRoot->Attribute("p_font_color");
	if (pFontColor)
	{
		int r, g, b, a;
		sscanf(pFontColor, "%d,%d,%d,%d", &r, &g, &b, &a);
		pInfo->PlaceHolderFontColor = { (GLubyte)r, (GLubyte)g, (GLubyte)b, (GLubyte)a };
	}

	pInfo->PlaceholderFontSize = XmlIntAttr(_editBoxRoot, "p_font_size", 16);
	pInfo->MaxLength = XmlIntAttr(_editBoxRoot, "max_length", 50);
	pInfo->InputMode = (EditBoxInputMode)XmlIntAttr(_editBoxRoot, "input_mode", 0);

	return pInfo;
}

UIElementInfo* UIInfoLoader::ReadElementCheckBox(tinyxml2::XMLElement* _checkBoxRoot)
{
	UICheckBoxInfo* pInfo = dbg_new UICheckBoxInfo();
	ReadElementCommon(_checkBoxRoot, pInfo);

	ImagePackManager* pPackManager = ImagePackManager::Get();

	const char* bgSga = _checkBoxRoot->Attribute("bg_sga");
	const char* bgImg = _checkBoxRoot->Attribute("bg_img");
	const char* crossSga = _checkBoxRoot->Attribute("cross_sga");
	const char* crossImg = _checkBoxRoot->Attribute("cross_img");

	if (bgSga && bgImg)
	{
		ImagePack* pBgPack = pPackManager->GetPackUnsafe(bgSga);
		if (pBgPack)
		{
			pInfo->BackgroundSga = pBgPack->GetPackIndex();
			pInfo->BackgroundImg = pBgPack->GetImgIndex(bgImg);
		}
	}

	if (crossSga && crossImg)
	{
		ImagePack* pCrossPack = pPackManager->GetPack(crossSga);
		pInfo->CrossSga = pCrossPack->GetPackIndex();
		pInfo->CrossImg = pCrossPack->GetImgIndex(crossImg);
	}

	pInfo->Check = XmlBoolAttr(_checkBoxRoot, "check", false);

	const char* sprite = _checkBoxRoot->Attribute("sprite");
	if (sprite)
	{
		sscanf(sprite, "%d,%d,%d,%d", &pInfo->Sprites[0], &pInfo->Sprites[1], &pInfo->Sprites[2], &pInfo->Sprites[3]);
	}

	jc_assert_msg(pInfo->Sprites[UICheckBox::INDEX_CROSS] != InvalidValue_v, "Checkbox missing cross image sprite.");
	return pInfo;
}

UIElementInfo* UIInfoLoader::ReadElementToggleButton(tinyxml2::XMLElement* _toggleButtonRoot)
{
	UIToggleButtonInfo* pInfo = dbg_new UIToggleButtonInfo();
	ReadElementCommon(_toggleButtonRoot, pInfo);

	ImagePackManager* pPackManager = ImagePackManager::Get();

	const char* sgaName = _toggleButtonRoot->Attribute("sga");
	const char* imageName = _toggleButtonRoot->Attribute("img");

	if (sgaName && imageName)
	{
		ImagePack* pPack = pPackManager->GetPack(sgaName);
		pInfo->Sga = pPack->GetPackIndex();
		pInfo->Img = pPack->GetImgIndex(imageName);
	}

	pInfo->LinearDodge = XmlBoolAttr(_toggleButtonRoot, "linear_dodge", false);

	const char* sprite = _toggleButtonRoot->Attribute("sprite");
	if (sprite)
	{
		sscanf(sprite, "%d,%d,%d,%d", &pInfo->Sprites[0][0], &pInfo->Sprites[0][1], &pInfo->Sprites[0][2], &pInfo->Sprites[0][3]);
	}

	const char* sprite2 = _toggleButtonRoot->Attribute("sprite2");
	if (sprite2)
	{
		sscanf(sprite2, "%d,%d,%d,%d", &pInfo->Sprites[1][0], &pInfo->Sprites[1][1], &pInfo->Sprites[1][2], &pInfo->Sprites[1][3]);
	}

	return pInfo;
}

UIElementInfo* UIInfoLoader::ReadElementScrollBar(tinyxml2::XMLElement* _scrollBarRoot)
{
	UIScrollBarInfo* pInfo = dbg_new UIScrollBarInfo();
	ReadElementCommon(_scrollBarRoot, pInfo);

	ImagePackManager* pPackManager = ImagePackManager::Get();

	const char* sgaName = _scrollBarRoot->Attribute("sga");
	const char* imageName = _scrollBarRoot->Attribute("img");

	if (sgaName && imageName)
	{
		ImagePack* pPack = pPackManager->GetPack(sgaName);
		pInfo->Sga = pPack->GetPackIndex();
		pInfo->Img = pPack->GetImgIndex(imageName);
	}

	pInfo->TrackSize.width = XmlFloatAttr(_scrollBarRoot, "track_width", 0);
	pInfo->TrackSize.height = XmlFloatAttr(_scrollBarRoot, "track_height", 0);

	const char* sprite = _scrollBarRoot->Attribute("sprite");
	if (sprite)
	{
		sscanf(sprite, "%d,%d,%d,%d,%d,%d,%d",
			&pInfo->Sprites[0], &pInfo->Sprites[1], &pInfo->Sprites[2],
			&pInfo->Sprites[3], &pInfo->Sprites[4], &pInfo->Sprites[5],
			&pInfo->Sprites[6]);
	}

	return pInfo;
}

UIElementInfo* UIInfoLoader::ReadElementProgressBar(tinyxml2::XMLElement* _progressBarRoot)
{
	UIProgressBarInfo* pInfo = dbg_new UIProgressBarInfo();
	ReadElementCommon(_progressBarRoot, pInfo);

	ImagePackManager* pPackManager = ImagePackManager::Get();

	const char* sgaName = _progressBarRoot->Attribute("sga");
	const char* imageName = _progressBarRoot->Attribute("img");

	if (sgaName && imageName)
	{
		ImagePack* pPack = pPackManager->GetPack(sgaName);
		pInfo->Sga = pPack->GetPackIndex();
		pInfo->Img = pPack->GetImgIndex(imageName);
	}

	pInfo->Sprite = XmlIntAttr(_progressBarRoot, "sprite", InvalidValue_v);
	pInfo->Size.width = XmlFloatAttr(_progressBarRoot, "width", 0);
	pInfo->Size.height = XmlFloatAttr(_progressBarRoot, "height", 0);
	pInfo->ProgressIncreaseDirection = (ProgressIncreaseDirection_t)XmlIntAttr(_progressBarRoot, "direction", 0);

	return pInfo;
}

UIElementInfo* UIInfoLoader::ReadElementStatic(tinyxml2::XMLElement* _staticRoot)
{
	UIStaticInfo* pInfo = dbg_new UIStaticInfo();
	ReadElementCommon(_staticRoot, pInfo);

	pInfo->Size.width = XmlFloatAttr(_staticRoot, "width", 0);
	pInfo->Size.height = XmlFloatAttr(_staticRoot, "height", 0);

	return pInfo;
}