#pragma once

#include "sg/Struct/SteinsGate_UI.h"
#include "sg/Util/DescLoaderAbstract.h"
#include "tinyxml2.h"

struct UIInfoLoader : DescLoaderAbstract
{
public:
	UIInfoLoader();
	ConfigFileType_t GetConfigFileType() override { return ConfigFileType::UI; }

	bool Load() override;

	static void ReadElementCommon(tinyxml2::XMLElement* _elementRoot, OUT UIElementInfo* _pElementInfo);
	static void ReadElementGroup(tinyxml2::XMLElement* _groupRoot, OUT UIGroupInfo* _pGroupInfo);
	static UIElementInfo* ReadElementButton(tinyxml2::XMLElement* _buttonRoot);
	static UIElementInfo* ReadElementLabel(tinyxml2::XMLElement* _labelRoot);
	static UIElementInfo* ReadElementSprite(tinyxml2::XMLElement* _spriteRoot);
	static UIElementInfo* ReadElementEditBox(tinyxml2::XMLElement* _editBoxRoot);
	static UIElementInfo* ReadElementCheckBox(tinyxml2::XMLElement* _checkBoxRoot);
	static UIElementInfo* ReadElementToggleButton(tinyxml2::XMLElement* _toggleButtonRoot);
	static UIElementInfo* ReadElementScrollBar(tinyxml2::XMLElement* _scrollBarRoot);
	static UIElementInfo* ReadElementProgressBar(tinyxml2::XMLElement* _progressBarRoot);
	static UIElementInfo* ReadElementStatic(tinyxml2::XMLElement* _staticRoot);

	static constexpr const char* XML_ROOT_TAG = "Group";
};